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

#include "CppUppsalaTensorFlowTokenizer.h"
#include "DeepTokenizerBase.h"
#include "tokUtils.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"

#include <QtCore/QTemporaryFile>
#include <QtCore/QRegularExpression>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <string>

#define DEBUG_THIS_FILE true

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::Common;
using namespace tensorflow;

#define EIGEN_DONT_VECTORIZE

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowTokenizer
{

static u32string SPACE = QString::fromUtf8(" ").toStdU32String();

static SimpleFactory<MediaProcessUnit,CppUppsalaTensorFlowTokenizer> cppupsalatokenizerFactory(CPPUPPSALATENSORFLOWTOKENIZER_CLASSID); // clazy:exclude=non-pod-global-static

#define LOG_ERROR_AND_THROW(msg, exc) { \
                                        TOKENIZERLOGINIT; \
                                        LERROR << msg; \
                                        throw exc; \
                                      }

#if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
  #define LOG_MESSAGE(stream, msg) stream << msg;
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) TOKENIZERLOGINIT; LOG_MESSAGE(stream, msg);
#else
  #define LOG_MESSAGE(stream, msg) ;
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif

class CppUppsalaTokenizerPrivate : public DeepTokenizerBase
{
public:
  CppUppsalaTokenizerPrivate();
  virtual ~CppUppsalaTokenizerPrivate();

  void init(const QString& model_refix);
  vector< vector< pair<QString, int> > > tokenize(const QString& text);

  MediaId m_language;
  FsaStringsPool* m_stringsPool;
  LinguisticGraphVertex m_currentVx;
  QString m_data;

protected:
  TokStatusCode encode_text(const u32string& text,
                            vector<vector<unsigned int>>& ngram_idxs);

  TokStatusCode generate_batch(const vector<vector<unsigned int>>& ngram_idxs,
                               size_t start,
                               size_t size,
                               vector<pair<string, Tensor>>& batch);

  void load_graph(const QString& frozen_graph_filename);
  void load_config(const QString& model_prefix);
  void load_ngram_defs(const QJsonArray& jsa);
  void load_dicts(const QJsonArray& jsa);

  QString m_model_path;

  Session* m_session;
  GraphDef m_graph_def;

  int64 m_max_seq_len;

  struct TNGramDef
  {
    int m_start;
    size_t m_len;

    TNGramDef() : m_start(0), m_len(0)
    { }

    TNGramDef(int start, size_t len) : m_start(start), m_len(len)
    { }
  };

  struct TDict
  {
    // N-gram dictionaries are in UTF-32 (wchar_t)
    vector<u32string> m_i2w;
    map<u32string, unsigned int> m_w2i;
  };

  // Dictionaries of tags are ordinary 1-byte strings
  vector<string> m_i2t;

  // Codes
  size_t m_token_begin;        // B
  size_t m_token_inside;       // I
  size_t m_token_end;          // E
  size_t m_token_outside;      // X
  size_t m_token_single;       // S

  // + EOS
  size_t m_token_single_last;  // T
  size_t m_token_end_last;     // U

  vector<TNGramDef> m_ngram_defs;
  vector<TDict> m_ngrams;

  vector<vector<float>> m_crf;

  static void load_string_array(const QJsonArray& jsa, vector<string>& v);
  static void load_string_array(const QJsonArray& jsa, vector<u32string>& v);
  static void load_string_to_uint_map(const QJsonObject& jso, map<u32string, unsigned int>& v);

  static float viterbi_decode(const vector<vector<float>>& scores,
                              const vector<vector<float>>& transitions,
                              vector<size_t>& result);
};

CppUppsalaTokenizerPrivate::CppUppsalaTokenizerPrivate() :
  m_stringsPool(nullptr),
  m_currentVx(0)
{
}

CppUppsalaTokenizerPrivate::~CppUppsalaTokenizerPrivate()
{
}

CppUppsalaTensorFlowTokenizer::CppUppsalaTensorFlowTokenizer() :
  m_d(new CppUppsalaTokenizerPrivate())
{
}

CppUppsalaTensorFlowTokenizer::~CppUppsalaTensorFlowTokenizer()
{
  delete m_d;
}

void CppUppsalaTensorFlowTokenizer::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "CppUppsalaTensorFlowTokenizer::init");

  m_d->m_language = manager->getInitializationParameters().media;
  m_d->m_stringsPool = &Common::MediaticData::MediaticData::changeable().stringsPool(m_d->m_language);

  try
  {
    m_d->m_data = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("data").c_str());
  }
  catch (NoSuchParam& )
  {
    m_d->m_data = QString::fromUtf8("SentenceBoundaries");
  }

  QString modelPrefix; // The path to the LIMA python tensorflow-based tokenizer
  try
  {
    modelPrefix = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("model_prefix").c_str());
  }
  catch (NoSuchParam& )
  {
    LOG_ERROR_AND_THROW("no param 'model_prefix' in CppUppsalaTensorFlowTokenizer group configuration",
                        InvalidConfiguration());
  }

  m_d->init(modelPrefix);
}

LimaStatusCode CppUppsalaTensorFlowTokenizer::process(AnalysisContent& analysis) const
{
  LOG_MESSAGE_WITH_PROLOG(LINFO, "start tokenizer process");
  TimeUtilsController TensorFlowTokenizerProcessTime("TensorFlowTokenizer");

  auto anagraph = new AnalysisGraph("AnalysisGraph",m_d->m_language,true,true);
  analysis.setData("AnalysisGraph",anagraph);
  LinguisticGraph* graph=anagraph->getGraph();
  m_d->m_currentVx = anagraph->firstVertex();
  // Get text from analysis
  LimaStringText* originalText=static_cast<LimaStringText*>(analysis.getData("Text"));

  // Evaluate TensorFlow model on the text
  auto sentencesTokens = m_d->tokenize(*originalText);

  // Insert the tokens in the graph and create sentence limits
  SegmentationData* sb = new SegmentationData("AnalysisGraph");
  analysis.setData(m_d->m_data.toUtf8().constData(), sb);

  remove_edge(anagraph->firstVertex(),
              anagraph->lastVertex(),
              *graph);
  LinguisticGraphVertex beginSentence = 0;

  // Insert the tokens in the graph and create sentence limits
  for (const auto& sentence: sentencesTokens)
  {
    LinguisticGraphVertex endSentence = numeric_limits< LinguisticGraphVertex >::max();
    for (const auto& token: sentence)
    {
      const auto& str = token.first;

      LOG_MESSAGE(LDEBUG, "      Adding token '" << str << "'");

      StringsPoolIndex form=(*m_d->m_stringsPool)[str];
      Token *tToken = new Token(form,str, token.second, token.first.size());
      if (tToken == 0)
        LOG_ERROR_AND_THROW("CppUppsalaTensorFlowTokenizer::process: Can't allocate memory with \"new Token(...)\"",
                            MemoryErrorException());

      m_d->computeDefaultStatus(*tToken);

      LOG_MESSAGE(LDEBUG, "      status is " << tToken->status().toString());

      // Adds on the path
      LinguisticGraphVertex newVx = add_vertex(*graph);
      endSentence = newVx;
      put(vertex_token, *graph, newVx, tToken);
      put(vertex_data, *graph, newVx, new MorphoSyntacticData());
      add_edge(m_d->m_currentVx, newVx, *graph);
      m_d->m_currentVx = newVx;
    }

    LOG_MESSAGE(LDEBUG, "adding sentence" << beginSentence << endSentence);

    sb->add(Segment("sentence", beginSentence, endSentence, anagraph));
    beginSentence = endSentence;
  }

  add_edge(m_d->m_currentVx,anagraph->lastVertex(),*graph);

  return SUCCESS_ID;
}

void CppUppsalaTokenizerPrivate::init(const QString& model_prefix)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "CppUppsalaTokenizerPrivate::init" << model_prefix);

  QString lang_str = Common::MediaticData::MediaticData::single().media(m_language).c_str();
  QString resources_path = Common::MediaticData::MediaticData::single().getResourcesPath().c_str();

  auto config_file_name = findFileInPaths(resources_path,
                                          QString::fromUtf8("/TensorFlowTokenizer/%1/%2.conf")
                                            .arg(lang_str).arg(model_prefix));
  load_config(config_file_name);

  tensorflow::SessionOptions options;
  tensorflow::ConfigProto & config = options.config;
  config.set_inter_op_parallelism_threads(8);
  config.set_intra_op_parallelism_threads(8);
  config.set_use_per_session_threads(false);
  Status status = NewSession(options, &m_session);
  if (!status.ok())
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::init: Can't create TensorFlow session: "
                        << status.ToString(),
                        LimaException());

  m_model_path = findFileInPaths(resources_path,
                                 QString::fromUtf8("/TensorFlowTokenizer/%1/%2.model")
                                  .arg(lang_str).arg(model_prefix));
  load_graph(m_model_path);

  // Add the graph to the session
  status = m_session->Create(m_graph_def);
  if (!status.ok())
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::init: Can't add graph to TensorFlow session: "
                        << status.ToString(),
                        LimaException());

  vector<Tensor> out;

  status = m_session->Run({}, {"CRF/crf"}, {}, &out);
  if (!status.ok())
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::init: Can't execute \"Run\" in TensorFlow session: "
                        << status.ToString(),
                        LimaException());

  auto crf = out[0].matrix<float>();

  m_crf.resize(out[0].dim_size(0));
  for (int64 j = 0; j < out[0].dim_size(0); j++)
  {
    m_crf[j].resize(out[0].dim_size(1));
    for (int64 k = 0; k < out[0].dim_size(1); k++)
      m_crf[j][k] = crf(j, k);
  }
}

void CppUppsalaTokenizerPrivate::load_string_array(const QJsonArray& jsa, vector<string>& v)
{
  v.clear();
  v.reserve(jsa.size());
  for (QJsonArray::const_iterator i = jsa.begin(); i != jsa.end(); ++i)
  {
    const QJsonValue value = *i;
    QString s = value.toString();
    v.push_back(s.toStdString());
  }
}

void CppUppsalaTokenizerPrivate::load_string_array(const QJsonArray& jsa, vector<u32string>& v)
{
  v.clear();
  v.reserve(jsa.size());
  for (QJsonArray::const_iterator i = jsa.begin(); i != jsa.end(); ++i)
  {
    QJsonValue value = *i;
    QString s = value.toString();
    v.push_back(s.toStdU32String());
  }
}

void CppUppsalaTokenizerPrivate::load_ngram_defs(const QJsonArray& jsa)
{
  m_ngram_defs.clear();
  m_ngram_defs.reserve(jsa.size());
  for (QJsonArray::const_iterator i = jsa.begin(); i != jsa.end(); ++i)
  {
    QJsonObject obj = (*i).toObject();
    TNGramDef def(obj["start"].toInt(), obj["len"].toInt());
    m_ngram_defs.push_back(def);
  }
}

void CppUppsalaTokenizerPrivate::load_string_to_uint_map(const QJsonObject& jso, map<u32string, unsigned int>& v)
{
  v.clear();
  for (QJsonObject::const_iterator i = jso.begin(); i != jso.end(); ++i)
  {
    if (v.end() != v.find(i.key().toStdU32String()))
      LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::load_string_to_uint_map: \"" << i.key() << "\" already known.",
                          LimaException());
    v[i.key().toStdU32String()] = i.value().toInt();
  }
}

void CppUppsalaTokenizerPrivate::load_dicts(const QJsonArray& jsa)
{
  m_ngrams.clear();
  m_ngrams.reserve(jsa.size());
  for (QJsonArray::const_iterator i = jsa.begin(); i != jsa.end(); ++i)
  {
    QJsonObject obj = (*i).toObject();
    TDict d;
    load_string_array(obj["i2w"].toArray(), d.m_i2w);
    load_string_to_uint_map(obj["w2i"].toObject(), d.m_w2i);
    m_ngrams.push_back(d);
  }
}

void CppUppsalaTokenizerPrivate::load_config(const QString& config_file_name)
{
//   QString config_file_name = model_prefix + ".conf";
  QFile file(config_file_name);

  if (!file.open(QIODevice::ReadOnly))
      LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::load_config can't load config from \""
                          << config_file_name << "\".",
                          LimaException());

  QByteArray bytes = file.readAll();
  QJsonDocument data = QJsonDocument::fromJson(bytes);

  m_max_seq_len = data["conf"]["max_seq_len"].toInt();
  load_string_array(data["conf"]["i2t"].toArray(), m_i2t);

  for (size_t i = 0; i < m_i2t.size(); ++i)
  {
    string tag = m_i2t[i];
    if ("B" == tag)
      m_token_begin = i;
    else if ("I" == tag)
      m_token_inside = i;
    else if ("E" == tag)
      m_token_end = i;
    else if ("X" == tag)
      m_token_outside = i;
    else if ("S" == tag)
      m_token_single = i;
    else if ("T" == tag)
      m_token_single_last = i;
    else if ("U" == tag)
      m_token_end_last = i;
    else
      LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::load_config: unknown tag\"" << tag << "\".",
                          LimaException());
  }

  load_ngram_defs(data["conf"]["ngrams"].toArray());
  load_dicts(data["dicts"]["ngrams"].toArray());
}

void CppUppsalaTokenizerPrivate::load_graph(const QString& model_path)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "CppUppsalaTokenizerPrivate::load_graph" << model_path);

  Status status = ReadBinaryProto(Env::Default(),
                                  model_path.toStdString(),
                                  &m_graph_def);
  if (!status.ok())
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::load_graph error reading binary proto:"
                        << status.ToString(),
                        LimaException());
}

TokStatusCode CppUppsalaTokenizerPrivate::encode_text(const u32string& text, vector<vector<unsigned int>>& ngram_idxs)
{
  ngram_idxs.clear();
  ngram_idxs.resize(m_ngram_defs.size());

  for (size_t n = 0; n < m_ngram_defs.size(); n++)
  {
    const TNGramDef& ngram_def = m_ngram_defs[n];
    const TDict& dict = m_ngrams[n];
    unsigned int idx_UNK = dict.m_w2i.find(QString::fromUtf8("<UNK>").toStdU32String())->second;
    ngram_idxs[n].reserve(text.size() - 2);

    for (size_t i = 1; i < text.size() - 1; i++)
    {
      size_t start = i + ngram_def.m_start;
      const u32string ngram = text.substr(start, ngram_def.m_len);
      if (ngram.size() != ngram_def.m_len)
        return TokStatusCode::INTERNAL_ERROR;

      auto it = dict.m_w2i.find(ngram);
      if (dict.m_w2i.end() != it)
        ngram_idxs[n].push_back(it->second);
      else
        ngram_idxs[n].push_back(idx_UNK);
    }
  }

  return TokStatusCode::SUCCESS;
}

TokStatusCode CppUppsalaTokenizerPrivate::generate_batch(const vector<vector<unsigned int>>& ngram_idxs,
                                                         size_t start,
                                                         size_t size,
                                                         vector<pair<string, Tensor>>& batch)
{
  batch.clear();

  int64 reserve = size;
  if (0 == reserve)
    reserve = 1 + (ngram_idxs[0].size() / 400);

  Tensor seq_len_tensor(DT_INT32, TensorShape({reserve}));
  auto seq_len = seq_len_tensor.tensor<int, 1>();

  batch.push_back( { "seq_len", seq_len_tensor } );

  vector<size_t> indices_tensor_pos;
  indices_tensor_pos.reserve(m_ngram_defs.size());

  for (size_t i = 0; i < m_ngram_defs.size(); ++i)
  {
    Tensor indices_tensor(DT_INT32, TensorShape({reserve, m_max_seq_len}));
    QString buff = QString::fromUtf8("ngram_idx_%1-%2")
      .arg(m_ngram_defs[i].m_start).arg(m_ngram_defs[i].m_len);
    string tensor_name = buff.toUtf8().constData();

    batch.push_back( { tensor_name, indices_tensor } );
    indices_tensor_pos.push_back(batch.size() - 1);
  }

  size_t seq_num = 0;
  for (size_t p = start; p < ngram_idxs[0].size(); p += m_max_seq_len)
  {
    size_t this_seq_len = (p + m_max_seq_len < ngram_idxs[0].size()) ? m_max_seq_len : (ngram_idxs[0].size() - p);
    seq_len(seq_num) = this_seq_len;

    for (size_t i = 0; i < m_ngram_defs.size(); ++i)
    {
      auto indices = batch[indices_tensor_pos[i]].second.tensor<int, 2>();

      size_t j = 0;
      for (; j < this_seq_len; j++)
        indices(seq_num, j) = ngram_idxs[i][p + j];

      for (; j < m_max_seq_len; j++)
        indices(seq_num, j) = 0;
    }

    seq_num += 1;
    if (size > 0 && seq_num == size)
      break;
  }

  return TokStatusCode::SUCCESS;
}

float CppUppsalaTokenizerPrivate::viterbi_decode(const vector<vector<float>>& scores,
                                                 const vector<vector<float>>& transitions,
                                                 vector<size_t>& result)
{
  size_t max_steps = scores.size();
  size_t max_states = transitions.size();

  vector<vector<float>> trellis;
  vector<vector<size_t>> backpointers;
  vector<vector<float>> v;

  trellis.resize(max_steps);
  for (auto& x : trellis)
    x.resize(max_states, 0.0);

  backpointers.resize(max_steps);
  for (auto& x : backpointers)
    x.resize(max_states, 0);

  v.resize(max_states);
  for (auto& x : v)
    x.resize(max_states, 0.0);

  trellis[0] = scores[0];

  for (size_t k = 1; k < scores.size(); ++k)
  {
    for (size_t i = 0; i < max_states; ++i)
      for (size_t j = 0; j < max_states; ++j)
        v[i][j] = trellis[k-1][i] + transitions[i][j];
        // transitions from [i] to [j]

    trellis[k] = scores[k];
    for (size_t i = 0; i < max_states; ++i)
    {
      size_t max_prev_state_no = 0;
      for (size_t j = 1; j < max_states; ++j)
        if (v[j][i] > v[max_prev_state_no][i])
          max_prev_state_no = j;
      trellis[k][i] += v[max_prev_state_no][i];
      backpointers[k][i] = max_prev_state_no;
    }
  }

  result.clear();
  result.resize(scores.size());
  size_t max_pos = 0;
  for (size_t i = 1; i < max_states; ++i)
    if (trellis[max_steps - 1][i] > trellis[max_steps - 1][max_pos])
      max_pos = i;

  result[0] = max_pos;
  vector<vector<size_t>> bp = backpointers;
  reverse(bp.begin(), bp.end());

  for (size_t i = 0; i < max_steps - 1; ++i)
    result[i+1] = bp[i][result[i]];

  reverse(result.begin(), result.end());

  max_pos = 0;
  for (size_t i = 1; i < max_states; ++i)
    if (trellis[trellis.size() - 1][i] > trellis[trellis.size() - 1][max_pos])
      max_pos = i;

  float viterbi_score = trellis[trellis.size() - 1][max_pos];

  return viterbi_score;
}

vector< vector< pair<QString, int> > > CppUppsalaTokenizerPrivate::tokenize(const QString& text)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "CppUppsalaTokenizerPrivate::tokenize" << text.left(100));

  // Encode text
  QString simplified_copy = text;
  simplified_copy.replace(QString::fromUtf8("\u00A0"), " ");
  simplified_copy.replace(QString::fromUtf8("\u000A"), " ");
  simplified_copy.replace(QString::fromUtf8("\u000D"), " ");
  u32string wtext = SPACE + simplified_copy.toStdU32String() + SPACE;
  vector<vector<unsigned int>> ngram_idxs;
  if (encode_text(wtext, ngram_idxs) != TokStatusCode::SUCCESS)
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::tokenize: error while encoding text to ngrams' indices.",
                        LimaException());

  // Generate batch
  vector<pair<string, Tensor>> inputs;
  if (generate_batch(ngram_idxs, 0, 0, inputs) != TokStatusCode::SUCCESS)
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::tokenize: error while generating batch.",
                        LimaException());

  // Run model
  vector<Tensor> out;
  Status status = m_session->Run(inputs, {"Dense/dropout/Identity"}, {}, &out);
  if (!status.ok())
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::tokenize: Can't execute \"Run\" in TensorFlow session: "
                        << status.ToString(),
                        LimaException());

  vector< vector< pair<QString, int> > > sentences;
  vector< pair<QString, int> > current_sentence;
  u32string current_token;
  int current_token_offset = 0;

  auto scores = out[0].tensor<float, 3>();
  for (int64 i = 0; i < out[0].dim_size(0); i++)
  {
    size_t len = 400;
    vector<vector<float>> converted_scores;
    converted_scores.resize(len);
    for (size_t j = 0; j < len; j++)
    {
      converted_scores[j].resize(out[0].dim_size(2));
      for (int64 k = 0; k < out[0].dim_size(2); k++)
        converted_scores[j][k] = scores(i, j, k);
    }

    vector<size_t> viterbi;
    viterbi_decode(converted_scores, m_crf, viterbi);

    for (size_t j = 0; j < viterbi.size(); ++j)
    {
      if (m_token_end == viterbi[j] || m_token_end_last == viterbi[j] || m_token_single == viterbi[j] || m_token_single_last == viterbi[j])
      {
        unsigned int pos = i * 400 + j;
        if (current_token.size() > 0 || L' ' != wtext[pos + 1])
        {
          current_token += wtext[pos + 1];


          if (m_token_single == viterbi[j] || m_token_single_last == viterbi[j])
            current_token_offset = i * 400 + j;
        }

        if (current_token.size() > 0 && current_token != SPACE)
        {
          current_sentence.push_back(make_pair(QString::fromStdU32String(current_token), current_token_offset));
          current_token.clear();
        }
      }
      else if (m_token_begin == viterbi[j] || m_token_inside == viterbi[j])
      {
        unsigned int pos = i * 400 + j;
        if (current_token.size() > 0 || L' ' != wtext[pos + 1])
        {
          current_token += wtext[pos + 1];
          if (m_token_begin == viterbi[j])
            current_token_offset = i * 400 + j;
        }
      }

      if (m_token_end_last == viterbi[j] || m_token_single_last == viterbi[j])
      {
        sentences.push_back(current_sentence);
        current_sentence.clear();
      }
    }
  }

  LOG_MESSAGE(LDEBUG, "CppUppsalaTensorFlowTokenizer::tokenize final sentences:" << sentences);

  return sentences;
}

} //namespace TensorFlowTokenizer
} // namespace LinguisticProcessing
} // namespace Lima
