// Copyright (C) <year>  <name of author>
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  AnalysisThreadPrivate(
    Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* m_analyzer,
    QHttpRequest *req, QHttpResponse *resp,
    const std::set<std::string>& langs);
  ~AnalysisThreadPrivate() {}

  QString TokensToJson( const QString& str );

  void initMetaData(const QString& meta, std::map<std::string, std::string>& metaData);

Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* m_analyzer;
  QHttpRequest* m_request;
  // QHttpResponse* m_response;
  const std::set<std::string>& m_langs;
  std::string m_mediaType;
  int m_response_code;
  std::map<QString,QString> m_response_header;
  QByteArray m_response_body;
  const QStringList m_methods = {"/", "/extractEN"};
};

AnalysisThreadPrivate::AnalysisThreadPrivate(
  Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* analyzer,
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

AnalysisThread::AnalysisThread (
  Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* analyzer,
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


void AnalysisThreadPrivate::initMetaData (const QString& meta, std::map<std::string, std::string>& metaData)
{
  // parse 'meta' argument to add metadata
  if (!meta.isEmpty())
  {
    auto metas = meta.split(",");
    for (const auto& aMeta: metas)
    {
      auto kv = aMeta.split(":");
      if (kv.size() != 2)
      {
        std::cerr << "meta argument '"<< aMeta.toStdString() << "' is not of the form XXX:YYY: ignored" << std::endl;
      }
      else
      {
        metaData[kv[0].toStdString()] = kv[1].toStdString();
      }
    }
  }
}

void AnalysisThread::startAnalysis()
{
  LIMASERVERLOGINIT;
  LDEBUG << "AnalysisThread::startAnalysis" << m_d->m_request->methodString() << m_d->m_request->url().path();
  LDEBUG << "AnalysisThread::startAnalysis mediaType=" << m_d->m_mediaType;
  int exitStatus=0;
  if( ! m_d->m_methods.contains( m_d->m_request->url().path() ) )
  {
    LWARN << "AnalysisThread::startAnalysis: entry point " << m_d->m_request->url().path() << " not recognized";
    m_d->m_response_code = 400;
    for (const auto& method: m_d->m_methods)
    {
      m_d->m_response_header.insert(std::pair<QString,QString>("Allow",method));
    }
    m_d->m_response_body=QByteArray("Accepted entry points are: ").append(m_d->m_methods.join(",").toStdString().c_str());
    exitStatus=1;
  }
  if( (m_d->m_request->methodString() != "HTTP_GET") && (m_d->m_request->methodString() != "HTTP_POST") )
  {
    LWARN << "AnalysisThread::startAnalysis: methodString "
          << m_d->m_request->methodString() << " not recognized";
    m_d->m_response_code = 405;
    m_d->m_response_header.insert(std::pair<QString,QString>("Allow","GET,POST"));
    m_d->m_response_body=QByteArray("Only GET and POST methods currently allowed.");
    exitStatus=1;
  }

  if(m_d->m_request->methodString() == "HTTP_GET")
  {
    LDEBUG << "AnalysisThread::startAnalysis: process request (mode HTTP_GET)";
  }
  else if(m_d->m_request->methodString() == "HTTP_POST")
  {
    LDEBUG << "AnalysisThread::startAnalysis: process request (mode HTTP_POST)";
  }

  QString language;
  QString pipeline ;
  QString textQS;
  std::map<std::string,std::string> metaData;
  QTemporaryFile tempFile;
  metaData["FileName"]=tempFile.fileName().toUtf8().constData();
  for(const auto& item: QUrlQuery(m_d->m_request->url()).queryItems(QUrl::FullyDecoded))
  {
    if (item.first == "lang")
    {
      language = item.second;
      metaData["Lang"]=language.toUtf8().constData();
      LDEBUG << "AnalysisThread::startAnalysis: language=" << language;
    }
    if (item.first == "pipeline")
    {
      pipeline = item.second;
      LDEBUG << "AnalysisThread::startAnalysis: pipeline=" << pipeline;
    }
    if (item.first == "meta")
    {
      m_d->initMetaData(item.second, metaData);
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
    const QByteArray& body = m_d->m_request->body();
    textQS = QString::fromUtf8( body.data() );
  }
  QString truncated = textQS.mid(0,20);
  LDEBUG << "AnalysisThread::startAnalysis: text='"<<truncated<<"...'";

  if( language.isEmpty() )
  {
    m_d->m_response_code = 400;
    m_d->m_response_body=QByteArray("Empty language");
    exitStatus=1;
  }
  else if( m_d->m_langs.find(metaData["Lang"]) == m_d->m_langs.end() )
  {
    m_d->m_response_code = 400;
    QString errorMessage = QString(tr("Language '%1' is not initialized")).arg(language);
    m_d->m_response_body=errorMessage.toUtf8();
    exitStatus=1;
  }
  else if( pipeline.isEmpty() )
  {
    m_d->m_response_code = 400;
    m_d->m_response_body=QByteArray("Empty pipeline");
    exitStatus=1;
  }
  else if( textQS.isEmpty() )
  {
    m_d->m_response_code = 400;
    m_d->m_response_body=QByteArray("Empty text");
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
    LinguisticProcessing::SimpleStreamHandler seLogWriter;
    handlers.insert(std::make_pair("se", &seLogWriter));
    auto simpleStreamHandler = new LinguisticProcessing::SimpleStreamHandler();
    handlers.insert(std::make_pair("simpleStreamHandler", simpleStreamHandler));

    std::ostringstream oss;
    seLogWriter.setOut(&oss);
    simpleStreamHandler->setOut(&oss);

    LDEBUG << "Analyzing" << language << textQS.mid(0,20) << "...";
    // std::ostringstream ots;

    std::set<std::string> inactiveUnits;
    try
    {
      m_d->m_analyzer->analyze(textQS,
                              metaData,
                              pipeline.toUtf8().constData(),
                              handlers,
                              inactiveUnits);
    }
    catch (const LinguisticProcessing::LinguisticProcessingException& e)
    {
      LIMASERVERLOGINIT;
      LERROR << "AnalysisThread::startAnalysis catch LinguisticProcessingException:"
              << e.what() << metaData;
      if (QString::fromUtf8(e.what()).startsWith("can't get pipeline"))
      {
        m_d->m_response_code = 400;
        m_d->m_response_body=QByteArray(e.what());
        exitStatus=1;
      }
      else
      {
        m_d->m_response_code = 400;
        m_d->m_response_body=QByteArray("Server error");
        exitStatus=1;
      }
      exit(exitStatus); // exit the eventLoop
      return;
    }
    QString resultString;
    if( m_d->m_mediaType.compare("text/xml") == 0)
    {
      resultString.append("<?xml version='1.0' encoding='UTF-8'?>\n");
      resultString.append("<tokens>\n");
      resultString.append(QString::fromUtf8(oss.str().c_str()));
      resultString.append("\n</tokens>\n");
    }
    else /*if( m_d->m_mediaType.compare("application/json") == 0)*/
    {
      resultString.append(m_d->TokensToJson(QString::fromUtf8(oss.str().c_str())));
    }

    LDEBUG << "AnalysisThread::startAnalysis: seLogger output is "
            << resultString.left(30);

    if( m_d->m_mediaType.compare("text/xml") == 0)
    {
      m_d->m_response_header.insert(
        std::pair<QString,QString>("Content-Type",
                                   "text/xml; charset=utf-8"));
    }
    else /*if( m_d->m_mediaType.compare("application/json") == 0)*/
    {
      m_d->m_response_header.insert(
        std::pair<QString,QString>("Content-Type",
                                   "application/json; charset=utf-8"));
    }
    m_d->m_response_code=200;

    m_d->m_response_body=resultString.toUtf8();
  }
#ifdef MULTITHREAD
    // m_d->m_request->deleteLater();
    // deleteLater();
    // quit(); // exit the eventLoop
    exit(exitStatus); // exit the eventLoop
#else
#endif
}

QString AnalysisThreadPrivate::TokensToJson( const QString & str )
{
  LIMASERVERLOGINIT;
  LDEBUG << "AnalysisThread::TokensToJson str=" << str.left(30) << "...";
//  LDEBUG << "AnalysisThreadPrivate::TokensToJson str=" << str;

  // Array of tokens
  QJsonArray array;
  // search for EOL: each line represents a token (or end of sentence for an empty line)
  auto lines = str.split('\n');
  for(const auto& line: lines)
  {
    // LDEBUG << "AnalysisThread::ConllToJson: pos:" << pos << ", i:"<< i;
    QJsonValue qval = QJsonValue(line);
    array.append(qval);
  }
  QJsonObject object;
  object["tokens"]=array;
  QJsonDocument doc(object);
  QString result(QString::fromUtf8(doc.toJson().constData()));

  LDEBUG << "AnalysisThreadPrivate::TokensToJson: result=" << result.left(80) << "...";
  return result;
}
