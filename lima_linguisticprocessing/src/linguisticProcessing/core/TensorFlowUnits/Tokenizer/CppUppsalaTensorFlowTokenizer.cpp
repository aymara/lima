/*
    Copyright 2002-2020 CEA LIST

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

#include <QtCore/QTemporaryFile>
#include <QtCore/QRegularExpression>
#include <QDir>
#include <QJsonDocument>

#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/common/helpers/ConfigurationHelper.h"

#include "Viterbi.h"
#include "QJsonHelpers.h"

#include "CppUppsalaTensorFlowTokenizer.h"
#include "DeepTokenizerBase.h"
#include "tokUtils.h"


#define DEBUG_THIS_FILE true

using namespace std;
using namespace tensorflow;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::TensorFlowUnits::Common;

#define EIGEN_DONT_VECTORIZE

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace Tokenizer
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

CONFIGURATIONHELPER_LOGGING_INIT(TOKENIZERLOGINIT);

class CppUppsalaTokenizerPrivate : public DeepTokenizerBase, public ConfigurationHelper
{
public:
  CppUppsalaTokenizerPrivate();
  virtual ~CppUppsalaTokenizerPrivate();

  struct TPrimitiveToken
  {
    TPrimitiveToken() { }
    TPrimitiveToken(const QString& w,
                    int pos,
                    const QString& orig=QString())
        : wordText(w), originalText(orig), start(pos)
    { }

    QString wordText;
    QString originalText;
    int start;
  };

  void init(GroupConfigurationStructure& unitConfiguration);
  void tokenize(const QString& text, vector<vector<TPrimitiveToken>>& sentences);

  MediaId m_language;
  FsaStringsPool* m_stringsPool;
  LinguisticGraphVertex m_currentVx;
  QString m_data;

protected:
  TokStatusCode encode_text(const u32string& text,
                            vector<vector<unsigned int>>& ngram_idxs);

  TokStatusCode generate_batch(const vector<vector<unsigned int>>& ngram_idxs,
                               size_t& start,
                               size_t size,
                               size_t overlap,
                               vector<pair<string, Tensor>>& batch);

  void append_new_word(vector< TPrimitiveToken >& current_sentence,
                       const u32string& current_token,
                       int current_token_offset) const;

  void load_graph(const QString& frozen_graph_filename);
  void load_config(const QString& model_prefix);
  void load_ngram_defs(const QJsonArray& jsa);
  void load_dicts(const QJsonArray& jsa);

  QString m_model_path;

  std::unique_ptr<Session> m_session;
  GraphDef m_graph_def;

  size_t m_max_seq_len;

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

  // Parameters
  bool m_ignoreEOL;

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

  map<QString, vector<QString>> m_trrules;

  vector<vector<float>> m_crf;
};

CppUppsalaTokenizerPrivate::CppUppsalaTokenizerPrivate() :
  ConfigurationHelper("CppUppsalaTokenizerPrivate", THIS_FILE_LOGGING_CATEGORY()),
  m_stringsPool(nullptr),
  m_currentVx(0),
  m_ignoreEOL(false)
{
}

CppUppsalaTokenizerPrivate::~CppUppsalaTokenizerPrivate()
{
  auto status = m_session->Close();
  if (!status.ok())
  {
    LOG_MESSAGE_WITH_PROLOG(LERROR, "CppUppsalaTokenizerPrivate::~CppUppsalaTokenizerPrivate(): Error closing session:"
                            << status.ToString());
  }
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
  GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "CppUppsalaTensorFlowTokenizer::init");

  m_d->m_language = manager->getInitializationParameters().media;
  m_d->m_stringsPool = &MediaticData::changeable().stringsPool(m_d->m_language);

  m_d->init(unitConfiguration);
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
  vector< vector< CppUppsalaTokenizerPrivate::TPrimitiveToken > > sentencesTokens;
  m_d->tokenize(*originalText, sentencesTokens);

  // Insert the tokens in the graph and create sentence limits
  SegmentationData* sb = new SegmentationData("AnalysisGraph");
  analysis.setData(m_d->m_data.toStdString(), sb);

  remove_edge(anagraph->firstVertex(),
              anagraph->lastVertex(),
              *graph);
  LinguisticGraphVertex beginSentence = 0;

  // Insert the tokens in the graph and create sentence limits
  for (const auto& sentence: sentencesTokens)
  {
    if (sentence.size() < 1)
      continue;

    LinguisticGraphVertex endSentence = numeric_limits< LinguisticGraphVertex >::max();
    for (const auto& token: sentence)
    {
      const auto& str = token.wordText;

      LOG_MESSAGE(LDEBUG, "      Adding token '" << str << "'");

      StringsPoolIndex form=(*m_d->m_stringsPool)[str];
      Token *tToken = new Token(form, str, token.start, token.wordText.size());
      if (tToken == nullptr)
        LOG_ERROR_AND_THROW("CppUppsalaTensorFlowTokenizer::process: Can't allocate memory with \"new Token(...)\"",
                            MemoryErrorException());

      if (token.originalText.size() > 0)
      {
        // tranduced token
        // save original word as orph alternative
        StringsPoolIndex orig = (*m_d->m_stringsPool)[token.originalText];
        tToken->addOrthographicAlternatives(orig);
      }

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

void CppUppsalaTokenizerPrivate::init(GroupConfigurationStructure& unitConfiguration)
{
  m_data = QString(getStringParameter(unitConfiguration, "data", 0, "SentenceBoundaries").c_str());
  QString model_prefix = getStringParameter(unitConfiguration, "model_prefix", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY).c_str();

  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "CppUppsalaTokenizerPrivate::init" << model_prefix);

  QString lang_str = MediaticData::single().media(m_language).c_str();
  QString resources_path = MediaticData::single().getResourcesPath().c_str();
  QString model_name = model_prefix;
  string udlang;
  MediaticData::single().getOptionValue("udlang", udlang);
  if (udlang.size() >= 4 && udlang.find(lang_str.toStdString()) == 0 && udlang[lang_str.size()] == '-')
  {
    udlang = udlang.substr(3);
  }
  else
  {
    // parse lang codes like 'eng.ud'
    if (udlang.size() == 0 && lang_str.size() >= 4 && lang_str.indexOf(".ud") == lang_str.size() - 3)
    {
      udlang = lang_str.left(3).toStdString();
      lang_str = "ud";
    }
    else
    {
      LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::init: Can't parse language id "
                          << udlang, Lima::InvalidConfiguration());
    }
  }

  model_name.replace(QString("$udlang"), QString(udlang.c_str()));

  auto config_file_name = findFileInPaths(resources_path,
                                          QString::fromUtf8("/TensorFlowTokenizer/%1/%2.conf")
                                            .arg(lang_str).arg(model_name));
  load_config(config_file_name);

  tensorflow::SessionOptions options;
  tensorflow::ConfigProto & config = options.config;
  config.set_inter_op_parallelism_threads(8);
  config.set_intra_op_parallelism_threads(8);
  config.set_use_per_session_threads(false);
  Session* session = 0;
  Status status = NewSession(options, &session);
  if (!status.ok())
  {
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::init: Can't create TensorFlow session: "
                        << status.ToString(),
                        LimaException());
  }
  m_session = std::unique_ptr<Session>(session);
  m_model_path = findFileInPaths(resources_path,
                                 QString::fromUtf8("/TensorFlowTokenizer/%1/%2.model")
                                  .arg(lang_str).arg(model_name));
  load_graph(m_model_path);

  // Add the graph to the session
  status = m_session->Create(m_graph_def);
  if (!status.ok())
  {
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::init: Can't add graph to TensorFlow session: "
                        << status.ToString(),
                        LimaException());
  }

  vector<Tensor> out;

  status = m_session->Run({}, {"CRF/crf"}, {}, &out);
  if (!status.ok())
  {
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::init: Can't execute \"Run\" in TensorFlow session: "
                        << status.ToString(),
                        LimaException());
  }

  auto crf = out[0].matrix<float>();

  m_crf.resize(out[0].dim_size(0));
  for (int64 j = 0; j < out[0].dim_size(0); j++)
  {
    m_crf[j].resize(out[0].dim_size(1));
    for (int64 k = 0; k < out[0].dim_size(1); k++)
    {
      m_crf[j][k] = crf(j, k);
    }
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
  QFile file(config_file_name);

  if (!file.open(QIODevice::ReadOnly))
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::load_config can't load config from \""
                        << config_file_name << "\".",
                        LimaException());

  QByteArray bytes = file.readAll();
  QJsonDocument data = QJsonDocument::fromJson(bytes);

  QJsonObject conf = data.object().value("conf").toObject();

  QJsonObject::ConstIterator i = conf.constFind("ignoreEOL");
  if (conf.constEnd() != i)
  {
    m_ignoreEOL = conf.value("ignoreEOL").toBool();
  }

  m_max_seq_len = conf.value("max_seq_len").toInt();
  load_string_array(conf.value("i2t").toArray(), m_i2t);

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

  load_ngram_defs(conf.value("ngrams").toArray());
  load_dicts(data.object().value("dicts").toObject().value("ngrams").toArray());

  i = conf.constFind("transduction_rules");
  if (conf.constEnd() != i)
  {
    QJsonArray jsa = conf.value("transduction_rules").toArray();
    for (QJsonArray::const_iterator i = jsa.begin(); i != jsa.end(); ++i)
    {
      QJsonObject obj = (*i).toObject();

      if (obj.value("token").isUndefined())
        LOG_ERROR_AND_THROW("TensorFlowTokenizer::load_config config file \""
              << config_file_name << "\" incorrect transduction rule.",
          LimaException());
      if (!obj.value("token").isString())
        LOG_ERROR_AND_THROW("TensorFlowTokenizer::load_config config file \""
              << config_file_name << "\" incorrect transduction rule: no/wrong \"token\".",
          LimaException());
      QString token = obj.value("token").toString();

      if (obj.value("words").isUndefined())
        LOG_ERROR_AND_THROW("TensorFlowTokenizer::load_config config file \""
              << config_file_name << "\" incorrect transduction rule.",
          LimaException());
      if (!obj.value("words").isArray())
        LOG_ERROR_AND_THROW("TensorFlowTokenizer::load_config config file \""
              << config_file_name << "\" incorrect transduction rule: no/wrong \"words\".",
          LimaException());
      vector<QString> words;
      load_string_array(obj.value("words").toArray(), words);

      if (m_trrules.find(token) != m_trrules.end())
          LOG_ERROR_AND_THROW("TensorFlowTokenizer::load_config config file \""
                << config_file_name << "\" incorrect transduction rule: duplicate items.",
            LimaException());

      m_trrules[token] = words;
    }
  }
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
                                                         size_t& start,
                                                         size_t size,
                                                         size_t overlap,
                                                         vector<pair<string, Tensor>>& batch)
{
  batch.clear();

  int64 reserve = size;
  //if (0 == reserve)
  //  reserve = 1 + (ngram_idxs[0].size() / 400);

  Tensor seq_len_tensor(DT_INT32, TensorShape({reserve}));
  auto seq_len = seq_len_tensor.tensor<int, 1>();

  batch.push_back( { "seq_len", seq_len_tensor } );

  vector<size_t> indices_tensor_pos;
  indices_tensor_pos.reserve(m_ngram_defs.size());

  for (size_t i = 0; i < m_ngram_defs.size(); ++i)
  {
    Tensor indices_tensor(DT_INT32,
                          TensorShape({reserve,
                            static_cast<long long>(m_max_seq_len)}));
    auto buff = QString::fromUtf8("ngram_idx_%1-%2")
      .arg(m_ngram_defs[i].m_start).arg(m_ngram_defs[i].m_len);
    string tensor_name = buff.toStdString();

    batch.push_back( { tensor_name, indices_tensor } );
    indices_tensor_pos.push_back(batch.size() - 1);
  }

  size_t seq_num = 0;
  for (size_t iter = 0; decltype(reserve)(iter) < reserve; iter++)
  {
    size_t p = start + iter * (m_max_seq_len - overlap * 2);
    size_t this_seq_len = 0;
    if (p < ngram_idxs[0].size())
    {
      this_seq_len = (p + m_max_seq_len < ngram_idxs[0].size()) ? m_max_seq_len : (ngram_idxs[0].size() - p);
    }
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

  start = start + reserve * (m_max_seq_len - overlap * 2);

  return TokStatusCode::SUCCESS;
}

void CppUppsalaTokenizerPrivate::append_new_word(vector< TPrimitiveToken >& current_sentence,
                                                 const u32string& current_token,
                                                 int current_token_offset) const
{
  QString ctoken_lower = QString::fromStdU32String(current_token).toLower();

  map<QString, vector<QString>>::const_iterator i = m_trrules.find(ctoken_lower);
  if (i == m_trrules.end())
  {
    current_sentence.push_back(TPrimitiveToken(QString::fromStdU32String(current_token),
                                               current_token_offset));
  }
  else
  {
    size_t n = 0;
    for (const QString& w : i->second)
    {
      if (n == 0)
        current_sentence.push_back(TPrimitiveToken(w,
                                                   current_token_offset,
                                                   QString::fromStdU32String(current_token)));
      else
          current_sentence.push_back(TPrimitiveToken(w, current_token_offset));
    }
  }
}

void CppUppsalaTokenizerPrivate::tokenize(const QString& text, vector<vector<TPrimitiveToken>>& sentences)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "CppUppsalaTokenizerPrivate::tokenize" << text.left(100));
  TimeUtils::updateCurrentTime();
  sentences.clear();
  sentences.reserve(text.size() / 15);

  u32string SENTENCE_BREAKS = QString::fromUtf8("\u000A\u000D").toStdU32String();
  u32string SPACE_CHARACTERS = QString::fromUtf8("\u0020\u000A\u000B\u000C\u000D").toStdU32String();

  // Encode text
  QString simplified_copy = text;
  simplified_copy.replace(QString::fromUtf8("\u00A0"), " ");

  if (m_ignoreEOL)
  {
    simplified_copy.replace(QString::fromUtf8("\u000A"), "");
    simplified_copy.replace(QString::fromUtf8("\u000D"), "");
  }
  else
  {
    simplified_copy.replace(QString::fromUtf8("\u000A"), " ");
    simplified_copy.replace(QString::fromUtf8("\u000D"), " ");
  }
  auto u32Copy = simplified_copy.toStdU32String();
  u32string wtext;
  wtext.assign(SPACE);
  wtext.append(u32Copy);
  wtext.append(SPACE);

  u32string original_text;
  original_text.assign(SPACE);
  if (m_ignoreEOL)
    original_text.append(simplified_copy.toStdU32String());
  else
    original_text.append(text.toStdU32String());
  original_text.append(SPACE);

  vector<vector<unsigned int>> ngram_idxs;
  if (encode_text(wtext, ngram_idxs) != TokStatusCode::SUCCESS)
    LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::tokenize: error while encoding text to ngrams' indices.",
                        LimaException());

  // Generate batch
  size_t iter_start = 0;
  size_t num_batches = 128;
  size_t overlap = 50;

  //vector< vector< TPrimitiveToken > > sentences;
  vector< TPrimitiveToken > current_sentence;
  u32string current_token;
  int current_token_offset = 0;

  while (iter_start < ngram_idxs[0].size())
  {
    size_t old_iter_start = iter_start;
    vector<pair<string, Tensor>> inputs;
    if (generate_batch(ngram_idxs, iter_start, num_batches, overlap, inputs) != TokStatusCode::SUCCESS)
      LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::tokenize: error while generating batch.",
                          LimaException());

    // Run model
    vector<Tensor> out;
    Status status = m_session->Run(inputs, {"Dense/dropout/Identity"}, {}, &out);
    if (!status.ok())
      LOG_ERROR_AND_THROW("CppUppsalaTokenizerPrivate::tokenize: Can't execute \"Run\" in TensorFlow session: "
                          << status.ToString(),
                          LimaException());

    auto scores = out[0].tensor<float, 3>();
    for (int64 i = 0; i < out[0].dim_size(0); i++)
    {
      size_t len = m_max_seq_len;
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
        unsigned int pos = old_iter_start + i * (m_max_seq_len - overlap * 2) + j;
        if (pos + 2 >= original_text.size())
          break;

        // TODO: check for \r\n and other combinations too
        if (SENTENCE_BREAKS.find(original_text[pos+1]) != u32string::npos
            && SENTENCE_BREAKS.find(original_text[pos+2]) != u32string::npos
            && original_text[pos+1] == original_text[pos+2]
            && j > 0)
        {
          size_t t = j-1;
          while (viterbi[t] == m_token_outside && t > 0)
            t -= 1;

          if (viterbi[t] == m_token_end)
            viterbi[t] = m_token_end_last;

          if (viterbi[t] == m_token_single)
            viterbi[t] = m_token_single_last;
        }

        if (SPACE_CHARACTERS.find(original_text[pos+1]) == u32string::npos
                && viterbi[j] == m_token_outside)
        {
          viterbi[j] = m_token_inside;
          if (j+1 < viterbi.size())
          {
            if (viterbi[j+1] == m_token_begin || viterbi[j+1] == m_token_outside)
              viterbi[j] = m_token_single;
          }
        }
      }

      for (size_t j = ((i > 0 || old_iter_start > 0) ? overlap : 0); j < viterbi.size() - overlap; ++j)
      {
        unsigned int pos = old_iter_start + i * (m_max_seq_len - overlap * 2) + j;
        if (pos + 1 >= wtext.size())
          break;
        const auto &tag = viterbi[j];

        if (m_token_end == tag || m_token_end_last == tag || m_token_single == tag || m_token_single_last == tag)
        {
          if (current_token.size() > 0 || SPACE[0] != wtext[pos + 1])
          {
            if (m_token_single == tag || m_token_single_last == tag || 0 == current_token.size())
              current_token_offset = pos;

            current_token += wtext[pos + 1];
          }

          if (current_token.size() > 0 && current_token != SPACE)
          {
            append_new_word(current_sentence, current_token, current_token_offset);

            //if (current_sentence.size() > 1
            //        && current_sentence[current_sentence.size() - 2].second == current_token_offset)
            //    throw;
            current_token.clear();
          }
        }
        else if (m_token_begin == tag || m_token_inside == tag)
        {
          if (current_token.size() > 0 || SPACE[0] != wtext[pos + 1])
          {
            if (m_token_begin == tag || 0 == current_token.size())
              current_token_offset = pos;

            current_token += wtext[pos + 1];
          }
        }

        if (m_token_end_last == tag || m_token_single_last == tag)
        {
          sentences.push_back(current_sentence);
          current_sentence.clear();
        }
      }
    }
  }

  if (current_token.size() > 0)
    append_new_word(current_sentence, current_token, current_token_offset);

  if (current_sentence.size() > 0)
    sentences.push_back(current_sentence);

  LOG_MESSAGE(LINFO, "End of Tokenizer");
  TimeUtils::logElapsedTime("CppUppsalaTokenizerPrivate");
}

} // namespace Tokenizer
} // namespace TensorFlowUnits
} // namespace LinguisticProcessing
} // namespace Lima
