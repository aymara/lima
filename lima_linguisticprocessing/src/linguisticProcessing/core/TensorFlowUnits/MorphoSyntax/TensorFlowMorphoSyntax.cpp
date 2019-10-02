/*
    Copyright 2002-2019 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/

#include <iostream>
#include <iterator>
#include <set>
#include <algorithm>

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

#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/cc/client/client_session.h"

#include "fastText/src/fasttext.h"

#include "Viterbi.h"
#include "QJsonHelpers.h"
#include "TensorFlowHelpers.h"

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
                                        TENSORFLOWMORPHOSYNTAXLOGINIT; \
                                        LERROR << msg; \
                                        throw exc; \
                                      }

#if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
  #define LOG_MESSAGE(stream, msg) { TENSORFLOWMORPHOSYNTAXLOGINIT; stream << msg; }
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) TENSORFLOWMORPHOSYNTAXLOGINIT; LOG_MESSAGE(stream, msg);
#else
  #define LOG_MESSAGE(stream, msg) ;
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif

class TensorFlowMorphoSyntaxPrivate
{
public:
  TensorFlowMorphoSyntaxPrivate()
    : m_stringsPool(nullptr) { }

  void init(GroupConfigurationStructure&,
            MediaId lang,
            const QString& model_prefix,
            const QString& embeddings);

  LimaStatusCode process(AnalysisContent& analysis) const;

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

    TSentence(LinguisticGraphVertex b, LinguisticGraphVertex e, size_t max_len)
      : begin(b), end(e), token_count(0)
    {
      tokens.resize(max_len);
    }

    void append(LinguisticGraphVertex v, Token *t, MorphoSyntacticData *m)
    {
      if (tokens.size() <= token_count)
        LOG_ERROR_AND_THROW("TSentence::append: there is no more space for new token",
                            LimaException());

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
  };

  vector<SeqTagOutput> m_seqtag_outputs;
  map<string, size_t> m_seqtag_id2idx;
  vector<DepparseOutput> m_depparse_outputs;
  map<string, size_t> m_depparse_id2idx;

  size_t m_max_seq_len;
  size_t m_max_word_len;
  size_t m_batch_size;

  map<u32string, unsigned int> m_char2idx;
  map<u32string, unsigned int> m_word2idx;
  u32string m_unk;
  u32string m_eos;

  map<string, string> m_input_node_names;

  QString m_model_path;

  unique_ptr<Session> m_session;
  GraphDef m_graph_def;

  fasttext::FastText m_fasttext;

  void load_config(const QString& config_file_name);
  void load_graph(const QString& model_path);
  void load_output_description(const QJsonObject& jso);
  void fill_linguistic_codes(const PropertyCodeManager& pcm, SeqTagOutput& out);
  void init_crf();

  void analyze(vector<TSentence>& sentences, SyntacticData& syntacticData) const;
  void generate_batch(const vector<TSentence>& sentences,
                      size_t start,
                      size_t size,
                      vector<pair<string, Tensor>>& batch) const;
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
  QString modelPrefix;
  try
  {
    modelPrefix = QString::fromUtf8(gcs.getParamsValueAtKey("model_prefix").c_str());
  }
  catch (NoSuchParam& )
  {
    LOG_ERROR_AND_THROW("no param 'model_prefix' in TensorFlowMorphoSyntax group configuration",
                        InvalidConfiguration());
  }

  QString embeddings;
  try
  {
    embeddings = QString::fromUtf8(gcs.getParamsValueAtKey("embeddings").c_str());
  }
  catch (NoSuchParam& )
  {
    LOG_ERROR_AND_THROW("no param 'embeddings' in TensorFlowMorphoSyntax group configuration",
                        InvalidConfiguration());
  }

  m_d->init(gcs, manager->getInitializationParameters().media, modelPrefix, embeddings);
}

void TensorFlowMorphoSyntaxPrivate::init(
  GroupConfigurationStructure& /*gcs*/,
  MediaId lang,
  const QString& model_prefix,
  const QString& embeddings)
{
  m_language = lang;
  m_stringsPool = &MediaticData::changeable().stringsPool(m_language);

  QString lang_str = MediaticData::single().media(m_language).c_str();
  QString resources_path = MediaticData::single().getResourcesPath().c_str();

  auto config_file_name = findFileInPaths(resources_path,
                                          QString::fromUtf8("/TensorFlowMorphoSyntax/%1/%2.conf")
                                            .arg(lang_str).arg(model_prefix));
  load_config(config_file_name);

  tensorflow::SessionOptions options;
  tensorflow::ConfigProto & config = options.config;
  config.set_inter_op_parallelism_threads(8);
  config.set_intra_op_parallelism_threads(8);
  config.set_use_per_session_threads(false);
  Session* session = 0;
  Status status = NewSession(options, &session);
  if (!status.ok())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::init: Can't create TensorFlow session: "
                        << status.ToString(),
                        LimaException());
  m_session = unique_ptr<Session>(session);
  m_model_path = findFileInPaths(resources_path,
                                 QString::fromUtf8("/TensorFlowMorphoSyntax/%1/%2.model")
                                  .arg(lang_str).arg(model_prefix));
  load_graph(m_model_path);

  // Add the graph to the session
  status = m_session->Create(m_graph_def);
  if (!status.ok())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::init: Can't add graph to TensorFlow session: "
                        << status.ToString(),
                        LimaException());

  init_crf();

  m_unk = QString("<<unk>>").toStdU32String();
  m_eos = QString("<<eos>>").toStdU32String();

  QString embeddings_path = findFileInPaths(resources_path,
                                            QString::fromUtf8("/TensorFlowMorphoSyntax/%1/%2")
                                             .arg(lang_str).arg(embeddings));
  m_fasttext.loadModel(embeddings_path.toStdString());
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
                          << status.ToString(),
                          LimaException());

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
                        LimaException());

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

LimaStatusCode TensorFlowMorphoSyntaxPrivate::process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();

  LOG_MESSAGE(LINFO, "Start of TensorFlowMorphoSyntax");

  AnalysisGraph* anagraph = static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  AnalysisGraph* posgraph = new AnalysisGraph("PosGraph", m_language, false, false, *anagraph);
  analysis.setData("PosGraph", posgraph);
  LinguisticGraph* src_graph = anagraph->getGraph();

  /** Creation of an annotation graph if necessary */
  AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));
  if (annotationData == 0)
  {
    annotationData = new AnnotationData();

    /** Creates a node in the annotation graph for each node of the
      * morphosyntactic graph. Each new node is annotated with the name mrphv and
      * associated to the morphosyntactic vertex number */
    anagraph->populateAnnotationGraph(annotationData, "AnalysisGraph");
    posgraph->populateAnnotationGraph(annotationData, "PosGraph");

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

  auto sb = static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb == nullptr)
  {
    LOG_MESSAGE(LERROR, "No SentenceBounds");
    return MISSING_DATA;
  }

  auto newSb = new SegmentationData("PosGraph");
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

    TSentence sent(beginSentence, endSentence, m_max_seq_len);

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

    sentences.push_back(sent);
  }

  syntacticData->setupDependencyGraph();

  analyze(sentences, *syntacticData);

  LOG_MESSAGE(LINFO, "End of TensorFlowMorphoSyntax");
  TimeUtils::logElapsedTime("TensorFlowMorphoSyntax");

  return SUCCESS_ID;
}

void TensorFlowMorphoSyntaxPrivate::analyze(vector<TSentence>& sentences,
                                            SyntacticData& syntacticData) const
{
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
                          << status.ToString(),
                          LimaException());

    // Apply prediction to Lima graphs
    size_t out_idx = 0;
    for (; out_idx < m_seqtag_outputs.size(); ++out_idx)
    {
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
          if (m_seqtag_outputs[out_idx].i2c[pred] != LinguisticCode(0))
          {
            TToken& t = sent.tokens[a];
            if (nullptr != t.morpho)
            {
              if (t.morpho->size() == 0)
                add_linguistic_element(t.morpho, *(t.token));

              m_seqtag_outputs[out_idx].accessor->writeValue(m_seqtag_outputs[out_idx].i2c[pred],
                                                             t.morpho->begin()->properties);
            }
          }
        }
      }
    }

    for (; (out_idx - m_seqtag_outputs.size()) < m_depparse_outputs.size(); ++out_idx)
    {
      auto arcs_tensor = out[out_idx].tensor<int, 2>();
      auto tags_tensor = out[out_idx + 1].tensor<int, 2>();

      auto arcs_logits = out[out_idx + 2].tensor<float, 3>();
      auto tags_logits = out[out_idx + 3].tensor<float, 3>();

      const DepparseOutput& out_descr = *(m_depparse_outputs.begin());

      for (int64 p = 0; p < arcs_tensor.dimension(0); p++)
      {
        TSentence& sent = sentences[i+p];
        size_t len = sent.token_count;
        if (0 == len)
          continue;

        for (size_t j = 1; j < len + 1; j++)
        {
          size_t pred_head = arcs_tensor(p, j);
          size_t pred_tag = tags_tensor(p, j);
          TToken& t = sent.tokens[j-1];

          if (pred_head > 0)
            pred_head -= 1;

          if (pred_head != 0)
          {
            TToken& h = sent.tokens[pred_head];
            syntacticData.addRelationNoChain(ld.getSyntacticRelationId(out_descr.i2t[pred_tag]),
                                             t.vertex,
                                             h.vertex);
          }
          else
          {
            syntacticData.addRelationNoChain(ld.getSyntacticRelationId("ud:root"),
                                             t.vertex,
                                             t.vertex);
          }
        }
      }
    }

    i += this_batch_size;
  }
}

void TensorFlowMorphoSyntaxPrivate::generate_batch(const vector<TSentence>& sentences,
                                                   size_t start,
                                                   size_t batch_size,
                                                   vector<pair<string, Tensor>>& batch) const
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
                                                   static_cast<long long>(300)
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
      fasttext::Vector vec(300);
      m_fasttext.getWordVector(vec, form.toUtf8().toStdString());
      for (int64 a = 0; a < vec.size(); ++a)
        input_pretrained(i, n, a) = vec[a];
    }

    len(i) = n + 1;
  }
}

void TensorFlowMorphoSyntaxPrivate::load_config(const QString& config_file_name)
{
  QFile file(config_file_name);

  if (!file.open(QIODevice::ReadOnly))
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntax::load_config can't load config from \""
                        << config_file_name << "\".",
                        LimaException());

  QByteArray bytes = file.readAll();
  QJsonDocument data = QJsonDocument::fromJson(bytes);

  m_max_seq_len = data.object().value("params").toObject().value("maxSeqLen").toInt();
  m_max_word_len = data.object().value("params").toObject().value("maxWordLen").toInt();
  m_batch_size = data.object().value("params").toObject().value("batchSize").toInt();

  load_string_to_uint_map(data.object().value("dicts").toObject().value("c2i").toObject(), m_char2idx);
  load_string_to_uint_map(data.object().value("dicts").toObject().value("w2i").toObject(), m_word2idx);

  load_output_description(data.object().value("output").toObject());

  const QJsonObject& jso = data.object().value("input").toObject();
  for (QJsonObject::const_iterator i = jso.begin(); i != jso.end(); ++i)
  {
    m_input_node_names[i.key().toStdString()] = i.value().toString().toStdString();
  }
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
      SeqTagOutput out;
      out.full_name = i.key().toStdString();
      out.feat_name = out.full_name;
      string::size_type pos = out.feat_name.rfind('/');
      if (string::npos != pos)
        out.feat_name = out.feat_name.substr(pos + 1);

      const QJsonObject& nodes = obj.value("nodes").toObject();
      out.logits_node_name = nodes.value("logits").toString().toStdString();
      out.crf_node_name = nodes.value("crf").toString().toStdString();

      load_string_array(obj.value("i2t").toArray(), out.i2t);
      fill_linguistic_codes(pcm, out);

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
          out.i2t[j] = string("ud:") + out.i2t[j];

      m_depparse_outputs.push_back(out);
      m_depparse_id2idx[out.feat_name] = m_depparse_outputs.size() - 1;
    }
    else
    {
      LOG_ERROR_AND_THROW("TensorFlowMorphoSyntaxPrivate::load_output_description unknown output type: \""
                          << type << "\".",
                          LimaException());
    }
  }
}

void TensorFlowMorphoSyntaxPrivate::load_graph(const QString& model_path)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "TensorFlowMorphoSyntaxPrivate::load_graph" << model_path);

  Status status = ReadBinaryProto(Env::Default(),
                                  model_path.toStdString(),
                                  &m_graph_def);
  if (!status.ok())
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntaxPrivate::load_graph error reading binary proto:"
                        << status.ToString(),
                        LimaException());
}

void TensorFlowMorphoSyntaxPrivate::fill_linguistic_codes(const PropertyCodeManager& pcm,
                                                          SeqTagOutput& out)
{
  string feat_name = out.feat_name;
  if (feat_name == "upos")
    feat_name = "MICRO";

  out.accessor = &(pcm.getPropertyAccessor(feat_name));
  if (nullptr == out.accessor)
    LOG_ERROR_AND_THROW("TensorFlowMorphoSyntaxPrivate::fill_linguistic_codes can't find accessor for \""
                        << feat_name << "\" feature",
                        LimaException());

  const auto& pm = pcm.getPropertyManager(feat_name);

  out.i2c.resize(out.i2t.size());
  for (size_t i = 0; i < out.i2t.size(); ++i)
  {
    if (out.i2t[i] == "#None")
      out.i2c[i] = 0;
    else
      out.i2c[i] = pm.getPropertyValue(out.i2t[i]);
  }
}

} // MorphoSyntax
} // TensorFlowUnits
} // LinguisticProcessing
} // Lima
