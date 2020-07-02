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

class ConlluReaderPrivate
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

  void init(const QString& model_refix);
  vector< vector< TPrimitiveToken > > tokenize(const QString& text);

  MediaId m_language;
  FsaStringsPool* m_stringsPool;
  LinguisticGraphVertex m_currentVx;
  QString m_data;

protected:
  void append_new_word(vector< TPrimitiveToken >& current_sentence,
                       const u32string& current_token,
                       int current_token_offset) const;

  // Parameters
  bool m_ignoreEOL;
};

ConlluReaderPrivate::ConlluReaderPrivate() :
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

  m_d->m_language = manager->getInitializationParameters().media;
  m_d->m_stringsPool = &MediaticData::changeable().stringsPool(m_d->m_language);

  try
  {
    m_d->m_data = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("data").c_str());
  }
  catch (NoSuchParam& )
  {
    m_d->m_data = QString::fromUtf8("SentenceBoundaries");
  }
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

void ConlluReaderPrivate::init(const QString& model_prefix)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "ConlluReaderPrivate::init" << model_prefix);
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
  for (size_t i = 0; i < list.size(); i++)
  {
    const QString line = list[i];
    QStringList fields = line.split(QString("\t"));

    if (fields.size() < 2)
    {
      if (current_sentence.size() > 0)
        sentences.push_back(current_sentence);
      current_sentence.clear();
      continue;
    }

    if (-1 != fields[0].indexOf("-") || -1 != fields[0].indexOf("."))
      continue;

    current_sentence.push_back(TPrimitiveToken(fields[1], current_token_offset));
    current_token_offset += fields[1].size() + 1;
  }

  if (current_sentence.size() > 0)
    sentences.push_back(current_sentence);

  return sentences;
}

} // namespace ConlluReader
} // namespace LinguisticProcessing
} // namespace Lima
