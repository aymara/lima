/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "analysisthread.h"

//#include "linguisticProcessing/core/AnalysisDumper/JsonResults.h"

#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
 #include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"

// factories
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"

#include <qhttprequest.h>
#include <qhttpresponse.h>

#include <QStringList>
#include <QPair>
#include <boost/graph/buffer_concepts.hpp>

#include <fstream>
#include <sstream>      // std::stringstream
#include <QtCore/QString>
#include <QtCore/QTemporaryFile>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#   include <QUrlQuery>
#endif
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;


class AnalysisThreadPrivate
{
friend class AnalysisThread;
public:
  AnalysisThreadPrivate (Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* m_analyzer,
                        QHttpRequest *req, QHttpResponse *resp,
                        const std::set<std::string>& langs);
  ~AnalysisThreadPrivate() {}

  Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* m_analyzer;
  QHttpRequest* m_request;
  // QHttpResponse* m_response;
  const std::set<std::string>& m_langs;
  std::string m_mediaType;
  int m_response_code;
  std::map<QString,QString> m_response_header;
  QByteArray m_response_body;
};

AnalysisThreadPrivate::AnalysisThreadPrivate(Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* analyzer,
                    QHttpRequest *req, QHttpResponse *resp,
                    const std::set<std::string>& langs) :
    m_analyzer(analyzer),
    m_request(req),
    // m_response(resp),
    m_langs(langs),
    m_mediaType((req->header("content-type")).toStdString())
{
  LIMA_UNUSED(resp);
}

AnalysisThread::AnalysisThread (Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* analyzer,
                  QHttpRequest *req, QHttpResponse *resp,
                  const std::set<std::string>& langs, QObject* parent ):
#ifdef MULTITHREAD
    QThread(parent),
#else
    QObject(parent),
#endif
    m_d(new AnalysisThreadPrivate(analyzer,req,resp,langs))
{
  LIMASERVERLOGINIT;
  LDEBUG << "AnalysisThread::AnalysisThread()...";
}

AnalysisThread::~AnalysisThread()
{
  LIMASERVERLOGINIT;
  LDEBUG << "AnalysisThread::~AnalysisThread...";
  delete m_d;
}

const QByteArray& AnalysisThread::response_body() const {
  return m_d->m_response_body;
}

int AnalysisThread::response_code() const {
  return m_d->m_response_code;
}

const std::map<QString,QString>& AnalysisThread::response_header() const {
  return m_d->m_response_header;
}

/*
void AnalysisThread::slotStarted()
{
  LIMASERVERLOGINIT;
  LDEBUG << "AnalysisThread::slotStarted";

}
*/

void AnalysisThread::run ()
{
  startAnalysis();
    // m_d->m_request->deleteLater();
    // deleteLater();
    exit(0); // exit the eventLoop
}


void AnalysisThread::startAnalysis()
{
  LIMASERVERLOGINIT;
  LDEBUG << "AnalysisThread::startAnalysis" << m_d->m_request->methodString()
          << m_d->m_request->url().path();
  LDEBUG << "AnalysisThread::startAnalysis mediaType=" << m_d->m_mediaType;
  int exitStatus=0;
  if( m_d->m_request->url().path() != "/extractEN")
  {
    LWARN << "AnalysisThread::startAnalysis: entry point "
          << m_d->m_request->url().path() << " not recognized";
    m_d->m_response_code = 400;
    m_d->m_response_header.insert(std::pair<QString,QString>("Allow","extractEN"));
    m_d->m_response_body=QByteArray("Only extractEN entry point accepted.");
    exitStatus=1;
  }
  if( (m_d->m_request->methodString() != "HTTP_GET")
    && (m_d->m_request->methodString() != "HTTP_POST") )
  {
    LWARN << "AnalysisThread::startAnalysis: methodString "
          << m_d->m_request->methodString() << " not recognized";
    m_d->m_response_code = 405;
    m_d->m_response_header.insert(std::pair<QString,QString>("Allow","GET,POST"));
    m_d->m_response_body=QByteArray("Only GET and POST methods currently allowed.");
    exitStatus=1;
  }

  QString language;
  QString pipeline ;
  QString textQS;
  std::string text_s;
  std::map<std::string,std::string> metaData;
  std::set<std::string> inactiveUnits;

  if(m_d->m_request->methodString() == "HTTP_GET")
  {
    LDEBUG << "AnalysisThread::startAnalysis: process extractEN request (mode HTTP_GET)";
  }
  else if(m_d->m_request->methodString() == "HTTP_POST")
  {
    LDEBUG << "AnalysisThread::startAnalysis: process extractEN request (mode HTTP_POST)";
  }

  for (const auto& item :
    QUrlQuery(m_d->m_request->url()).queryItems(QUrl::FullyDecoded))
  {
    QTemporaryFile tempFile;
    metaData["FileName"] = tempFile.fileName().toStdString();
    if (item.first == "lang")
    {
      language = item.second;
      metaData["Lang"] = language.toStdString();
      LDEBUG << "AnalysisThread::startAnalysis: language=" << language;
    }
    if (item.first == "pipeline")
    {
      pipeline = item.second;
      LDEBUG << "AnalysisThread::startAnalysis: pipeline=" << pipeline;
    }
    if (item.first == "meta")
    {
      auto metadataValues = item.second.split(",");
      LDEBUG << "AnalysisThread::startAnalysis: meta=" << metadataValues;
      for (const auto& metadataValue: metadataValues)
      {
        auto metaDataKeyValue = metadataValue.split(":");
        if (metaDataKeyValue.size() != 2)
        {
          LIMASERVERLOGINIT;
          LERROR << "meta argument '"<< metadataValue
                  << "' is not of the form XXX:YYY: ignored";
        }
        else
        {
          metaData.insert(std::make_pair(metaDataKeyValue[0].toStdString(),
                                         metaDataKeyValue[1].toStdString()));
        }
      }

    }
    if( (item.first == "text") & (m_d->m_request->methodString() == "HTTP_GET") )
    {
      // Get text from parameter in GET mode
      textQS = item.second;
    }
  }
  if(m_d->m_request->methodString() == "HTTP_POST")
  {
    // Get text from body in POST mode
    const auto& body = m_d->m_request->body();
    text_s = std::string(body.data());
    textQS = QString::fromUtf8(text_s.c_str());
  }
  auto truncated = textQS.mid(0,20);
  LDEBUG << "AnalysisThread::startAnalysis: " << "text='" << truncated << "...'";

  if( language.isEmpty() )
  {
    m_d->m_response_code = 400;
    m_d->m_response_body = QByteArray("Empty language");
    exitStatus=1;
  }
  else if( m_d->m_langs.find(metaData["Lang"]) == m_d->m_langs.end() )
  {
    m_d->m_response_code = 400;
    auto errorMessage = QString(tr("Language '%1' is not initialized")).arg(language);
    m_d->m_response_body = errorMessage.toUtf8();
    exitStatus=1;
  }
  else if( pipeline.isEmpty() )
  {
    m_d->m_response_code = 400;
    m_d->m_response_body = QByteArray("Empty pipeline");
    exitStatus=1;
  }
  else if( textQS.isEmpty() )
  {
    m_d->m_response_code = 400;
    m_d->m_response_body = QByteArray("Empty text");
    exitStatus=1;
  }

  if( exitStatus != 0 )
  {
    exit(exitStatus); // exit the eventLoop
    return;
  }
  else
  {
    std::map<std::string, AbstractAnalysisHandler*> handlers;
    auto seLogWriter = new LinguisticProcessing::SimpleStreamHandler();
    handlers.insert(std::make_pair("se", seLogWriter));
    auto simpleStreamHandler = new LinguisticProcessing::SimpleStreamHandler();
    handlers.insert(std::make_pair("simpleStreamHandler", simpleStreamHandler));

    auto oss = new std::ostringstream();
    seLogWriter->setOut(oss);
    simpleStreamHandler->setOut(oss);

    LDEBUG << "Analyzing" << language << textQS.mid(0,20) << "...";
    // std::ostringstream ots;

    QTemporaryFile tempFile;
    metaData["FileName"] = tempFile.fileName().toStdString();
    std::string pipe = pipeline.toStdString();

    m_d->m_analyzer->analyze(textQS,metaData, pipe, handlers, inactiveUnits);

    std::string resultString;
    if( m_d->m_mediaType.compare("text/xml") == 0)
    {
      resultString.append("<?xml version='1.0' encoding='UTF-8'?>");
      resultString.append(oss->str());
    }
    else if( m_d->m_mediaType.compare("application/json") == 0)
    {
      resultString.append(ConllToJson( oss->str()));
    }
    else
    {
      resultString.append(ConllToJson( oss->str()));
    }

    LDEBUG << "AnalysisThread::startAnalysis: seLogger output is "
            << QString::fromUtf8((std::string(resultString,0, 30)).c_str());

    if( m_d->m_mediaType.compare("text/xml") == 0)
    {
      m_d->m_response_header.insert(
        std::pair<QString,QString>("Content-Type","text/xml; charset=utf-8"));
    }
    if( m_d->m_mediaType.compare("application/json") == 0)
    {
      m_d->m_response_header.insert(
        std::pair<QString,QString>("Content-Type",
                                   "application/json; charset=utf-8"));
    }
    m_d->m_response_code = 200;

    m_d->m_response_body = QByteArray(resultString.c_str());
    LDEBUG << "AnalysisThread::startAnalysis: delete oss... ";
    delete seLogWriter;
    delete oss; oss = 0;
  }
#ifdef MULTITHREAD
  // m_d->m_request->deleteLater();
  // deleteLater();
  // quit(); // exit the eventLoop
  exit(exitStatus); // exit the eventLoop
#else
#endif
}

std::string  AnalysisThread::ConllToJson( const std::string & str )
{
  LIMASERVERLOGINIT;
  // LDEBUG << "AnalysisThread::ConllToJson str=" << std::string(str,0, 30) << "...";
  LDEBUG << "AnalysisThread::ConllToJson str=" << str;

  // Array of tokens
  QJsonArray array;
  std::string::size_type pos = 0;
  std::string::size_type i=0;
  for(; i != std::string::npos ;)
  {
    // search for EOL: each line represents a token (or end of sentence for an empty line)
    i = str.find('\n', pos);
    // LDEBUG << "AnalysisThread::ConllToJson: pos:" << pos << ", i:"<< i;
    QJsonValue qval;
    if( pos == i )
    {
      // empty line = end of sentence
      qval = QJsonValue("");
    }
    else
    {
      std::string line;
      if( i == std::string::npos )
      {
        line = std::string(str,pos);
      }
      else
      {
        line = std::string(str,pos,i-pos);
      }
      qval = QJsonValue(line.c_str());
    }
    array.append(qval);
    pos = i+1;
  }
  QJsonObject object;
  object["tokens"] = array;
  QJsonDocument doc(object);
  std::string result(doc.toJson().data());

  std::string truncated = std::string(result,0, 80);
  LDEBUG << "AnalysisThread::ConllToJson: result=" << truncated << "...";
  return result;
}
