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
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "linguisticProcessing/common/helpers/ConfigurationHelper.h"

#include "ConlluReader.h"


#define DEBUG_THIS_FILE true

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;


namespace Lima
{
namespace LinguisticProcessing
{
namespace ConlluReader
{

namespace
{
inline string THIS_FILE_LOGGING_CATEGORY()
{
  TOKENIZERLOGINIT;
  return logger.zone().toStdString();
}
}

static u32string SPACE = QString::fromUtf8(" ").toStdU32String();

static SimpleFactory<MediaProcessUnit,ConlluReader> conlluReaderFactory(CONLLUREADER_CLASSID); // clazy:exclude=non-pod-global-static

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

class ConlluReaderPrivate : public ConfigurationHelper
{
public:
  ConlluReaderPrivate();
  virtual ~ConlluReaderPrivate();

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

  void init(MediaId language, GroupConfigurationStructure& unitConfiguration);
  vector< vector< TPrimitiveToken > > tokenize(const QString& text);
  void computeDefaultStatus(Token& token);

  MediaId m_language;
  FsaStringsPool* m_stringsPool;
  LinguisticGraphVertex m_currentVx;
  std::string m_data;
  LinguisticCode m_boundaryMicro;

protected:
  void append_new_word(vector< TPrimitiveToken >& current_sentence,
                       const u32string& current_token,
                       int current_token_offset) const;

  // Parameters
  bool m_ignoreEOL;
};

ConlluReaderPrivate::ConlluReaderPrivate() :
  ConfigurationHelper("ConlluReader", THIS_FILE_LOGGING_CATEGORY()),
  m_stringsPool(nullptr),
  m_currentVx(0),
  m_ignoreEOL(false)
{
}

ConlluReaderPrivate::~ConlluReaderPrivate()
{
}

ConlluReader::ConlluReader() :
  m_d(new ConlluReaderPrivate())
{
}

ConlluReader::~ConlluReader()
{
  delete m_d;
}

void ConlluReader::init(GroupConfigurationStructure& unitConfiguration,
                        Manager* manager)

{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "ConlluReader::init");
  m_d->init(manager->getInitializationParameters().media, unitConfiguration);
}

LimaStatusCode ConlluReader::process(AnalysisContent& analysis) const
{
  LOG_MESSAGE_WITH_PROLOG(LINFO, "start ConlluReader");
  TimeUtilsController ConlluReaderProcessTime("ConlluReader");

  auto anagraph = new AnalysisGraph("AnalysisGraph",m_d->m_language,true,true);
  analysis.setData("AnalysisGraph",anagraph);
  LinguisticGraph* graph=anagraph->getGraph();
  m_d->m_currentVx = anagraph->firstVertex();
  // Get text from analysis
  LimaStringText* originalText=static_cast<LimaStringText*>(analysis.getData("Text"));

  // Evaluate TensorFlow model on the text
  auto sentencesTokens = m_d->tokenize(*originalText);

  // Insert the tokens in the graph and create sentence limits
  SegmentationData* sb = nullptr;
  if (m_d->m_data.size() > 0)
  {
    sb = new SegmentationData("AnalysisGraph");
    analysis.setData(m_d->m_data, sb);
  }

  auto microAccessor = &(GET_PROPERTY_ACCESSOR(m_d->m_language, "MICRO"));

  remove_edge(anagraph->firstVertex(),
              anagraph->lastVertex(),
              *graph);
  LinguisticGraphVertex beginSentence = anagraph->firstVertex();

  // Insert the tokens in the graph and create sentence limits
  for (const auto& sentence: sentencesTokens)
  {
    if (sentence.size() < 1)
      continue;

    LinguisticGraphVertex endSentence = anagraph->lastVertex();
    for (const auto& token: sentence)
    {
      const auto& str = token.wordText;

      LOG_MESSAGE(LDEBUG, "      Adding token '" << str << "'");

      StringsPoolIndex form=(*m_d->m_stringsPool)[str];
      Token *tToken = new Token(form, str, token.start, token.wordText.size());
      if (tToken == 0)
        LOG_ERROR_AND_THROW("ConlluReader::process: Can't allocate memory with \"new Token(...)\"",
                            MemoryErrorException());

      /*if (token.originalText.size() > 0)
      {
        // tranduced token
        // save original word as orph alternative
        StringsPoolIndex orig = (*m_d->m_stringsPool)[token.originalText];
        tToken->addOrthographicAlternatives(orig);
      }*/

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

    if (nullptr != sb)
    {
      sb->add(Segment("sentence", beginSentence, endSentence, anagraph));
    }

    Token *tToken = get(vertex_token, *graph, endSentence);
    MorphoSyntacticData *morphoData = get(vertex_data, *graph, endSentence);
    if (nullptr != morphoData)
    {
      LinguisticElement elem;
      elem.inflectedForm = tToken->form();
      elem.lemma = tToken->form();
      elem.normalizedForm = tToken->form();
      elem.type = SIMPLE_WORD;
      microAccessor->writeValue(m_d->m_boundaryMicro, elem.properties);
      morphoData->push_back(elem);
    }
    else
    {
      LOG_MESSAGE(LERROR, "vertex" << endSentence << "has no MorphoSyntacticData");
    }
    beginSentence = endSentence;
  }

  add_edge(m_d->m_currentVx,anagraph->lastVertex(),*graph);

  return SUCCESS_ID;
}

void ConlluReaderPrivate::init(MediaId language, GroupConfigurationStructure& unitConfiguration)
{
  m_language = language;
  m_stringsPool = &MediaticData::changeable().stringsPool(m_language);

  m_data = getStringParameter(unitConfiguration, "data", 0, "SentenceBoundaries");

  try
  {
    string boundaryMicro = getStringParameter(unitConfiguration, "boundaryMicro", 0, "PONCTU_FORTE");

    const auto& microManager = GET_PROPERTY_MANAGER(m_language, "MICRO");
    m_boundaryMicro = microManager.getPropertyValue(boundaryMicro);
    if (m_boundaryMicro == L_NONE)
    {
      LOG_ERROR_AND_THROW("ConlluReaderPrivate::init(): cannot find linguistic code for micro " << boundaryMicro,
                          InvalidConfiguration());
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& )
  {
    throw InvalidConfiguration("ConlluReaderPrivate::init(): can't find boundary micro");
  }
}

// set default key in status according to other elements in status
void ConlluReaderPrivate::computeDefaultStatus(Token& token)
{
  static QRegularExpression reCapital("^[[:upper:]]+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reSmall("^[[:lower:]]+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reCapital1st("^[[:upper:]]\\w+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reAcronym("^([[:upper:]]\\.)+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reCapitalSmall("^([[:upper:][:lower:]])+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reAbbrev("^\\w+\\.$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reTwitter("^[@#]\\w+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reAlphaHyphen("^\\w+[\\-]\\w+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reAlpha("^\\w+$", QRegularExpression::UseUnicodePropertiesOption);

  // t_cardinal_roman
  static QRegularExpression reCardinalRoman("^(?=[MDCLXVI])M*(C[MD]|D?C{0,3})(X[CL]|L?X{0,3})(I[XV]|V?I{0,3})$");
  // t_ordinal_roman
  static QRegularExpression reOrdinalRoman("^(?=[MDCLXVI])M*(C[MD]|D?C{0,3})(X[CL]|L?X{0,3})(I[XV]|V?I{0,3})(st|nd|d|th|er|ème)$");
  // t_integer
  static QRegularExpression reInteger("^\\d+$");
  // t_comma_number
  static QRegularExpression reCommaNumber("^\\d+,\\d$");
  // t_dot_number
  static QRegularExpression reDotNumber("^\\d\\.\\d$");
  // t_fraction
  static QRegularExpression reFraction("^\\d([.,]\\d+)?/\\d([.,]\\d+)?$");
  // t_ordinal_integer
  static QRegularExpression reOrdinalInteger("^\\d+(st|nd|d|th|er|ème)$");
  // t_alphanumeric
  static QRegularExpression reAlphanumeric("^[\\d[:lower:][:upper:]]+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reSentenceBreak("^[;.!?]$");

  TStatus curSettings;
  if (reCapital.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_capital";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_capital"));
  }
  else if (reSmall.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_small";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_small"));
  }
  else if (reCapital1st.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_capital_1st";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_capital_1st"));
  }
  else if (reAcronym.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_acronym";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_acronym"));
  }
  else if (reCapitalSmall.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_capital_small";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_capital_small"));
  }
  else if (reAbbrev.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_abbrev";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_abbrev"));
  }
  else if (reTwitter.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_twitter";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_twitter"));
  }
  else if (reCardinalRoman.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_cardinal_roman";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_cardinal_roman"));
  }
  else if (reOrdinalRoman.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_ordinal_roman";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_ordinal_roman"));
  }
  else if (reInteger.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_integer";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_integer"));
  }
  else if (reCommaNumber.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_comma_number";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_comma_number"));
  }
  else if (reDotNumber.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_dot_number";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_dot_number"));
  }
  else if (reFraction.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_fraction";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_fraction"));
  }
  else if (reOrdinalInteger.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_ordinal_integer";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_ordinal_integer"));
  }
  else if (reAlphanumeric.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_alphanumeric";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_alphanumeric"));
  }
  else if (reAlphaHyphen.match(token.stringForm()).hasMatch())
  {
    curSettings.setDefaultKey(QString::fromUtf8("t_alpha_hyphen"));
    curSettings.setAlphaHyphen(true);
  }
  else if (reSentenceBreak.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_sentence_brk";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_sentence_brk"));
  }
  else // if (reSmall.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_word_brk (default)";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_word_brk"));
  }

  /*
  // t_not_roman
  static QRegularExpression reNotRoman("^$");
  // t_alpha_concat_abbrev
  static QRegularExpression reAlphConcatAbbrev("^$");
  // t_pattern
  static QRegularExpression rePattern("^$");
  // t_word_brk
  static QRegularExpression reWordBreak("^$");
  // t_sentence_brk
  static QRegularExpression reSentenceBreak("^$");
  //  t_paragraph_brk
  static QRegularExpression reParagraphBreak("^$");
  */

  token.setStatus(curSettings);
}


void ConlluReaderPrivate::append_new_word(vector< TPrimitiveToken >& current_sentence,
                                          const u32string& current_token,
                                          int current_token_offset) const
{
  current_sentence.push_back(TPrimitiveToken(QString::fromStdU32String(current_token), current_token_offset));
}

vector< vector< ConlluReaderPrivate::TPrimitiveToken > > ConlluReaderPrivate::tokenize(const QString& text)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "ConlluReaderPrivate::tokenize" << text.left(100));

  vector< vector< TPrimitiveToken > > sentences;
  vector< TPrimitiveToken > current_sentence;
  int current_token_offset = 0;

  QStringList list = text.split(QString("\n"));
  for (size_t i = 0; i < size_t(list.size()); i++)
  {
    const QString line = list[i];
    QStringList fields = line.split(QString("\t"));

    if (fields.size() < 2)
    {
      if (current_sentence.size() > 0)
      {
        sentences.push_back(current_sentence);
      }
      current_sentence.clear();
      continue;
    }

    if (-1 != fields[0].indexOf("-") || -1 != fields[0].indexOf("."))
    {
      continue;
    }

    current_sentence.push_back(TPrimitiveToken(fields[1], current_token_offset));
    current_token_offset += fields[1].size() + 1;
  }

  if (current_sentence.size() > 0)
  {
    sentences.push_back(current_sentence);
  }

  return sentences;
}

} // namespace ConlluReader
} // namespace LinguisticProcessing
} // namespace Lima
