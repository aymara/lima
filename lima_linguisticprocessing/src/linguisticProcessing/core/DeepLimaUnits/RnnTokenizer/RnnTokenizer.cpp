/*
    Copyright 2002-2021 CEA LIST

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
#include "linguisticProcessing/common/helpers/DeepTokenizerBase.h"
#include "linguisticProcessing/common/helpers/LangCodeHelpers.h"

#include "RnnTokenizer.h"

#include "deeplima/segmentation.h"

#include <chrono>
#include <thread>


#define DEBUG_THIS_FILE true

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

using namespace deeplima;

namespace Lima
{
namespace LinguisticProcessing
{
namespace DeepLimaUnits
{
namespace RnnTokenizer
{

static u32string SPACE = QString::fromUtf8(" ").toStdU32String();

static SimpleFactory<MediaProcessUnit, RnnTokenizer> rnntokenizerFactory(RNNTOKENIZER_CLASSID); // clazy:exclude=non-pod-global-static

#if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
  #define LOG_MESSAGE(stream, msg) stream << msg;
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) TOKENIZERLOGINIT; LOG_MESSAGE(stream, msg);
#else
  #define LOG_MESSAGE(stream, msg) ;
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif

CONFIGURATIONHELPER_LOGGING_INIT(TOKENIZERLOGINIT);

class RnnTokenizerPrivate : public DeepTokenizerBase, public ConfigurationHelper
{
public:
  RnnTokenizerPrivate();
  virtual ~RnnTokenizerPrivate();

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
  void append_new_word(vector< TPrimitiveToken >& current_sentence,
                       const QString& current_token,
                       int current_token_offset) const;

  size_t m_max_seq_len;

  map<QString, vector<QString>> m_trrules;

  // Parameters
  bool m_ignoreEOL;

  segmentation::impl::SegmentationModuleUtf8 m_segm;

  function<void()> m_load_fn;
  bool m_loaded;
};

RnnTokenizerPrivate::RnnTokenizerPrivate() :
  ConfigurationHelper("RnnTokenizerPrivate", THIS_FILE_LOGGING_CATEGORY()),
  m_stringsPool(nullptr),
  m_currentVx(0),
  m_ignoreEOL(false),
  m_loaded(false)
{
}

RnnTokenizerPrivate::~RnnTokenizerPrivate()
{
}

RnnTokenizer::RnnTokenizer() :
  m_d(new RnnTokenizerPrivate())
{
}

RnnTokenizer::~RnnTokenizer()
{
  delete m_d;
}

void RnnTokenizer::init(
  GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnTokenizer::init");

  m_d->m_language = manager->getInitializationParameters().media;
  m_d->m_stringsPool = &MediaticData::changeable().stringsPool(m_d->m_language);

  m_d->init(unitConfiguration);
}

LimaStatusCode RnnTokenizer::process(AnalysisContent& analysis) const
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "start tokenizer process");
  TimeUtilsController RnnTokenizerProcessTime("RnnTokenizer");

  auto anagraph = new AnalysisGraph("AnalysisGraph",m_d->m_language,true,true);
  analysis.setData("AnalysisGraph",anagraph);
  LinguisticGraph* graph=anagraph->getGraph();
  m_d->m_currentVx = anagraph->firstVertex();
  // Get text from analysis
  LimaStringText* originalText=static_cast<LimaStringText*>(analysis.getData("Text"));

  // Execute model on the text
  vector< vector< RnnTokenizerPrivate::TPrimitiveToken > > sentencesTokens;
  m_d->tokenize(*originalText, sentencesTokens);
    LOG_MESSAGE(LDEBUG, "      Number of token '" << sentencesTokens.size() << "'");

  // Insert the tokens in the graph and create sentence limits
  SegmentationData* sb = new SegmentationData("AnalysisGraph");
  analysis.setData(m_d->m_data.toStdString(), sb);
git
  remove_edge(anagraph->firstVertex(),
              anagraph->lastVertex(),
              *graph);
  LinguisticGraphVertex beginSentence = 0;

  // Insert the tokens in the graph and create sentence limits
  for (const auto& sentence: sentencesTokens)
  {
    if (sentence.size() < 1)
    {
      continue;
    }

    LinguisticGraphVertex endSentence = numeric_limits< LinguisticGraphVertex >::max();
    for (const auto& token: sentence)
    {
      const auto& str = token.wordText;

      LOG_MESSAGE(LDEBUG, "      Adding token '" << str << "'");

      StringsPoolIndex form=(*m_d->m_stringsPool)[str];
      Token *tToken = new Token(form, str, token.start+1, token.wordText.size());
      if (tToken == nullptr)
      {
        TOKENIZERLOGINIT;
        LERROR << "RnnFlowTokenizer::process: Can't allocate memory with \"new Token(...)\"";
        throw MemoryErrorException();
      }

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

void RnnTokenizerPrivate::init(GroupConfigurationStructure& unitConfiguration)
{
  m_data = QString(getStringParameter(unitConfiguration, "data", 0, "SentenceBoundaries").c_str());
  QString model_prefix = getStringParameter(unitConfiguration, "model_prefix", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY).c_str();

  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnTokenizerPrivate::init" << model_prefix);

  QString lang_str = MediaticData::single().media(m_language).c_str();
  QString resources_path = MediaticData::single().getResourcesPath().c_str();
  QString model_name = model_prefix;
  string udlang;
  MediaticData::single().getOptionValue("udlang", udlang);

  if (!fix_lang_codes(lang_str, udlang))
  {
    LIMA_EXCEPTION_SELECT_LOGINIT(TOKENIZERLOGINIT,
      "RnnTokenizerPrivate::init: Can't parse language id " << udlang.c_str(),
      Lima::InvalidConfiguration);
  }

  model_name.replace(QString("$udlang"), QString(udlang.c_str()));

  auto model_file_name = findFileInPaths(resources_path,
                                         QString::fromUtf8("/RnnTokenizer/%1/%2.pt")
                                           .arg(lang_str).arg(model_name));
  if (model_file_name.isEmpty())
  {
    throw InvalidConfiguration("RnnTokenizerPrivate::init: tokenizer model file not found.");
  }

  m_load_fn = [this, model_file_name]()
  {
    if (m_loaded)
    {
      return;
    }

    m_segm.load(model_file_name.toStdString());
    m_segm.init(1, 16*1024); // threads, buffer size per thread

    m_loaded = true;
  };

  if (!isInitLazy())
  {
    m_load_fn();
  }
}

void RnnTokenizerPrivate::append_new_word(vector< TPrimitiveToken >& current_sentence,
                                          const QString& current_token,
                                          int current_token_offset) const
{
  QString ctoken_lower = current_token.toLower();

  map<QString, vector<QString>>::const_iterator i = m_trrules.find(ctoken_lower);
  if (i == m_trrules.end())
  {
    current_sentence.push_back(TPrimitiveToken(current_token, current_token_offset));
  }
  else
  {
    size_t n = 0;
    for (const QString& w : i->second)
    {
      if (n == 0)
      {
        current_sentence.push_back(TPrimitiveToken(w,
                                                   current_token_offset,
                                                   current_token));
      }
      else
      {
        current_sentence.push_back(TPrimitiveToken(w, current_token_offset));
      }
    }
  }
}

void RnnTokenizerPrivate::tokenize(const QString& text, vector<vector<TPrimitiveToken>>& sentences)
{
  m_load_fn();

  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnTokenizerPrivate::tokenize" << text.left(100));
  TimeUtils::updateCurrentTime();
  sentences.clear();
  sentences.reserve(text.size() / 15);

  vector< TPrimitiveToken > current_sentence;
  int current_token_offset = 0;

  string text_utf8 = text.toStdString();

  m_segm.register_handler([this, &sentences, &current_sentence, &current_token_offset]
                          (const vector<segmentation::token_pos>& tokens,
                           uint32_t len)
  {
    for (size_t i = 0; i < len; i++)
    {
      const segmentation::token_pos& tok = tokens[i];
      if (0 == tok.m_len)
      {
        continue;
      }
      append_new_word(current_sentence, QString::fromUtf8(tok.m_pch, tok.m_len), current_token_offset);
      current_token_offset += (tok.m_offset + tok.m_len);
      if (tok.m_flags & segmentation::token_pos::flag_t::sentence_brk || tok.m_len == strlen(tok.m_pch)-1)
      {
        sentences.push_back(current_sentence);
        current_sentence.clear();
      }
    }
  });

  size_t bytes_consumed = 0;
  m_segm.parse_from_stream([&text_utf8, &bytes_consumed]
                         (uint8_t* buffer,
                         uint32_t& read,
                         uint32_t max)
  {
    read = (text_utf8.size() - bytes_consumed) > max ? max : (text_utf8.size() - bytes_consumed);
    memcpy(buffer, text_utf8.c_str() + bytes_consumed, read);
    bytes_consumed += read;
    return (text_utf8.size() - bytes_consumed) > max;
  });

  LOG_MESSAGE(LINFO, "End of Tokenizer");
  TimeUtils::logElapsedTime("RnnTokenizerPrivate");
}

} // namespace RnnTokenizer
} // namespace DeepLimaUnits
} // namespace LinguisticProcessing
} // namespace Lima
