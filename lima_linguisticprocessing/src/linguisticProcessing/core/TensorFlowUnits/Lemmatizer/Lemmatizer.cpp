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

#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/cc/client/client_session.h"

#include "QJsonHelpers.h"
#include "TensorFlowHelpers.h"
#include "Cache.h"

#include "Lemmatizer.h"


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
namespace Lemmatizer
{

SimpleFactory<MediaProcessUnit, TensorFlowLemmatizer> tensorflowmorphosyntaxFactory(TENSORFLOWLEMMATIZER_CLASSID);

#define LOG_ERROR_AND_THROW(msg, exc) { \
                                        TENSORFLOWLEMMATIZERLOGINIT; \
                                        LERROR << msg; \
                                        throw exc; \
                                      }

#if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
  #define LOG_MESSAGE(stream, msg) { stream << msg; }
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) TENSORFLOWLEMMATIZERLOGINIT; LOG_MESSAGE(stream, msg);
#else
  #define LOG_MESSAGE(stream, msg) ;
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif

CONFIGURATIONHELPER_LOGGING_INIT(TENSORFLOWLEMMATIZERLOGINIT);

class TensorFlowLemmatizerPrivate : public ConfigurationHelper
{
public:
  TensorFlowLemmatizerPrivate()
    : ConfigurationHelper("TensorFlowLemmatizerPrivate", THIS_FILE_LOGGING_CATEGORY()),
      m_stringsPool(nullptr),
      m_lemmatization_required(false),
      m_batch_size(0),
      m_max_input_len(0),
      m_ctx_len(0),
      m_beam_size(0),
      m_feat_separator(LimaString::fromUtf8("|")) { }
  ~TensorFlowLemmatizerPrivate();

  void init(GroupConfigurationStructure&,
            MediaId lang);

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
      TENSORFLOWLEMMATIZERLOGINIT;
      LDEBUG << "TSentence::append" << v << t->stringForm();
#endif
      if (tokens.size() <= token_count)
      {
        tokens.resize(tokens.size() * 2);
        QString errorString;
        QTextStream ts(&errorString);
        //ts << "TSentence::append tokens size (" << tokens.size() << ") <= token_count (" << token_count << ")";
        //TENSORFLOWLEMMATIZERLOGINIT;
        //LWARN << errorString;
      }

      tokens[token_count].vertex = v;
      tokens[token_count].token = t;
      tokens[token_count].morpho = m;
      token_count += 1;
    }
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
      string m_tf_name;

      const PropertyAccessor* accessor;

      void fill_linguistic_codes(const PropertyManager& pm)
      {
        for (const auto& kv : m_w2i)
        {
          if (kv.first == "#None")
            m_c2i[LinguisticCode(0)] = kv.second;
          else
          {
            LinguisticCode code = pm.getPropertyValue(kv.first);
            if (LinguisticCode(0) == code)
            {
              LOG_MESSAGE_WITH_PROLOG(LWARN, "Unknown property value: \"" << kv.first << "\".");
            }
            else
            {
              m_c2i[code] = kv.second;
            }
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

  u32string m_unk;
  u32string m_eos;

  map<string, string> m_input_node_names;

  QString m_model_path;

  unique_ptr<Session> m_session;
  GraphDef m_graph_def;

  void load_config(const QString& config_file_name);
  void load_graph(const QString& model_path, GraphDef* graph_def);
  void load_cache(const QString& file_name);
  void save_cache(const QString& file_name);

  // Json parsing
  QJsonObject get_json_object(const QJsonObject& root, const QString& child_name);
  QJsonArray get_json_array(const QJsonObject& root, const QString& child_name);

  bool m_lemmatization_required;
  LemmatizerConf m_lemmatizer_conf;
  size_t m_batch_size;
  size_t m_max_input_len;
  size_t m_ctx_len;
  size_t m_beam_size;

  set<LinguisticCode> m_dont_lemmatize;
  LimaString m_main_alphabet;
  LimaString m_special_chars;
  LRUCache<LimaString, LimaString> m_cache;
  LimaString m_feat_separator;

  struct TFormOccurrences
  {
    StringsPoolIndex form;
    StringsPoolIndex lemma;
    bool firstWord;
    set<MorphoSyntacticData::iterator> occurrences;

    TFormOccurrences() : firstWord(false) {}
  };

  void lemmatize(vector<TSentence>& sentences, SyntacticData& syntacticData);
  size_t get_code_for_feature(const TSentence &sent, size_t tid, const LemmatizerConf::dict<string> &d) const;
  size_t get_code_for_feature(const TToken &token, const LemmatizerConf::dict<string> &d) const;
  size_t get_code_for_feature(const LinguisticElement &le, const LemmatizerConf::dict<string> &d) const;

  void generate_batch(const vector<TSentence>& sentences,
                      size_t& current_sentence,
                      size_t& current_token,
                      vector<pair<string, Tensor>>& batch) const;

  void generate_batch(const vector<TSentence>& sentences,
                      const vector<pair<size_t, size_t>>& tokens_to_lemmatize,
                      vector<pair<string, Tensor>>& batch) const;

  void generate_batch(const vector<TFormOccurrences*>& forms_to_lemmatize,
                      vector<pair<string, Tensor>>& batch) const;

  void encode_token_for_batch(const vector<TSentence>& sentences,
                                         size_t current_sentence,
                                         size_t current_token,
                                         const map<string, size_t> &feat2idx,
                                         size_t batch_item_idx,
                                         vector<pair<string, Tensor>>& batch) const;

  void encode_token_for_batch(const TFormOccurrences* form_occurrences,
                                         const map<string, size_t> &feat2idx,
                                         size_t batch_item_idx,
                                         vector<pair<string, Tensor>>& batch) const;


  void set_token_lemma(vector<TSentence>& sentences, const u32string& lemma, size_t current_sentence, size_t current_token) const;
  void set_token_lemma(TToken &token, const u32string& lemma) const;
  void set_token_lemma(TToken &token, const LimaString& lemma) const;
  void set_token_lemma(MorphoSyntacticData::iterator& md_it, const u32string& lemma) const;
  void set_token_lemma(MorphoSyntacticData::iterator& md_it, const LimaString& lemma) const;
  void set_token_lemma(MorphoSyntacticData::iterator& md_it, StringsPoolIndex lemma_idx) const;

  void create_batch_template(vector<pair<string, Tensor>>& batch,
                                   map<string, size_t>& feat2idx) const;

  void lemmatize_with_model(vector<TSentence>& sentences,
                            const vector<pair<size_t, size_t>> tokens_to_lemmatize);

  void lemmatize_with_model(vector<map<StringsPoolIndex, map<LimaString, TFormOccurrences>>>& buckets);

  void process_batch(vector<TFormOccurrences*>& forms_for_batch, size_t pos) const ;

  LimaString create_form_key(const TToken& token, const LimaString& extra) const;
  LimaString create_form_key(const LimaString& form, const map<string, string>& features) const;
  LimaString create_feat_str(const TToken& token, const LimaString& extra) const;

  bool hasCharFromList(const LimaString& str, const LimaString& char_list) const;
  bool hasNoCharsFromList(const LimaString& str, const LimaString& char_list) const;
};

TensorFlowLemmatizer::TensorFlowLemmatizer()
    : m_d(new TensorFlowLemmatizerPrivate())
{
}

TensorFlowLemmatizer::~TensorFlowLemmatizer()
{
  delete m_d;
}

void TensorFlowLemmatizer::init(GroupConfigurationStructure& gcs,
                                Manager* manager)
{
  m_d->init(gcs, manager->getInitializationParameters().media);
}

TensorFlowLemmatizerPrivate::~TensorFlowLemmatizerPrivate()
{
  if (m_session.get() != nullptr)
  {
    auto status = m_session->Close();
    if (!status.ok())
    {
      LOG_MESSAGE_WITH_PROLOG(LERROR, "TensorFlowLemmatizerPrivate::~TensorFlowLemmatizerPrivate(): Error closing session:"
                              << status.ToString());
    }
  }
}

void TensorFlowLemmatizerPrivate::init(GroupConfigurationStructure& gcs,
                                       MediaId lang)
{
  m_language = lang;
  m_stringsPool = &MediaticData::changeable().stringsPool(m_language);

  QString lang_str = MediaticData::single().media(m_language).c_str();
  QString resources_path = MediaticData::single().getResourcesPath().c_str();

  string udlang;
  MediaticData::single().getOptionValue("udlang", udlang);

  if (lang_str != QString("ud") || udlang.find("ud-") == 0)
  {
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
        LOG_ERROR_AND_THROW("TensorFlowLemmatizerPrivate::init: Can't parse language id "
                            << udlang, LimaException());
      }
    }
  }

  QString model_name = getStringParameter(gcs, "model_prefix", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY).c_str();
  model_name.replace(QString("$udlang"), QString(udlang.c_str()));

  auto config_file_name = findFileInPaths(resources_path,
                                          QString::fromUtf8("/TensorFlowLemmatizer/%1/%2.conf")
                                            .arg(lang_str).arg(model_name));
  load_config(config_file_name);

  if (!m_lemmatization_required)
  {
    return;
  }
  auto cache_file_name = findFileInPaths(resources_path,
                                          QString::fromUtf8("/TensorFlowLemmatizer/%1/%2.cache")
                                            .arg(lang_str).arg(model_name));

  m_cache.set_max_size(2000000);

  if (cache_file_name.size() > 0)
    load_cache(cache_file_name);

  tensorflow::SessionOptions options;
  tensorflow::ConfigProto & config = options.config;
  //config.set_inter_op_parallelism_threads(2);
  //config.set_intra_op_parallelism_threads(0);
  config.set_use_per_session_threads(true);
  config.set_isolate_session_state(true);
  Session* session = 0;
  Status status = NewSession(options, &session);
  if (!status.ok())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::init: Can't create TensorFlow session: "
                        << status.ToString(),
                        LimaException());
  m_session = unique_ptr<Session>(session);

  m_model_path = findFileInPaths(resources_path,
                                 QString::fromUtf8("/TensorFlowLemmatizer/%1/%2.model")
                                  .arg(lang_str).arg(model_name));
  load_graph(m_model_path, &m_graph_def);

  // Add the graph to the session
  status = m_session->Create(m_graph_def);
  if (!status.ok())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::init: Can't add graph to TensorFlow session: "
                        << status.ToString(),
                        LimaException());
}

QJsonObject TensorFlowLemmatizerPrivate::get_json_object(const QJsonObject& root, const QString& child_name)
{
  QJsonValue value = root.value(child_name);
  if (value.isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::get_json_object can't get value \""
                        << child_name << "\"", LimaException());

  QJsonObject object = value.toObject();

  //if (object.isEmpty())
  //  LOG_ERROR_AND_THROW("TensorFlowLemmatizer::get_json_object object \""
  //                      << child_name << "\" is empty", LimaException());

  return object;
}

QJsonArray TensorFlowLemmatizerPrivate::get_json_array(const QJsonObject& root, const QString& child_name)
{
  QJsonValue value = root.value(child_name);
  if (value.isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::get_json_object can't get value \""
                        << child_name << "\"", LimaException());

  QJsonArray array = value.toArray();

  return array;
}

void TensorFlowLemmatizerPrivate::load_config(const QString& config_file_name)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "TensorFlowLemmatizerPrivate::load_config" << config_file_name)

  QFile file(config_file_name);

  if (!file.open(QIODevice::ReadOnly))
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config can't load config from \""
                        << config_file_name << "\".",
                        LimaException());

  QByteArray bytes = file.readAll();
  QJsonDocument data = QJsonDocument::fromJson(bytes);
  if (data.isNull())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config can't load config from \""
                        << config_file_name << "\". Invalid Json.",
                        LimaException());
  if (!data.isObject())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config can't load config from \""
                        << config_file_name << "\". Loaded data is not an object",
                        LimaException());

  if (!data.object().value("lemmatize").isUndefined())
  {
    if (!data.object().value("lemmatize").isBool())
    {
      LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
                          << config_file_name << "\": lemmatize parameter must be bool.",
                          LimaException());
    }

    m_lemmatization_required = data.object().value("lemmatize").toBool();

    if (!m_lemmatization_required)
      return;
  }

  m_lemmatization_required = true;

  // batch_size
  if (data.object().value("batch_size").isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" missing param batch_size.",
      LimaException());
  if (!data.object().value("batch_size").isDouble())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" param batch_size is not a number.",
      LimaException());
  m_batch_size = data.object().value("batch_size").toInt();

  QJsonObject encoder_conf = get_json_object(data.object(), "encoder");

  // max_input_len
  if (encoder_conf.value("max_len").isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" missing param max_len.",
      LimaException());
  if (!encoder_conf.value("max_len").isDouble())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" param max_len is not a number.",
      LimaException());
  m_max_input_len = encoder_conf.value("max_len").toInt();

  // ctx_len
  if (encoder_conf.value("ctx_len").isUndefined())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" missing param ctx_len.",
      LimaException());
  if (!encoder_conf.value("ctx_len").isDouble())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" param ctx_len is not a number.",
      LimaException());
  m_ctx_len = encoder_conf.value("ctx_len").toInt();

  // beam_size
  m_beam_size = 5;
  QJsonObject decoder_conf = get_json_object(data.object(), "decoder");
  if (decoder_conf.value("beam_size").isUndefined())
  {
    LOG_MESSAGE(LERROR, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" missing param beam_size.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" missing param beam_size.",
      LimaException());*/
  }
  else if (!decoder_conf.value("beam_size").isDouble())
  {
    LOG_MESSAGE(LERROR, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" param beam_size is not a number.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" param beam_size is not a number.",
      LimaException());*/
  }
  else
  {
    m_beam_size = decoder_conf.value("beam_size").toInt();
  }

  load_string_array(get_json_array(encoder_conf, "i2c"), m_lemmatizer_conf.encoder_dict.m_i2w);
  load_string_to_uint_map(get_json_object(encoder_conf, "c2i"), m_lemmatizer_conf.encoder_dict.m_w2i);

  load_string_array(get_json_array(decoder_conf, "i2c"), m_lemmatizer_conf.decoder_dict.m_i2w);
  load_string_to_uint_map(get_json_object(decoder_conf, "c2i"), m_lemmatizer_conf.decoder_dict.m_w2i);

  vector<string> feats_to_use;
  load_string_array(get_json_array(encoder_conf, "feats_to_use"), feats_to_use);

  const auto &pcm = static_cast<const LanguageData&>(MediaticData::single().mediaData(m_language)).getPropertyCodeManager();

  QJsonObject all_feats = get_json_object(encoder_conf, "feats");
  for (const string& f : feats_to_use)
  {
    QJsonObject feature = get_json_object(all_feats, f.c_str());
    m_lemmatizer_conf.feature_dicts[f] = LemmatizerConf::dict<string>();
    m_lemmatizer_conf.feature_dicts[f].m_tf_name = f;
    if (!feature.value("name_tf").isUndefined())
    {
      if (!feature.value("name_tf").isString())
      {
        LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
              << config_file_name << "\" param " << f << "/name_tf is not a string.",
          LimaException());
      }

      m_lemmatizer_conf.feature_dicts[f].m_tf_name = feature.value("name_tf").toString().toStdString();
    }
    load_string_array(get_json_array(feature, "i2c"), m_lemmatizer_conf.feature_dicts[f].m_i2w);
    load_string_to_uint_map(get_json_object(feature, "c2i"), m_lemmatizer_conf.feature_dicts[f].m_w2i);
    if (f != "FirstWord")
    {
      try
      {
        m_lemmatizer_conf.feature_dicts[f].accessor = &(pcm.getPropertyAccessor(f));
      }
      catch (const InvalidConfiguration& e)
      {
        continue;
      }
      m_lemmatizer_conf.feature_dicts[f].fill_linguistic_codes(pcm.getPropertyManager(f));
    }
  }

  load_string_array(get_json_array(encoder_conf, "i2t"), m_lemmatizer_conf.pos_dict.m_i2w);
  load_string_to_uint_map(get_json_object(encoder_conf, "t2i"), m_lemmatizer_conf.pos_dict.m_w2i);
  m_lemmatizer_conf.pos_dict.accessor = &(pcm.getPropertyAccessor("MICRO"));
  m_lemmatizer_conf.pos_dict.fill_linguistic_codes(pcm.getPropertyManager("MICRO"));

  m_input_node_names["input"] = "input";
  m_input_node_names["length"] = "length";
  m_input_node_names["context"] = "context";

  for (const string& f : feats_to_use)
  {
    string name = "feat_" + f;
    m_input_node_names[name] = "feat_" + m_lemmatizer_conf.feature_dicts[f].m_tf_name;
  }

  const auto& pm = pcm.getPropertyManager("MICRO");
  if (!data.object().value("dont_lemmatize").isUndefined())
  {
    vector<string> dont_lemmatize_pos_names;
    load_string_array(get_json_array(data.object(), "dont_lemmatize"), dont_lemmatize_pos_names);
    for ( const auto s : dont_lemmatize_pos_names )
    {
      m_dont_lemmatize.insert(pm.getPropertyValue(s));
    }
  }
  else
  {
    LOG_MESSAGE(LERROR, "ERROR: TensorFlowLemmatizerPrivate::load_config: \"dont_lemmatize\" isn't defined.");
    m_dont_lemmatize.insert(pm.getPropertyValue("PUNCT"));
  }

  if (data.object().value("main_alphabet").isUndefined())
  {
    LOG_MESSAGE(LERROR, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" missing param main_alphabet.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" missing param main_alphabet.",
      LimaException());*/
  }
  else if (!data.object().value("main_alphabet").isString())
  {
    LOG_MESSAGE(LERROR, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" param main_alphabet is not a string.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" param main_alphabet is not a string.",
      LimaException());*/
  }
  else
  {
    m_main_alphabet = data.object().value("main_alphabet").toString();
  }

  if (data.object().value("special_chars").isUndefined())
  {
    LOG_MESSAGE(LERROR, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" missing param special_chars.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" missing param special_chars.",
      LimaException());*/
  }
  else if (!data.object().value("special_chars").isString())
  {
    LOG_MESSAGE(LERROR, "TensorFlowLemmatizer::load_config config file \""
                << config_file_name << "\" param special_chars is not a string.");
    /*LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_config config file \""
          << config_file_name << "\" param special_chars is not a string.",
      LimaException());*/
  }
  else
  {
    m_special_chars = data.object().value("special_chars").toString();
  }
}

void TensorFlowLemmatizerPrivate::add_linguistic_element(MorphoSyntacticData* msdata, const Token &token) const
{
  if (msdata == nullptr)
    LOG_ERROR_AND_THROW("TensorFlowLemmatizerPrivate::add_linguistic_element: msdata == nullptr",
                        LimaException());

  LinguisticElement elem;
  elem.inflectedForm = token.form();
  elem.lemma = token.form();
  elem.normalizedForm = token.form();
  elem.type = SIMPLE_WORD;

  msdata->push_back(elem);
}

LimaStatusCode TensorFlowLemmatizer::process(AnalysisContent& analysis) const
{
  return m_d->process(analysis);
}

LimaStatusCode TensorFlowLemmatizerPrivate::process(AnalysisContent& analysis)
{
  if (!m_lemmatization_required)
  {
    return SUCCESS_ID;
  }
  TimeUtils::updateCurrentTime();

  LOG_MESSAGE_WITH_PROLOG(LINFO, "Start of TensorFlowLemmatizer");

  AnalysisGraph* anagraph = static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  AnalysisGraph* posgraph = static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  LinguisticGraph* src_graph = anagraph->getGraph();

  auto sb = static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb == nullptr)
  {
    LOG_MESSAGE(LERROR, "No SentenceBounds");
    return MISSING_DATA;
  }

  SyntacticData* syntacticData = static_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  // graph is empty if it has only 2 vertices, start (0) and end (0)
  if (num_vertices(*src_graph) <= 2)
  {
      return SUCCESS_ID;
  }

  LinguisticGraph* result_graph = posgraph->getGraph();

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

    TSentence sent(beginSentence, endSentence, 100);

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

  lemmatize(sentences, *syntacticData);

  LOG_MESSAGE(LINFO, "End of TensorFlowLemmatizer");
  TimeUtils::logElapsedTime("TensorFlowLemmatizer");

  return SUCCESS_ID;
}

bool TensorFlowLemmatizerPrivate::hasCharFromList(const LimaString& str, const LimaString& char_list) const
{
  for (int i = 0; i < str.size(); i++)
  {
    if (char_list.indexOf(str[i]) != -1)
      return true;
  }

  return false;
}

bool TensorFlowLemmatizerPrivate::hasNoCharsFromList(const LimaString& str, const LimaString& char_list) const
{
  for (int i = 0; i < str.size(); i++)
  {
    if (char_list.indexOf(str[i]) != -1)
      return false;
  }

  return true;
}

void TensorFlowLemmatizerPrivate::set_token_lemma(TToken &token, const u32string& lemma) const
{
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
  string utf8_lemma = cvt.to_bytes(lemma);

  set_token_lemma(token, LimaString(utf8_lemma.c_str()));
}

void TensorFlowLemmatizerPrivate::set_token_lemma(TToken &token, const LimaString& lemma) const
{
  FsaStringsPool* stringsPool = &MediaticData::changeable().stringsPool(m_language);

  if (token.morpho != nullptr)
  {
    StringsPoolIndex lemma_idx = (*stringsPool)[lemma];
    (*token.morpho)[0].lemma = lemma_idx ;
  }
}

void TensorFlowLemmatizerPrivate::set_token_lemma(MorphoSyntacticData::iterator& md_it, StringsPoolIndex lemma_idx) const
{
  md_it->lemma = lemma_idx;
}

void TensorFlowLemmatizerPrivate::set_token_lemma(MorphoSyntacticData::iterator& md_it, const LimaString& lemma) const
{
  set_token_lemma(md_it, (*m_stringsPool)[lemma]);
}

void TensorFlowLemmatizerPrivate::set_token_lemma(MorphoSyntacticData::iterator& md_it, const u32string& lemma) const
{
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
  string utf8_lemma = cvt.to_bytes(lemma);

  set_token_lemma(md_it, LimaString(utf8_lemma.c_str()));
}

void TensorFlowLemmatizerPrivate::set_token_lemma(vector<TSentence>& sentences, const u32string& lemma, size_t current_sentence, size_t current_token) const
{
  FsaStringsPool* stringsPool = &MediaticData::changeable().stringsPool(m_language);

  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
  string utf8_lemma = cvt.to_bytes(lemma);

  TSentence &sent = sentences[current_sentence];
  TToken &token = sent.tokens[current_token];

  LimaString src_form = (*m_stringsPool)[token.token->form()];
  if ((0 == m_main_alphabet.size() || hasCharFromList(src_form, m_main_alphabet))
      && (0 == m_special_chars.size() || hasNoCharsFromList(src_form, m_special_chars)))
  {
    if (token.morpho != nullptr && token.morpho->size() > 0)
    {
      StringsPoolIndex lemma_idx = (*stringsPool)[QString(utf8_lemma.c_str())];
      (*token.morpho)[0].lemma = lemma_idx ;
    }
  }
}

LimaString TensorFlowLemmatizerPrivate::create_form_key(const LimaString& form,
                                                        const map<string, string>& features) const
{
  LimaString key = form + LimaString::fromUtf8("\t");

  auto it = features.find("upos");
  if (features.end() == it)
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::create_form_key upos is missing", LimaException());
  key += LimaString::fromStdString(it->second);

  for (const auto& kv : m_lemmatizer_conf.feature_dicts)
  {
    auto it = features.find(kv.first);
    if (features.end() == it)
      continue;

    key += m_feat_separator
            + LimaString::fromStdString(kv.first)
            + LimaString::fromUtf8("=")
            + LimaString::fromStdString(it->second);
  }

  return key;
}

LimaString TensorFlowLemmatizerPrivate::create_feat_str(const TToken& token, const LimaString& extra = LimaString("")) const
{
  size_t code = get_code_for_feature(token, m_lemmatizer_conf.pos_dict);
  LimaString key = LimaString::fromStdString(m_lemmatizer_conf.pos_dict.m_i2w[code]);
  bool add_extra = (extra.size() > 0);

  for (const auto& kv : m_lemmatizer_conf.feature_dicts)
  {
    auto it = m_lemmatizer_conf.feature_dicts.find(kv.first);
    const LemmatizerConf::dict<string> &rd = it->second;
    if (rd.accessor == nullptr)
      continue;
    size_t code = get_code_for_feature(token, rd);
    if (0 == code)
      continue;

    LimaString k = LimaString::fromStdString(kv.first);
    if (add_extra && extra < k)
    {
      key += m_feat_separator + extra;
      add_extra = false;
    }

    key += m_feat_separator + k
            + LimaString::fromUtf8("=")
            + LimaString::fromStdString(kv.second.m_i2w[code]);
  }

  if (add_extra)
  {
    if (key.size() > 0)
      key += m_feat_separator;
    key += extra;
  }

  return key;
}

LimaString TensorFlowLemmatizerPrivate::create_form_key(const TToken& token, const LimaString& extra = LimaString("")) const
{
  LimaString key = (*m_stringsPool)[token.token->form()]
                 + LimaString::fromUtf8("\t")
                 + create_feat_str(token, extra);

  return key;
}

void TensorFlowLemmatizerPrivate::lemmatize_with_model(vector<TSentence>& sentences,
                                                       const vector<pair<size_t, size_t>> tokens_to_lemmatize)
{
  vector<string> requested_nodes;
  requested_nodes.push_back("Decoder/decoder_with_shared_attention_mechanism_1/decoder/transpose");

  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
  u32string EOS = cvt.from_bytes("<EOS>");
  size_t idx_EOS = m_lemmatizer_conf.decoder_dict.m_w2i.find(EOS)->second;

  // Generate batch
  vector<pair<string, Tensor>> inputs;
  generate_batch(sentences, tokens_to_lemmatize, inputs);

  // Run model
  vector<Tensor> out;
  Status status = m_session->Run(inputs, requested_nodes, {}, &out);
  if (!status.ok())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizerPrivate::lemmatize: Can't execute \"Run\" in TensorFlow session: "
                        << status.ToString(),
                        LimaException());

  auto sample_id = out[0].tensor<int, 3>();

  for (int i = 0; i < sample_id.dimension(0) && size_t(i) < tokens_to_lemmatize.size(); i++)
  {
    u32string lemma;
    int c = 0;
    while ((size_t)sample_id(i, c, 0) != idx_EOS && c < sample_id.dimension(1))
    {
      char32_t ch = sample_id(i, c, 0);
      lemma += m_lemmatizer_conf.decoder_dict.m_i2w[ch];
      c++;
    }

    TSentence &sent = sentences[tokens_to_lemmatize[i].first];
    TToken &token = sent.tokens[tokens_to_lemmatize[i].second];

    LimaString src_form = (*m_stringsPool)[token.token->form()];
    size_t src_size = src_form.size();
    int size_diff = int(lemma.size()) - src_size;
    if (size_diff >= 10)
      lemma = src_form.toStdU32String();

    set_token_lemma(sentences, lemma, tokens_to_lemmatize[i].first, tokens_to_lemmatize[i].second);

    LimaString src_form_key = create_form_key(token);
    m_cache.put(src_form_key, LimaString::fromStdU32String(lemma));
  }
}

void TensorFlowLemmatizerPrivate::lemmatize_with_model(vector<map<StringsPoolIndex, map<LimaString, TFormOccurrences>>>& buckets)
{
  vector<string> requested_nodes;
  requested_nodes.push_back("Decoder/decoder_with_shared_attention_mechanism_1/decoder/transpose");

  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
  u32string EOS = cvt.from_bytes("<EOS>");

  vector<TFormOccurrences*> forms_for_batch;
  forms_for_batch.resize(m_batch_size);

  size_t pos = 0;
  for (size_t i = 0; i < buckets.size(); i++)
  {
    for (auto form_it = buckets[i].begin(); form_it != buckets[i].end(); form_it++)
    {
      map<LimaString, TFormOccurrences>& homonyms = form_it->second;
      for (auto feat_it = homonyms.begin(); feat_it != homonyms.end(); feat_it++)
      {
        forms_for_batch[pos] = &(feat_it->second);
        pos++;
        if (pos == m_batch_size)
        {
          process_batch(forms_for_batch, pos);
          pos = 0;
        }
      }
    }
  }

  if (pos > 0)
  {
    process_batch(forms_for_batch, pos);
  }
}

void TensorFlowLemmatizerPrivate::process_batch(vector<TFormOccurrences*>& forms_for_batch, size_t pos) const
{
  vector<string> requested_nodes;
  requested_nodes.push_back("Decoder/decoder_with_shared_attention_mechanism_1/decoder/transpose");

  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
  u32string EOS = cvt.from_bytes("<EOS>");
  size_t idx_EOS = m_lemmatizer_conf.decoder_dict.m_w2i.find(EOS)->second;

  // Generate batch
  vector<pair<string, Tensor>> inputs;
  generate_batch(forms_for_batch, inputs);

  // Run model
  vector<Tensor> out;
  Status status = m_session->Run(inputs, requested_nodes, {}, &out);
  if (!status.ok())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizerPrivate::lemmatize: Can't execute \"Run\" in TensorFlow session: "
                        << status.ToString(),
                        LimaException());

  auto sample_id = out[0].tensor<int, 3>();

  // Apply results
  for (int j = 0; j < sample_id.dimension(0) && (size_t)j < pos; j++)
  {
    u32string lemma;
    int c = 0;
    while ((size_t)sample_id(j, c, 0) != idx_EOS && c < sample_id.dimension(1))
    {
      char32_t ch = sample_id(j, c, 0);
      lemma += m_lemmatizer_conf.decoder_dict.m_i2w[ch];
      c++;
    }

    LimaString src_form = (*m_stringsPool)[forms_for_batch[j]->form];
    size_t src_size = src_form.size();
    int size_diff = int(lemma.size()) - src_size;
    if (size_diff >= 10)
      lemma = src_form.toStdU32String();

    StringsPoolIndex lemma_idx = (*m_stringsPool)[LimaString::fromStdU32String(lemma)];
    forms_for_batch[j]->lemma = lemma_idx;

    for ( auto md_it : forms_for_batch[j]->occurrences )
    {
      set_token_lemma(md_it, lemma_idx);
    }
  }
}

void TensorFlowLemmatizerPrivate::lemmatize(vector<TSentence>& sentences,
                                            SyntacticData& /*syntacticData*/)
{
  size_t current_sentence = 0;
  size_t current_token = 0;

  vector<pair<size_t, size_t>> tokens_to_lemmatize;
  tokens_to_lemmatize.reserve(m_batch_size);
  vector<map<StringsPoolIndex, map<LimaString, TFormOccurrences>>> buckets;
  buckets.resize(m_max_input_len);

  while (current_sentence < sentences.size())
  {
    if (current_token >= sentences[current_sentence].token_count)
    {
      current_sentence += 1;
      if (current_sentence >= sentences.size())
        break;
      current_token = 0;
    }

    TSentence &sent = sentences[current_sentence];
    TToken &token = sent.tokens[current_token];

    if (nullptr == token.token)
    {
      current_token++;
      continue;
    }

    LimaString src_form_key = create_form_key(token, (0 == current_token ? "FirstWord=Yes" : ""));

    LimaString cached_lemma;
    if (m_cache.get(src_form_key, cached_lemma))
    {
      set_token_lemma(sentences, cached_lemma.toStdU32String(), current_sentence, current_token);
      current_token++;
      continue;
    }

    LinguisticCode pos_code = token.morpho->firstValue(*m_lemmatizer_conf.pos_dict.accessor);
    StringsPoolIndex form_idx = token.token->form();
    const LimaString form = (*m_stringsPool)[form_idx];
    if (m_dont_lemmatize.find(pos_code) != m_dont_lemmatize.end())
    {
      set_token_lemma(token, form);
      current_token++;
      continue;
    }

    if ((0 == m_main_alphabet.size() || hasCharFromList(form, m_main_alphabet))
          && (0 == m_special_chars.size() || hasNoCharsFromList(form, m_special_chars)))
    {
      const LimaString feat_str = create_feat_str(token, (0 == current_token ? "FirstWord=Yes" : ""));
      size_t form_size = form.size();
      if (form_size > buckets.size())
        form_size = buckets.size();
      buckets[form_size - 1][form_idx][feat_str].form = form_idx;
      buckets[form_size - 1][form_idx][feat_str].firstWord = (0 == current_token);
      buckets[form_size - 1][form_idx][feat_str].occurrences.insert(token.morpho->begin());
    }

    current_token++;
  }

  lemmatize_with_model(buckets);

  // Put results to cache
  for (size_t i = 0; i < buckets.size(); i++)
  {
    for (auto form_it = buckets[i].begin(); form_it != buckets[i].end(); form_it++)
    {
      LimaString form = (*m_stringsPool)[form_it->first];
      map<LimaString, TFormOccurrences>& homonyms = form_it->second;
      for (auto feat_it = homonyms.begin(); feat_it != homonyms.end(); feat_it++)
      {
        LimaString k = form + LimaString::fromUtf8("\t") + feat_it->first;
        if (!m_cache.has(k))
        {
          LimaString v = (*m_stringsPool)[feat_it->second.lemma];
          m_cache.put(k, v);
        }
      }
    }
  }

  save_cache("lemmatizer.cache");
}

size_t TensorFlowLemmatizerPrivate::get_code_for_feature(const TToken &token,
                                                         const LemmatizerConf::dict<string> &d) const
{
  LinguisticCode lc = token.morpho->firstValue(*d.accessor);

  auto it = d.m_c2i.find(lc);
  if (d.m_c2i.end() == it)
  {
    LOG_MESSAGE_WITH_PROLOG(LINFO, "WARNING: unknown feature value for word \"" << token.token->stringForm() << "\".");
    auto it = d.m_c2i.find(LinguisticCode(0));
    if (d.m_c2i.end() == it)
      LOG_ERROR_AND_THROW("ERROR: can\'t find #None LinguisticCode.", LimaException());
    return it->second;
  }
  return it->second;
}

size_t TensorFlowLemmatizerPrivate::get_code_for_feature(const TSentence &sent,
                                                         size_t tid,
                                                         const LemmatizerConf::dict<string> &d) const
{
  const TToken t = sent.tokens[tid];
  return get_code_for_feature(t, d);
}

size_t TensorFlowLemmatizerPrivate::get_code_for_feature(const LinguisticElement &le,
                                                         const LemmatizerConf::dict<string> &d) const
{
  LinguisticCode lc = d.accessor->readValue(le.properties);

  auto it = d.m_c2i.find(lc);
  if (d.m_c2i.end() == it)
  {
    LOG_MESSAGE_WITH_PROLOG(LINFO, "WARNING: unknown feature value in get_code_for_feature(const LinguisticElement, ...)\".");
    auto it = d.m_c2i.find(LinguisticCode(0));
    if (d.m_c2i.end() == it)
      LOG_ERROR_AND_THROW("ERROR: can\'t find #None LinguisticCode.", LimaException());
    return it->second;
  }
  return it->second;
}

void TensorFlowLemmatizerPrivate::encode_token_for_batch(const vector<TSentence>& sentences,
                                                         size_t current_sentence,
                                                         size_t current_token,
                                                         const map<string, size_t> &feat2idx,
                                                         size_t batch_item_idx,
                                                         vector<pair<string, Tensor>>& batch) const
{
  auto len = batch[0].second.tensor<int, 1>();
  auto input = batch[1].second.tensor<int, 2>();
  auto context = batch[2].second.tensor<int, 2>();

  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
  u32string EOS = cvt.from_bytes("<EOS>");
  size_t encoder_eos = m_lemmatizer_conf.encoder_dict.m_w2i.find(EOS)->second;

  const TSentence& sent = sentences[current_sentence];
  const TToken& token = sent.tokens[current_token];

  const LimaString& form = (*m_stringsPool)[token.token->form()];
  const u32string u32_form_lc = form.toStdU32String();

  // len
  int seq_length = u32_form_lc.size();
  if (seq_length > input.dimension(1))
    seq_length = input.dimension(1);

  len(batch_item_idx) = seq_length;

  // input
  for (int n = 0; (size_t)n < u32_form_lc.size() && n < seq_length; n++)
  {
    u32string current_symbol = u32_form_lc.substr(n, 1);
    auto it = m_lemmatizer_conf.encoder_dict.m_w2i.find(current_symbol);
    size_t code = encoder_eos;
    if (it != m_lemmatizer_conf.encoder_dict.m_w2i.end())
    {
      code = it->second;
      input(batch_item_idx, n) = code;
    }
  }

  // context
  size_t left_context_size = 0;
  size_t right_context_size = 0;
  for (size_t n = 1; n <= left_context_size; n++)
  {
    if (current_token < n)
      continue;

    context(batch_item_idx, n - 1) = get_code_for_feature(sent, current_token - n, m_lemmatizer_conf.pos_dict);
  }

  size_t code = get_code_for_feature(sent, current_token, m_lemmatizer_conf.pos_dict);
  context(batch_item_idx, left_context_size) = code;

  for (size_t n = 1; n <= right_context_size; n++)
  {
    if (sent.token_count - 1 < current_token + n)
      continue;

    context(batch_item_idx, left_context_size + n) = get_code_for_feature(sent, current_token + n, m_lemmatizer_conf.pos_dict);
  }

  // features
  for (const auto& kv : m_lemmatizer_conf.feature_dicts)
  {
    string name = string("feat_") + kv.first;
    auto feat_it = feat2idx.find(name);
    if (feat2idx.end() == feat_it)
      LOG_ERROR_AND_THROW("ERROR: something wrong in encode_token_for_lemmatizer_batch.", LimaException());
    auto t = batch[feat_it->second].second.tensor<int, 1>();

    if ("FirstWord" == kv.first)
    {
      if (0 == current_token)
      {
        t(batch_item_idx) = 0;
      }
      else
      {
        t(batch_item_idx) = 0;
      }
      continue;
    }

    auto it = m_lemmatizer_conf.feature_dicts.find(kv.first);
    const LemmatizerConf::dict<string> &rd = it->second;
    if (rd.accessor == nullptr)
    {
      continue;
    }
    size_t code = get_code_for_feature(sent, current_token, rd);
    t(batch_item_idx) = code;
  }
}

void TensorFlowLemmatizerPrivate::encode_token_for_batch(const TFormOccurrences* form_occurrences,
                                       const map<string, size_t> &feat2idx,
                                       size_t batch_item_idx,
                                       vector<pair<string, Tensor>>& batch) const
{
    auto len = batch[0].second.tensor<int, 1>();
    auto input = batch[1].second.tensor<int, 2>();
    auto context = batch[2].second.tensor<int, 2>();

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    u32string EOS = cvt.from_bytes("<EOS>");
    size_t encoder_eos = m_lemmatizer_conf.encoder_dict.m_w2i.find(EOS)->second;

    const LimaString& form = (*m_stringsPool)[form_occurrences->form];
    const u32string u32_form_lc = form.toStdU32String();

    // len
    int seq_length = u32_form_lc.size();
    if (seq_length > input.dimension(1))
      seq_length = input.dimension(1);

    len(batch_item_idx) = seq_length;

    // input
    for (int n = 0; (size_t)n < u32_form_lc.size() && n < seq_length; n++)
    {
      u32string current_symbol = u32_form_lc.substr(n, 1);
      auto it = m_lemmatizer_conf.encoder_dict.m_w2i.find(current_symbol);
      size_t code = encoder_eos;
      if (it != m_lemmatizer_conf.encoder_dict.m_w2i.end())
      {
        code = it->second;
        input(batch_item_idx, n) = code;
      }
    }

    // context
    size_t code = get_code_for_feature(**(form_occurrences->occurrences.begin()), m_lemmatizer_conf.pos_dict);
    context(batch_item_idx, 0) = code;

    // features
    for (const auto& kv : m_lemmatizer_conf.feature_dicts)
    {
      string name = string("feat_") + kv.first;
      auto feat_it = feat2idx.find(name);
      if (feat2idx.end() == feat_it)
        LOG_ERROR_AND_THROW("ERROR: something wrong in encode_token_for_lemmatizer_batch.", LimaException());
      auto t = batch[feat_it->second].second.tensor<int, 1>();

      if ("FirstWord" == kv.first)
      {
        if (form_occurrences->firstWord)
          t(batch_item_idx) = 1;
        else
          t(batch_item_idx) = 0;
        continue;
      }

      auto it = m_lemmatizer_conf.feature_dicts.find(kv.first);
      const LemmatizerConf::dict<string> &rd = it->second;
      if (rd.accessor == nullptr)
      {
        continue;
      }
      size_t code = get_code_for_feature(**(form_occurrences->occurrences.begin()), rd);
      t(batch_item_idx) = code;
    }
}

void TensorFlowLemmatizerPrivate::create_batch_template(vector<pair<string, Tensor>>& batch,
                                                                     map<string, size_t>& feat2idx) const
{
  // len - input word length
  Tensor t_len(DT_INT32, TensorShape({static_cast<long long>(m_batch_size)}));

  batch.push_back({ m_input_node_names.find("length")->second, t_len });

  // input - char indices
  // batch_size x max_input_len
  Tensor t_input(DT_INT32, TensorShape({static_cast<long long>(m_batch_size),
                                        static_cast<long long>(m_max_input_len)
                                       }));

  batch.push_back({ m_input_node_names.find("input")->second, t_input });

  // context - pos indices
  // batch_size x ctx_len
  Tensor t_context(DT_INT32, TensorShape({static_cast<long long>(m_batch_size),
                                          static_cast<long long>(m_ctx_len)
                                         }));

  batch.push_back({ m_input_node_names.find("context")->second, t_context });

  for (const auto& kv : m_lemmatizer_conf.feature_dicts)
  {
    Tensor t_feat(DT_INT32, TensorShape({static_cast<long long>(m_batch_size)}));
    string name = string("feat_") + kv.first;
    batch.push_back({ m_input_node_names.find(name)->second, t_feat });
    feat2idx[name] = batch.size() - 1;
    auto t = batch[batch.size() - 1].second.tensor<int, 1>();
    init_1d_tensor(t, 0);
  }
}

void TensorFlowLemmatizerPrivate::generate_batch(const vector<TSentence>& sentences,
                                                 const vector<pair<size_t, size_t>>& tokens_to_lemmatize,
                                                 vector<pair<string, Tensor>>& batch) const
{
  map<string, size_t> feat2idx;
  create_batch_template(batch, feat2idx);

  auto len = batch[0].second.tensor<int, 1>();
  auto input = batch[1].second.tensor<int, 2>();
  auto context = batch[2].second.tensor<int, 2>();

  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;

  u32string EOS = cvt.from_bytes("<EOS>");
  string PAD = "<PAD>";
  size_t encoder_eos = m_lemmatizer_conf.encoder_dict.m_w2i.find(EOS)->second;
  size_t encoder_pad = m_lemmatizer_conf.pos_dict.m_w2i.find(PAD)->second;

  init_1d_tensor(len, 1);
  init_2d_tensor(input, encoder_eos);
  init_2d_tensor(context, encoder_pad);

  for (size_t i = 0; i < m_batch_size && i < tokens_to_lemmatize.size(); ++i)
  {
    encode_token_for_batch(sentences, tokens_to_lemmatize[i].first, tokens_to_lemmatize[i].second, feat2idx, i, batch);
  }
}

void TensorFlowLemmatizerPrivate::generate_batch(const vector<TFormOccurrences*>& forms_to_lemmatize,
                                                 vector<pair<string, Tensor>>& batch) const
{
  map<string, size_t> feat2idx;
  create_batch_template(batch, feat2idx);

  auto len = batch[0].second.tensor<int, 1>();
  auto input = batch[1].second.tensor<int, 2>();
  auto context = batch[2].second.tensor<int, 2>();

  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;

  u32string EOS = cvt.from_bytes("<EOS>");
  string PAD = "<PAD>";
  size_t encoder_eos = m_lemmatizer_conf.encoder_dict.m_w2i.find(EOS)->second;
  size_t encoder_pad = m_lemmatizer_conf.pos_dict.m_w2i.find(PAD)->second;

  init_1d_tensor(len, 1);
  init_2d_tensor(input, encoder_eos);
  init_2d_tensor(context, encoder_pad);

  for (size_t i = 0; i < m_batch_size && i < forms_to_lemmatize.size(); ++i)
  {
    if (nullptr != forms_to_lemmatize[i])
    {
      encode_token_for_batch(forms_to_lemmatize[i], feat2idx, i, batch);
    }
  }
}

void TensorFlowLemmatizerPrivate::generate_batch(const vector<TSentence>& sentences,
                                                 size_t& current_sentence,
                                                 size_t& current_token,
                                                 vector<pair<string, Tensor>>& batch) const
{
  map<string, size_t> feat2idx;
  create_batch_template(batch, feat2idx);

  auto len = batch[0].second.tensor<int, 1>();
  auto input = batch[1].second.tensor<int, 2>();
  auto context = batch[2].second.tensor<int, 2>();

  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;

  u32string EOS = cvt.from_bytes("<EOS>");
  string PAD = "<PAD>";
  size_t encoder_eos = m_lemmatizer_conf.encoder_dict.m_w2i.find(EOS)->second;
  size_t encoder_pad = m_lemmatizer_conf.pos_dict.m_w2i.find(PAD)->second;

  init_1d_tensor(len, 1);
  init_2d_tensor(input, encoder_eos);
  init_2d_tensor(context, encoder_pad);

  for (size_t i = 0; i < m_batch_size; ++i)
  {
    if (current_token >= sentences[current_sentence].token_count)
    {
      current_sentence += 1;
      current_token = 0;
    }

    if (current_sentence >= sentences.size())
    {
      break;
    }

    encode_token_for_batch(sentences, current_sentence, current_token, feat2idx, i, batch);

    current_token++;
  }
}

void TensorFlowLemmatizerPrivate::load_graph(const QString& model_path, GraphDef* graph_def)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "TensorFlowLemmatizerPrivate::load_graph" << model_path);

  Status status = ReadBinaryProto(Env::Default(),
                                  model_path.toStdString(),
                                  graph_def);
  if (!status.ok())
    LOG_ERROR_AND_THROW("TensorFlowLemmatizerPrivate::load_graph error reading binary proto:"
                        << status.ToString(),
                        LimaException());
}

void TensorFlowLemmatizerPrivate::load_cache(const QString& file_name)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "TensorFlowLemmatizerPrivate::load_cache" << file_name)

  QFile file(file_name);

  if (!file.open(QIODevice::ReadOnly))
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_cache can't load cache from "
                        << file_name << ".", LimaException());

  QTextStream in(&file);
  while (!in.atEnd())
  {
    QString line = in.readLine();
    QStringList fields = line.split('\t');
    if (fields.size() != 3)
      LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_cache can't parse line "
                          << line << " from file " << file_name << ".",
                          LimaException());

    if (fields[2].size() == 0)
    {
      LOG_MESSAGE(LDEBUG, "TensorFlowLemmatizer::load_cache invalid cache record: "
                  << line << " from file " << file_name << ".");
      continue;
    }

    QStringList features = fields[1].split('|');
    map<string, string> features_map;
    for (int i = 0; i < features.size(); i++)
    {
      QStringList kv = features[i].split("=");
      if (i > 0 && kv.size() != 2)
        LOG_ERROR_AND_THROW("TensorFlowLemmatizer::load_cache can't parse line "
                            << line << " from file " << file_name << ".",
                            LimaException());

      if (0 == i && 1 == kv.size())
        features_map["upos"] = features[i].toStdString();
      else
        features_map[kv[0].toStdString()] = kv[1].toStdString();
    }

    LimaString form_key = create_form_key(fields[0], features_map);
    m_cache.put(form_key, fields[2]);
  }

  file.close();
}

void TensorFlowLemmatizerPrivate::save_cache(const QString& file_name)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "TensorFlowLemmatizerPrivate::save_cache" << file_name)

  QFile file(file_name);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    LOG_ERROR_AND_THROW("TensorFlowLemmatizer::save_cache can't save cache from "
                        << file_name << ".",   LimaException());

  QTextStream out(&file);
  out.setCodec("UTF-8");

  vector<LimaString> cache_keys;
  cache_keys.reserve(m_cache.size());
  m_cache.get_keys(cache_keys);

  sort(cache_keys.begin(), cache_keys.end()/*, [](const LimaString& a, const LimaString& b) {
    return a < b;
  }*/);

  for ( const auto& k : cache_keys )
  {
    LimaString v;
    m_cache.get(k, v);
    out << k << "\t" << v << endl;
  }

  file.close();
}

} // Lemmatizer
} // TensorFlowUnits
} // LinguisticProcessing
} // Lima
