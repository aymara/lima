// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <iterator>
#include <set>
#include <algorithm>
#include <typeinfo>
#include <locale>
#include <codecvt>

#include <QJsonDocument>

#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/common/helpers/ConfigurationHelper.h"
#include "linguisticProcessing/common/helpers/LangCodeHelpers.h"

#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/common_runtime/local_device.h"
#include "tensorflow/cc/client/client_session.h"

#include "fastText/src/fasttext.h"

#include "Viterbi.h"
#include "Arborescence.h"
#include "QJsonHelpers.h"
#include "TensorFlowHelpers.h"
#include "Cache.h"

#include "TensorFlowMorphoSyntax.h"


#define DEBUG_THIS_FILE true

using namespace boost;
using namespace std;
using namespace tensorflow;
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::TensorFlowUnits::Common;

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace MorphoSyntax
{

SimpleFactory<MediaProcessUnit, TensorFlowMorphoSyntax> tensorflowmorphosyntaxFactory(TENSORFLOWMORPHOSYNTAX_CLASSID);

#define LOG_ERROR_AND_THROW(msg, exc) { \
                                        LIMA_EXCEPTION_SELECT_LOGINIT(TENSORFLOWMORPHOSYNTAXLOGINIT, msg, exc ); \
                                      }

#if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
  #define LOG_MESSAGE(stream, msg) { stream << msg; }
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) TENSORFLOWMORPHOSYNTAXLOGINIT; LOG_MESSAGE(stream, msg);
#else
  #define LOG_MESSAGE(stream, msg) ;
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif

CONFIGURATIONHELPER_LOGGING_INIT(TENSORFLOWMORPHOSYNTAXLOGINIT);

class TensorFlowMorphoSyntaxPrivate : public ConfigurationHelper
{
public:
  TensorFlowMorphoSyntaxPrivate()
    : ConfigurationHelper("TensorFlowMorphoSyntaxPrivate", THIS_FILE_LOGGING_CATEGORY()),
      m_stringsPool(nullptr),
      m_loaded(false)
  { }
  ~TensorFlowMorphoSyntaxPrivate();

  void init(GroupConfigurationStructure&, MediaId lang);
  LimaStatusCode process(AnalysisContent& analysis);

private:
  void add_linguistic_element(MorphoSyntacticData* msdata, const Token &token) const;

  MediaId m_language;
  FsaStringsPool* m_stringsPool;

protected:
  struct TToken
  {
    LinguisticGraphVertex vertex;
    Token *token;
    MorphoSyntacticData *morpho;

    TToken()
      : vertex(0), token(nullptr), morpho(nullptr) { }
    TToken(LinguisticGraphVertex v, Token *t, MorphoSyntacticData *m)
      : vertex(v), token(t), morpho(m) { }
  };

  struct TSentence
  {
    LinguisticGraphVertex begin;
    LinguisticGraphVertex end;

    vector<TToken> tokens;
    size_t token_count;
    size_t offset;

    TSentence(LinguisticGraphVertex b, LinguisticGraphVertex e, size_t max_len)
      : begin(b), end(e), token_count(0), offset(0)
    {
      tokens.resize(max_len);
    }

    void append(LinguisticGraphVertex v, Token *t, MorphoSyntacticData *m)
    {
#ifdef DEBUG_LP
      TENSORFLOWMORPHOSYNTAXLOGINIT;
      LDEBUG << "TSentence::append" << v << t->stringForm();
#endif
      if (tokens.size() <= token_count)
      {
        tokens.resize(tokens.size() * 2);
        QString errorString;
        QTextStream ts(&errorString);
        //ts << "TSentence::append tokens size (" << tokens.size() << ") <= token_count (" << token_count << ")";
        //TENSORFLOWMORPHOSYNTAXLOGINIT;
        //LWARN << errorString;
      }

      tokens[token_count].vertex = v;
      tokens[token_count].token = t;
      tokens[token_count].morpho = m;
      token_count += 1;
    }
  };

  struct SeqTagOutput
  {
    string full_name;
    string feat_name;
    string crf_node_name;
    string logits_node_name;
    vector<vector<float>> crf;

    vector<string> i2t;         // index -> UD label
    vector<LinguisticCode> i2c; // index -> LinguisticCode

    const PropertyAccessor* accessor;
  };

  struct DepparseOutput
  {
    string full_name;
    string feat_name;
    string arcs_logits_node_name;
    string arcs_pred_node_name;
    string tags_logits_node_name;
    string tags_pred_node_name;
    vector<string> i2t;
    size_t root_tag_idx;
  };

  struct LemmatizerConf
  {
    template <class S>
    struct dict {
      dict()
          : accessor(nullptr) { }

      vector<S> m_i2w;
      map<S, unsigned int> m_w2i;

      map<LinguisticCode, unsigned int> m_c2i; // LinguisticCode -> index

      const PropertyAccessor* accessor;

      void fill_linguistic_codes(const PropertyManager& pm)
      {
        for (const auto& kv : m_w2i)
        {
          if (kv.first == "#None")
            m_c2i[L_NONE] = kv.second;
          else
          {
            LinguisticCode code = pm.getPropertyValue(kv.first);
            m_c2i[code] = kv.second;
          }
        }
      }
    };

    dict<u32string> encoder_dict;
    dict<u32string> decoder_dict;
    dict<string> pos_dict;
    map<string, dict<string>> feature_dicts;

    string sample_id_node_name;
  };

  vector<SeqTagOutput> m_seqtag_outputs;
  map<string, size_t> m_seqtag_id2idx;
  vector<DepparseOutput> m_depparse_outputs;
  map<string, size_t> m_depparse_id2idx;

  vector<string> m_feat_order;
  vector<set<string>> m_feat_deps;
  LimaString m_main_alphabet;
  size_t m_upos_idx;

  size_t m_max_seq_len;
  size_t m_max_word_len;
  size_t m_batch_size;

  map<u32string, unsigned int> m_char2idx;
  map<u32string, unsigned int> m_word2idx;
  u32string m_unk;
  u32string m_eos;

  map<string, string> m_input_node_names;

  unique_ptr<Session> m_session;
  GraphDef m_graph_def;

  fasttext::FastText m_fasttext;
  LRUCache<StringsPoolIndex, fasttext::Vector> m_fasttext_cache;

  function<void()> m_load_fn;
  bool m_loaded;

  void load_config(const QString& config_file_name);
  void load_graph(const QString& model_path, GraphDef* graph_def);
  void load_output_description(const QJsonObject& jso);
  bool fill_linguistic_codes(const PropertyCodeManager& pcm, SeqTagOutput& out);
  void init_crf();

  void analyze(vector<TSentence>& sentences, SyntacticData& syntacticData);
  void generate_batch(const vector<TSentence>& sentences,
                      size_t start,
                      size_t size,
                      vector<pair<string, Tensor>>& batch);

  void save_arcs_logits(const TSentence& sent,
                        const TTypes<float, 3>::Tensor& logits,
                        int64 idx,
                        const string& prefix,
                        size_t id) const;
  size_t fix_tag_for_no_root_link(const DepparseOutput& out_descr,
                                  std::ptrdiff_t pred_tag,
                                  std::ptrdiff_t pos_in_batch,
                                  std::ptrdiff_t word_num,
                                  const TTypes<float, 3>::Tensor& tags_logits) const;
  void fixMissingFeature(const vector<vector<float>>& converted_scores,
                         const string& feat_name,
                         TToken& t,
                         size_t a,
                         size_t& pred);

  // Json parsing
  QJsonObject get_json_object(const QJsonObject& root, const QString& child_name);
  QJsonArray get_json_array(const QJsonObject& root, const QString& child_name);
};

TensorFlowMorphoSyntax::TensorFlowMorphoSyntax()
    : m_d(new TensorFlowMorphoSyntaxPrivate())
{
}

TensorFlowMorphoSyntax::~TensorFlowMorphoSyntax()
{
  delete m_d;
}

void TensorFlowMorphoSyntax::init(
  GroupConfigurationStructure& gcs,
  Manager* manager)
{
  m_d->init(gcs, manager->getInitializationParameters().media);
}

void TensorFlowMorphoSyntaxPrivate::init(
  GroupConfigurationStructure& gcs,
  MediaId lang)
{
  m_language = lang;
  m_stringsPool = &MediaticData::changeable().stringsPool(m_language);

  QString lang_str = MediaticData::single().media(m_language).c_str();
  QString resources_path = MediaticData::single().getResourcesPath().c_str();
  QString model_name = getStringParameter(gcs, "model_prefix", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY).c_str();
  QString embeddings = getStringParameter(gcs, "embeddings", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY).c_str();
  string udlang;
  MediaticData::single().getOptionValue("udlang", udlang);

  if (!fix_lang_codes(lang_str, udlang))
  {
    LIMA_EXCEPTION_SELECT_LOGINIT(TOKENIZERLOGINIT,
      "TensorFlowMorphoSyntaxPrivate::init: Can't parse language id " << udlang.c_str(),
      Lima::InvalidConfiguration);
  }

  model_name.replace(QString("$udlang"), QString(udlang.c_str()));

  auto config_file_name = findFileInPaths(resources_path,
                                          QString::fromUtf8("/TensorFlowMorphoSyntax/%1/%2.conf")
                                            .arg(lang_str).arg(model_name));
  if (config_file_name.isEmpty())
  {
    throw InvalidConfiguration("TensorFlowMorphoSyntaxPrivate::init: config file not found.");
  }

  auto model_file_name = findFileInPaths(resources_path,
                                         QString::fromUtf8("/TensorFlowMorphoSyntax/%1/%2.model")
                                           .arg(lang_str).arg(model_name));
  if (model_file_name.isEmpty())
  {
    throw InvalidConfiguration("TensorFlowMorphoSyntaxPrivate::init: model file not found.");
  }

  QString embeddings_name = embeddings;
  embeddings_name.replace(QString("$udlang"), QString(udlang.c_str()));

  auto embeddings_file_name = findFileInPaths(resources_path,
                                              QString::fromUtf8("/TensorFlowMorphoSyntax/%1/%2")
                                                .arg(lang_str).arg(embeddings_name));
  if (embeddings_file_name.isEmpty())
  {
    throw InvalidConfiguration("TensorFlowMorphoSyntaxPrivate::init: embeddings file not found.");
  }

  m_load_fn = [this, config_file_name, model_file_name, embeddings_file_name]()
  {
    if (m_loaded)
    {
      return;
    }

    load_config(config_file_name);

    // Following line requires patched version of TensorFlow.
    // It only helps to increase analysis speed and can be commented out.
    // Commenting out to avoid problems using a non-patched TensorFlow
//     tensorflow::LocalDevice::set_use_global_threadpool(false);

    tensorflow::SessionOptions options;
    tensorflow::ConfigProto & config = options.config;
    config.set_inter_op_parallelism_threads(2);
    config.set_intra_op_parallelism_threads(1);
    config.set_use_per_session_threads(true);
    config.set_isolate_session_state(true);
    Session* session = nullptr;
    Status status = NewSession(options, &session);
    if (!status.ok())
    {
      LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::init: Can't create TensorFlow session: "
                          << status.ToString().c_str(),
                          LimaException);
    }
    m_session = unique_ptr<Session>(session);

    load_graph(model_file_name, &m_graph_def);

    // Add the graph to the session
    status = m_session->Create(m_graph_def);
    if (!status.ok())
    {
      LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::init: Can't add graph to TensorFlow session: "
                          << status.ToString().c_str(),
                          LimaException);
    }

    init_crf();

    m_unk = QString("<<unk>>").toStdU32String();
    m_eos = QString("<<eos>>").toStdU32String();

    m_fasttext.loadModel(embeddings_file_name.toStdString());

    m_loaded = true;
  };

  if (!isInitLazy())
  {
    m_load_fn();
  }
}

QJsonObject TensorFlowMorphoSyntaxPrivate::get_json_object(const QJsonObject& root, const QString& child_name)
{
  QJsonValue value = root.value(child_name);
  if (value.isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::get_json_object can't get value \""
                        << child_name << "\"", LimaException);

  QJsonObject object = value.toObject();

  //if (object.isEmpty())
  //  LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::get_json_object object \""
  //                      << child_name << "\" is empty", LimaException);

  return object;
}

QJsonArray TensorFlowMorphoSyntaxPrivate::get_json_array(const QJsonObject& root, const QString& child_name)
{
  QJsonValue value = root.value(child_name);
  if (value.isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::get_json_object can't get value \""
                        << child_name << "\"", LimaException);

  QJsonArray array = value.toArray();

  return array;
}

void TensorFlowMorphoSyntaxPrivate::init_crf()
{
  Status status;

  for (SeqTagOutput& out : m_seqtag_outputs)
  {
    vector<Tensor> res;
    status = m_session->Run({}, {out.crf_node_name}, {}, &res);
    if (!status.ok())
      LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::init: Can't execute \"Run\" in TensorFlow session: "
                          << status.ToString().c_str(),
                          LimaException);

    auto crf = res[0].matrix<float>();

    out.crf.resize(res[0].dim_size(0));
    for (int64 j = 0; j < res[0].dim_size(0); j++)
    {
      out.crf[j].resize(res[0].dim_size(1));
      for (int64 k = 0; k < res[0].dim_size(1); k++)
        out.crf[j][k] = crf(j, k);
    }
  }
}

void TensorFlowMorphoSyntaxPrivate::add_linguistic_element(MorphoSyntacticData* msdata, const Token &token) const
{
  if (msdata == nullptr)
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntaxPrivate::add_linguistic_element: msdata == nullptr",
                        LimaException);

  LinguisticElement elem;
  elem.inflectedForm = token.form();
  elem.lemma = token.form();
  elem.normalizedForm = token.form();
  elem.type = SIMPLE_WORD;

  msdata->push_back(elem);
}

LimaStatusCode TensorFlowMorphoSyntax::process(AnalysisContent& analysis) const
{
  return m_d->process(analysis);
}

LimaStatusCode TensorFlowMorphoSyntaxPrivate::process(AnalysisContent& analysis)
{
  m_load_fn();

  TimeUtils::updateCurrentTime();

  LOG_MESSAGE_WITH_PROLOG(LINFO, "Start of TensorFlowMorphoSyntax");

  auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"));
  AnalysisGraph* posgraph = new AnalysisGraph("PosGraph", m_language, false, false, *anagraph);
  analysis.setData("PosGraph", posgraph);
  LinguisticGraph* src_graph = anagraph->getGraph();

  /** Creation of an annotation graph if necessary */
  auto annotationData = std::dynamic_pointer_cast<AnnotationData>(analysis.getData("AnnotationData"));
  if (annotationData == 0)
  {
    annotationData = std::shared_ptr<AnnotationData>();

    /** Creates a node in the annotation graph for each node of the
      * morphosyntactic graph. Each new node is annotated with the name mrphv and
      * associated to the morphosyntactic vertex number */
    anagraph->populateAnnotationGraph(annotationData.get(), "AnalysisGraph");
    posgraph->populateAnnotationGraph(annotationData.get(), "PosGraph");

    LinguisticGraphVertexIt it, it_end;
    boost::tie(it, it_end) = vertices(*src_graph);
    for (; it != it_end; it++)
    {
      if (annotationData->matches("AnalysisGraph", *it, "PosGraph").empty())
      {
        annotationData->addMatching("AnalysisGraph", *it, "PosGraph", *it);
      }
    }

    analysis.setData("AnnotationData", annotationData);
  }

  auto sb = std::dynamic_pointer_cast<SegmentationData>(analysis.getData("SentenceBoundaries"));
  if (sb == nullptr)
  {
    LOG_MESSAGE(LERROR, "No SentenceBounds");
    return MISSING_DATA;
  }

  auto newSb = std::make_shared<SegmentationData>("PosGraph");
  for (const auto& segment: sb->getSegments())
  {
    auto firstVxMatches = annotationData->matches("AnalysisGraph",
                                                  segment.getFirstVertex(),
                                                  "PosGraph");
    if (segment.getFirstVertex() == 0)
      firstVxMatches.insert(0);

    if (firstVxMatches.empty())
    {
      LOG_MESSAGE(LERROR, "No PoS graph vertex for segment first AnalysisGraph vextex"
                          << segment.getFirstVertex());
      continue;
    }

    auto lastVxMatches = annotationData->matches("AnalysisGraph",
                                                 segment.getLastVertex(),
                                                 "PosGraph");
    if (segment.getFirstVertex() == 1)
      firstVxMatches.insert(1);

    if (lastVxMatches.empty())
    {
      LOG_MESSAGE(LERROR, "No PoS graph vertex for segment last AnalysisGraph vextex"
                          << segment.getLastVertex());
      continue;
    }
    Segment newSegment("sentence", *firstVxMatches.begin(), *lastVxMatches.begin(), posgraph);
    newSb->add(newSegment);
  }

  analysis.removeData("SentenceBoundaries");
  analysis.setData("SentenceBoundaries", newSb);

  sb = newSb;

  if (sb->getGraphId() != "PosGraph") {
    LOG_MESSAGE(LERROR, "SentenceBounds have been computed on " << sb->getGraphId() << " !");
    LOG_MESSAGE(LERROR, "SentenceBounds must be on PosGraph");
    return INVALID_CONFIGURATION;
  }

  SyntacticData* syntacticData = new SyntacticData(posgraph, 0);
  analysis.setData("SyntacticData", syntacticData);

  // graph is empty if it has only 2 vertices, start (0) and end (0)
  if (num_vertices(*src_graph) <= 2)
  {
      return SUCCESS_ID;
  }

  LinguisticGraph* result_graph = posgraph->getGraph();
  //remove_edge(posgraph->firstVertex(), posgraph->lastVertex(), *result_graph);

  vector<TSentence> sentences;
  LinguisticGraphVertex prevSentenceEnd = LinguisticGraphVertex(0);

  for (vector<Segment>::const_iterator boundItr = (sb->getSegments()).begin();
       boundItr != (sb->getSegments()).end();
       boundItr++)
  {
    LinguisticGraphVertex beginSentence = boundItr->getFirstVertex();
    LinguisticGraphVertex endSentence = boundItr->getLastVertex();
    LOG_MESSAGE(LDEBUG, "analyze sentence from vertex " << beginSentence
                << " to vertex " << endSentence);

    TSentence sent(beginSentence, endSentence, m_max_seq_len - 1);

    LinguisticGraphVertex curr = beginSentence;
    while (curr != endSentence)
    {
      if (sentences.size() == 0 || (sentences.size() > 0 && prevSentenceEnd != curr))
      {
        Token* token = get(vertex_token, *result_graph, curr);
        MorphoSyntacticData* morpho_data = get(vertex_data, *result_graph, curr);

        if (token != nullptr)
          sent.append(curr, token, morpho_data);
      }

      LinguisticGraphOutEdgeIt it, it_end;
      boost::tie(it, it_end) = out_edges(curr, *result_graph);
      set<LinguisticGraphVertex> out_set;
      while (it != it_end)
      {
        out_set.insert(target(*it, *result_graph));
        it++;
      }
      if (out_set.size() > 1)
      {
        LOG_MESSAGE(LERROR, "Non unique out path from vertex ");
        return UNKNOWN_ERROR;
      }
      if (out_set.size() == 0)
      {
        LOG_MESSAGE(LERROR, "Zero out paths from vertex ");
        return UNKNOWN_ERROR;
      }
      curr = *out_set.begin();
    }

    Token* token = get(vertex_token, *result_graph, curr);
    MorphoSyntacticData* morpho_data = get(vertex_data, *result_graph, curr);

    if (token != nullptr)
    {
      sent.append(curr, token, morpho_data);
      prevSentenceEnd = curr;
    }

    if (sent.token_count < m_max_seq_len - 1)
      sentences.push_back(sent);
    else
    {
      size_t offset = 0;
      while (offset < sent.token_count)
      {
        TSentence s(sent.begin, sent.end, m_max_seq_len - 1);
        size_t m = sent.token_count - offset;
        if (m > m_max_seq_len - 1)
          m = m_max_seq_len - 1;
        for (size_t j = 0; j < m; j++)
          s.tokens[j] = sent.tokens[offset + j];
        s.token_count = m;

        s.offset = offset;
        sentences.push_back(s);

        offset += m;
      }
    }
  }

  syntacticData->setupDependencyGraph();

  TimeUtils::logElapsedTime("TensorFlowMorphoSyntax::process");

  analyze(sentences, *syntacticData);

  LOG_MESSAGE(LINFO, "End of TensorFlowMorphoSyntax");

  return SUCCESS_ID;
}

size_t TensorFlowMorphoSyntaxPrivate::fix_tag_for_no_root_link(const DepparseOutput& out_descr,
                                                               std::ptrdiff_t pred_tag,
                                                               std::ptrdiff_t pos_in_batch,
                                                               std::ptrdiff_t word_num,
                                                               const TTypes<float, 3>::Tensor& tags_logits) const
{
  size_t new_pred_tag = pred_tag;

  if (pred_tag == out_descr.root_tag_idx || out_descr.i2t[pred_tag] == "ud:_")
  {
    // pred_head != 0 => this isn't a root => we have to change tag
    std::ptrdiff_t a = 0;
    float best_score = tags_logits(pos_in_batch, word_num, a);
    while (a == out_descr.root_tag_idx || a == pred_tag || out_descr.i2t[a] == "ud:_")
    {
      a++;
      best_score = tags_logits(pos_in_batch, word_num, a);
    }

    new_pred_tag = a;
    while (a < tags_logits.dimension(2))
    {
      float score = tags_logits(pos_in_batch, word_num, a);
      LIMA_UNUSED(score);
      if (tags_logits(pos_in_batch, word_num, a) > best_score
              && a != out_descr.root_tag_idx
              && out_descr.i2t[a] != "ud:_")
      {
        best_score = tags_logits(pos_in_batch, word_num, a);
        new_pred_tag = a;
      }
      a += 1;
    }
  }

  return new_pred_tag;
}

void TensorFlowMorphoSyntaxPrivate::analyze(vector<TSentence>& sentences,
                                            SyntacticData& syntacticData)
{
  TimeUtils::updateCurrentTime();
  const LanguageData& ld = static_cast<const LanguageData&>(MediaticData::single().mediaData(m_language));

  vector<string> requested_nodes;
  for (auto& out : m_seqtag_outputs)
  {
    requested_nodes.push_back(out.logits_node_name);
  }

  for (auto& out : m_depparse_outputs)
  {
    requested_nodes.push_back(out.arcs_pred_node_name);
    requested_nodes.push_back(out.tags_pred_node_name);
    requested_nodes.push_back(out.arcs_logits_node_name);
    requested_nodes.push_back(out.tags_logits_node_name);
  }

  LinguisticGraphVertex prev_sent_head = 0;
  for (size_t i = 0; i < sentences.size();)
  {
    // Generate batch
    size_t this_batch_size = (i + m_batch_size > sentences.size()) ? sentences.size() - i : m_batch_size;
    vector<pair<string, Tensor>> inputs;
    generate_batch(sentences, i, this_batch_size, inputs);

    // Run model
    vector<Tensor> out;
    Status status = m_session->Run(inputs, requested_nodes, {}, &out);
    if (!status.ok())
      LOG_ERROR_AND_THROW("TensorFlowMorphoSyntaxPrivate::analyze: Can't execute \"Run\" in TensorFlow session: "
                          << status.ToString().c_str(),
                          LimaException);

    // Apply prediction to Lima graphs
    for (const string& feat_name : m_feat_order)
    {
      if (m_seqtag_id2idx.end() == m_seqtag_id2idx.find(feat_name))
          LOG_ERROR_AND_THROW("TensorFlowMorphoSyntaxPrivate::analyze: unknown feature: " << feat_name.c_str(), LimaException);
      size_t out_idx = m_seqtag_id2idx.find(feat_name)->second;
      auto scores = out[out_idx].tensor<float, 3>();
      for (int64 p = 0; p < out[out_idx].dim_size(0); p++)
      {
        TSentence& sent = sentences[i+p];
        size_t len = sent.token_count;
        if (0 == len)
          continue;

        vector<vector<float>> converted_scores;
        converted_scores.resize(len + 1);
        for (size_t j = 0; j < len + 1; j++)
        {
          converted_scores[j].resize(scores.dimension(2));
          for (int64 k = 0; k < scores.dimension(2); k++)
            converted_scores[j][k] = scores(p, j, k);
        }

        vector<size_t> viterbi;
        viterbi_decode(converted_scores, m_seqtag_outputs[out_idx].crf, viterbi);

        for (size_t a = 0; a < viterbi.size() - 1; ++a)
        {
          size_t pred = viterbi[a + 1];
          //if (m_seqtag_outputs[out_idx].i2c[pred] != LinguisticCode(0))
          {
            TToken& t = sent.tokens[a];
            if (nullptr != t.morpho)
            {
              if (t.morpho->size() == 0)
                add_linguistic_element(t.morpho, *(t.token));

              if (m_seqtag_outputs[out_idx].i2t[pred] == "#None")
              {
                fixMissingFeature(converted_scores, feat_name, t, a, pred);
              }

              m_seqtag_outputs[out_idx].accessor->writeValue(m_seqtag_outputs[out_idx].i2c[pred],
                                                             t.morpho->begin()->properties);
            }
          }
        }
      }
    }

    for (size_t out_idx = m_seqtag_outputs.size(); (out_idx - m_seqtag_outputs.size()) < m_depparse_outputs.size(); ++out_idx)
    {
      auto arcs_tensor = out[out_idx].tensor<int, 2>();
      auto tags_tensor = out[out_idx + 1].tensor<int, 2>();

      auto arcs_logits = out[out_idx + 2].tensor<float, 3>();
      auto tags_logits = out[out_idx + 3].tensor<float, 3>();

      const DepparseOutput& out_descr = *(m_depparse_outputs.begin());

      for (std::ptrdiff_t p = 0; p < arcs_tensor.dimension(0); p++)
      {
        TSentence& sent = sentences[i+p];
        size_t len = sent.token_count;
        if (0 == len)
          continue;

        if (sent.offset == 0)
          prev_sent_head = 0;

        //parents.reserve(m_max_seq_len);
        vector<size_t> parents;
        parents.resize(len + 1);
        arborescence<size_t, float>([&arcs_logits, p](std::ptrdiff_t i, std::ptrdiff_t j) -> float {
                                      return arcs_logits(p, i, j);
                                    },
                                    len + 1,
                                    parents);

        size_t root_counter = 0;
        for (size_t a = 1; a < len + 1; a++)
          if (parents[a] == 0)
            root_counter += 1;

        if (root_counter > 1)
        {
          LOG_MESSAGE_WITH_PROLOG(LERROR, "Multiple roots in sentence " << i + p);
        }
        //save_arcs_logits(sent, arcs_logits, p, "matrix_", i+p);

        for (size_t j = 1; j < len + 1; j++)
        {
          size_t pred_head = parents[j];
          size_t pred_tag = tags_tensor(p, j);

          if (sent.offset == 0 && pred_head == 0)
            prev_sent_head = sent.tokens[j-1].vertex; // we use this in case of splitted sentences

          TToken& t = sent.tokens[j-1];

          if (pred_head != 0)
          {
            pred_tag = fix_tag_for_no_root_link(out_descr, pred_tag, p, j, tags_logits);

            TToken& h = sent.tokens[pred_head - 1];
            syntacticData.addRelationNoChain(ld.getSyntacticRelationId(out_descr.i2t[pred_tag]),
                                             t.vertex,
                                             h.vertex);
          }
          else
          {
            if (sent.offset > 0)
            {
              pred_tag = fix_tag_for_no_root_link(out_descr, pred_tag, p, j, tags_logits);
              syntacticData.addRelationNoChain(ld.getSyntacticRelationId(out_descr.i2t[pred_tag]),
                                               t.vertex,
                                               prev_sent_head);
            }
            else
            {
              syntacticData.addRelationNoChain(ld.getSyntacticRelationId("root"),
                                               t.vertex,
                                               t.vertex);
            }
          }
        }
      }
    }

    i += this_batch_size;
  }

  TimeUtils::logElapsedTime("TensorFlowMorphoSyntax::analyze");
}

TensorFlowMorphoSyntaxPrivate::~TensorFlowMorphoSyntaxPrivate()
{
  if (m_session.get() != nullptr)
  {
    auto status = m_session->Close();
    if (!status.ok())
    {
      LOG_MESSAGE_WITH_PROLOG(LERROR, "TensorFlowMorphoSyntaxPrivate::~TensorFlowMorphoSyntaxPrivate(): Error closing session:"
                              << status.ToString());
    }
  }
}

void TensorFlowMorphoSyntaxPrivate::fixMissingFeature(const vector<vector<float>>& converted_scores,
                                                      const string& feat_name,
                                                      TToken& t,
                                                      size_t a,
                                                      size_t& pred)
{
  if (m_main_alphabet.size() > 0 && nullptr != t.token && m_main_alphabet.indexOf(t.token->stringForm()[0]) == -1)
    return;

  LinguisticCode lc_upos = m_seqtag_outputs[m_upos_idx].accessor->readValue(t.morpho->begin()->properties);
  string upos_str = "";
  size_t upos_val = 0;
  for (size_t ii = 0; ii < m_seqtag_outputs[m_upos_idx].i2c.size(); ii++)
    if (lc_upos == m_seqtag_outputs[m_upos_idx].i2c[ii])
    {
      upos_str = m_seqtag_outputs[m_upos_idx].i2t[ii];
      upos_val = ii;
      break;
    }

  if (upos_str.size() == 0)
    throw;

  if (m_feat_deps[upos_val].size() == 0)
    return;

  if (m_feat_deps[upos_val].end() != m_feat_deps[upos_val].find(feat_name))
  {
    size_t curr_idx = 0;
    float curr_val = -100500;
    for (size_t k = 1; k < converted_scores[a+1].size(); k++)
    {
      if (k == pred)
        continue;

      if (converted_scores[a+1][k] > curr_val)
      {
        curr_val = converted_scores[a+1][k];
        curr_idx = k;
      }
    }
    pred = curr_idx;
  }
}

void TensorFlowMorphoSyntaxPrivate::generate_batch(const vector<TSentence>& sentences,
                                                   size_t start,
                                                   size_t batch_size,
                                                   vector<pair<string, Tensor>>& batch)
{
  Tensor t_batch_size_tensor(DT_INT32, tensorflow::TensorShape());

  // words - characters
  // batch_size x max_seq_len x max_word_len
  Tensor t_words(DT_INT32, TensorShape({static_cast<long long>(batch_size),
                                        static_cast<long long>(m_max_seq_len),
                                        static_cast<long long>(m_max_word_len)
                                       }));

  // words_len - length of words in characters
  // batch_size x max_seq_len
  Tensor t_word_len(DT_INT32, TensorShape({static_cast<long long>(batch_size),
                                           static_cast<long long>(m_max_seq_len)
                                          }));

  // len - sentence length in words
  Tensor t_len(DT_INT32, TensorShape({static_cast<long long>(batch_size)}));

  // input_trainable - indices
  // batch_size x max_seq_len
  Tensor t_input_trainable(DT_INT32, TensorShape({static_cast<long long>(batch_size),
                                                  static_cast<long long>(m_max_seq_len)
                                                 }));

  // input_pretrained
  // batch_size x max_seq_len x embd_dim
  Tensor t_input_pretrained(DT_FLOAT, TensorShape({static_cast<long long>(batch_size),
                                                   static_cast<long long>(m_max_seq_len),
                                                   static_cast<long long>(m_fasttext.getDimension())
                                                  }));

  batch.push_back({ m_input_node_names.find("input_trainable")->second, t_input_trainable });
  batch.push_back({ m_input_node_names.find("input_pretrained")->second, t_input_pretrained });
  batch.push_back({ m_input_node_names.find("words")->second, t_words });
  batch.push_back({ m_input_node_names.find("word_len")->second, t_word_len });
  batch.push_back({ m_input_node_names.find("len")->second, t_len });
  batch.push_back({ m_input_node_names.find("batch_size")->second, t_batch_size_tensor });

  auto input_trainable = batch[0].second.tensor<int, 2>();
  auto input_pretrained = batch[1].second.tensor<float, 3>();
  auto words = batch[2].second.tensor<int, 3>();
  auto word_len = batch[3].second.tensor<int, 2>();
  auto len = batch[4].second.tensor<int, 1>();

  init_2d_tensor(input_trainable, 0);
  init_3d_tensor(input_pretrained, 0.0f);
  init_3d_tensor(words, 0);
  init_2d_tensor(word_len, 0);
  init_1d_tensor(len, 0);

  batch[5].second.scalar<int>()() = batch_size;

  unsigned int trainable_idx_unk = m_word2idx.find(m_unk)->second;
  unsigned int char_idx_unk = m_char2idx.find(m_unk)->second;

  for (size_t i = 0; i < batch_size; ++i)
  {
    const TSentence& sent = sentences[start + i];
    size_t j = 0;
    size_t n = 1;
    for (; j < sent.token_count; ++j)
    {
      n = j + 1;
      // input_trainable
      unsigned int idx = trainable_idx_unk;
      const TToken& t = sent.tokens[j];
      const LimaString& form = (*m_stringsPool)[t.token->form()];
      const u32string u32_form_lc = form.toLower().toStdU32String();
      map<u32string, unsigned int>::const_iterator it = m_word2idx.find(u32_form_lc);

      if (m_word2idx.end() != it)
        idx = it->second;

      input_trainable(i, n) = idx;

      // words
      const u32string u32_form = form.toStdU32String();
      size_t max_chars = u32_form.size() > m_max_word_len ? m_max_word_len : u32_form.size();
      int64 k = 0;
      for (; k < max_chars; ++k)
      {
        unsigned int char_idx = char_idx_unk;
        map<u32string, unsigned int>::const_iterator it = m_char2idx.find(u32_form.substr(k, 1));
        if (m_char2idx.end() != it)
          char_idx = it->second;
        words(i, n, k) = char_idx;
      }
      word_len(i, n) = k;

      // input pretrained
      fasttext::Vector vec(m_fasttext.getDimension());
      if (!m_fasttext_cache.get(t.token->form(), vec))
      {
        m_fasttext.getWordVector(vec, form.toUtf8().toStdString());
        m_fasttext_cache.put(t.token->form(), vec);
      }
      for (int64 a = 0; a < vec.size(); ++a)
        input_pretrained(i, n, a) = vec[a];
    }

    len(i) = n + 1;
    if (n + 1 > m_max_seq_len)
      throw;
  }
}

void TensorFlowMorphoSyntaxPrivate::load_config(const QString& config_file_name)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "TensorFlowMorphoSyntaxPrivate::load_config" << config_file_name)

  QFile file(config_file_name);

  if (!file.open(QIODevice::ReadOnly))
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config can't load config from \""
                        << config_file_name << "\".",
                        LimaException);

  QByteArray bytes = file.readAll();
  QJsonDocument data = QJsonDocument::fromJson(bytes);
  if (data.isNull())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config can't load config from \""
                        << config_file_name << "\". Invalid Json.",
                        LimaException);
  if (!data.isObject())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config can't load config from \""
                        << config_file_name << "\". Loaded data is not an object",
                        LimaException);
  auto params = data.object().value("conf");
  if (params.isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" has no value params.",
      LimaException);
  auto paramsObject = params.toObject();
  if (paramsObject.isEmpty())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" params is not an object.",
      LimaException);
  if (paramsObject.value("maxSeqLen").isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" missing param maxSeqLen.",
      LimaException);
  if (!paramsObject.value("maxSeqLen").isDouble())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" param maxSeqLen is not a number.",
      LimaException);
  m_max_seq_len = paramsObject.value("maxSeqLen").toInt();

  if (paramsObject.value("maxWordLen").isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" missing param maxWordLen.",
      LimaException);
  if (!paramsObject.value("maxWordLen").isDouble())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" param maxWordLen is not a number.",
      LimaException);
  m_max_word_len = params.toObject().value("maxWordLen").toInt();

  if (paramsObject.value("batchSize").isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" missing param batchSize.",
      LimaException);
  if (!paramsObject.value("batchSize").isDouble())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" param batchSize is not a number.",
      LimaException);
  m_batch_size = params.toObject().value("batchSize").toInt();

  auto dicts = data.object().value("dicts");
  if (dicts.isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" has no value dicts.",
      LimaException);
  auto dictsObject = dicts.toObject();
  if (dictsObject.isEmpty())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" dicts is not an object.",
      LimaException);

  auto c2i = dictsObject.value("c2i");
  if (c2i.isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" has no value dicts[\"c2i\"].",
      LimaException);
  auto c2iObject = c2i.toObject();
  if (c2iObject.isEmpty())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" c2i object is empty.",
      LimaException);
  load_string_to_uint_map(c2iObject, m_char2idx);

  auto w2i = dictsObject.value("w2i");
  if (w2i.isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" has no value dicts[\"w2i\"].",
      LimaException);
  auto w2iObject = w2i.toObject();
  if (w2iObject.isEmpty())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" w2i object is empty.",
      LimaException);
  load_string_to_uint_map(w2iObject, m_word2idx);

  auto output = data.object().value("output");
  if (output.isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" has no value output.",
      LimaException);
  auto outputObject = output.toObject();
  if (outputObject.isEmpty())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" output object is empty.",
      LimaException);
  load_output_description(outputObject);

  auto input = data.object().value("input");
  if (input.isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config config file \""
          << config_file_name << "\" has no value input.",
      LimaException);
  auto jso = input.toObject();
  for (auto i = jso.constBegin(); i != jso.constEnd(); ++i)
  {
    m_input_node_names[i.key().toStdString()] = i.value().toString().toStdString();
  }

  if (data.object().value("main_alphabet").isUndefined())
  {
    LOG_MESSAGE(LINFO, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" missing param main_alphabet.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" missing param main_alphabet.",
      LimaException);*/
  }
  else if (!data.object().value("main_alphabet").isString())
  {
    LOG_MESSAGE(LERROR, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" param main_alphabet is not a string.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" param main_alphabet is not a string.",
      LimaException);*/
  }
  else
  {
    m_main_alphabet = data.object().value("main_alphabet").toString();
  }

  if (data.object().value("feat_order").isUndefined())
  {
    LOG_MESSAGE(LINFO, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" missing param feat_order.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" missing param feat_order.",
      LimaException);*/
  }
  else if (!data.object().value("feat_order").isArray())
  {
    LOG_MESSAGE(LERROR, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" param feat_order is not an array.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" param feat_order is not an array.",
      LimaException);*/
  }
  else
  {
    load_string_array(data.object().value("feat_order").toArray(), m_feat_order);
  }

  m_upos_idx = m_seqtag_id2idx.find("upos")->second;
  m_feat_deps.resize(m_seqtag_outputs[m_upos_idx].i2t.size());

  if (0 == m_feat_order.size())
  {
    m_feat_order.push_back("upos");
    for (const auto& out : m_seqtag_outputs)
    {
      if (string("upos") != out.feat_name)
        m_feat_order.push_back(out.feat_name);
    }
  }

  if (data.object().value("feat_deps").isUndefined())
  {
    LOG_MESSAGE(LINFO, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" missing param feat_deps.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" missing param feat_deps.",
      LimaException);*/
  }
  else if (!data.object().value("feat_deps").isObject())
  {
    LOG_MESSAGE(LERROR, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" param feat_deps is not an object.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" param feat_deps is not an object.",
      LimaException);*/
  }
  else
  {
    auto featDepsObject = data.object().value("feat_deps").toObject();

    for (auto i = featDepsObject.constBegin(); i != featDepsObject.constEnd(); ++i)
    {
      string upos_name = i.key().toStdString();
      for (size_t upos_idx = 0; upos_idx < m_seqtag_outputs[m_upos_idx].i2t.size(); upos_idx++)
      {
        if (upos_name == m_seqtag_outputs[m_upos_idx].i2t[upos_idx])
        {
          vector<string> v;
          load_string_array(i.value().toArray(), v);
          for ( const auto& item : v )
          {
            m_feat_deps[upos_idx].insert(item);
          }
          break;
        }
      }
    }
  }
}

inline string& std_string_replace(string& s, const string& str_old, const string& str_new)
{
  string::size_type pos = 0;
  while ((pos = s.find(str_old, pos)) != string::npos)
  {
    s.replace(pos, str_old.size(), str_new);
    pos += str_new.size();
  }
  return s;
}

void TensorFlowMorphoSyntaxPrivate::load_output_description(const QJsonObject& jso)
{
  const auto &pcm = static_cast<const LanguageData&>(MediaticData::single().mediaData(m_language)).getPropertyCodeManager();

  for (QJsonObject::const_iterator i = jso.begin(); i != jso.end(); ++i)
  {
    const QJsonObject& obj = i.value().toObject();
    string type = obj.value("type").toString().toStdString();

    if (type == string("seqtag"))
    {
      if (i.key().endsWith("-bw") || i.key().endsWith("-fw"))
        continue;

      SeqTagOutput out;
      out.full_name = i.key().toStdString();
      out.feat_name = out.full_name;
      string::size_type pos = out.feat_name.rfind('/');
      if (string::npos != pos)
        out.feat_name = out.feat_name.substr(pos + 1);
      std_string_replace(std_string_replace(out.feat_name, "-_", "[") , "_-", "]");

      const QJsonObject& nodes = obj.value("nodes").toObject();
      out.logits_node_name = nodes.value("logits").toString().toStdString();
      out.crf_node_name = nodes.value("crf").toString().toStdString();

      load_string_array(obj.value("i2t").toArray(), out.i2t);
      if (!fill_linguistic_codes(pcm, out))
        continue;

      m_seqtag_outputs.push_back(out);
      m_seqtag_id2idx[out.feat_name] = m_seqtag_outputs.size() - 1;
    }
    else if (type == string("depparse"))
    {
      DepparseOutput out;
      out.full_name = i.key().toStdString();
      out.feat_name = out.full_name;
      string::size_type pos = out.feat_name.rfind('/');
      if (string::npos != pos)
        out.feat_name = out.feat_name.substr(pos + 1);

      const QJsonObject& arcs = obj.value("arcs").toObject().value("nodes").toObject();
      out.arcs_logits_node_name = arcs.value("logits").toString().toStdString();
      out.arcs_pred_node_name  = arcs.value("pred").toString().toStdString();

      const QJsonObject& tags = obj.value("tags").toObject().value("nodes").toObject();
      out.tags_logits_node_name = tags.value("logits").toString().toStdString();
      out.tags_pred_node_name  = tags.value("pred").toString().toStdString();

      load_string_array(obj.value("tags").toObject().value("i2t").toArray(), out.i2t);

      for (size_t j = 0; j < out.i2t.size(); ++j)
      {
        if (out.i2t[j] == "root")
          out.root_tag_idx = j;
        out.i2t[j] = out.i2t[j];
      }

      m_depparse_outputs.push_back(out);
      m_depparse_id2idx[out.feat_name] = m_depparse_outputs.size() - 1;
    }
    else
    {
      LOG_ERROR_AND_THROW("TensorFlowMorphoSyntaxPrivate::load_output_description unknown output type: \""
                          << type.c_str() << "\".",
                          LimaException);
    }
  }
}

void TensorFlowMorphoSyntaxPrivate::load_graph(const QString& model_path, GraphDef* graph_def)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "TensorFlowMorphoSyntaxPrivate::load_graph" << model_path);

  Status status = ReadBinaryProto(Env::Default(),
                                  model_path.toStdString(),
                                  graph_def);
  if (!status.ok())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntaxPrivate::load_graph error reading binary proto:"
                        << status.ToString().c_str(),
                        LimaException);
}

bool TensorFlowMorphoSyntaxPrivate::fill_linguistic_codes(const PropertyCodeManager& pcm,
                                                          SeqTagOutput& out)
{
  string feat_name = out.feat_name;
  if (feat_name == "upos")
    feat_name = "MICRO";

  try
  {
    out.accessor = &(pcm.getPropertyAccessor(feat_name));
  }
  catch (const InvalidConfiguration& e)
  {
    //
    return false;
  }
  if (nullptr == out.accessor)
  {
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntaxPrivate::fill_linguistic_codes can't find accessor for \""
                        << feat_name.c_str() << "\" feature",
                        LimaException);}

  const auto& pm = pcm.getPropertyManager(feat_name);

  out.i2c.resize(out.i2t.size());
  for (size_t i = 0; i < out.i2t.size(); ++i)
  {
    if (out.i2t[i] == "#None")
      out.i2c[i] = L_NONE;
    else
      out.i2c[i] = pm.getPropertyValue(out.i2t[i]);
  }
  return true;
}

} // MorphoSyntax
} // TensorFlowUnits
} // LinguisticProcessing
} // Lima
