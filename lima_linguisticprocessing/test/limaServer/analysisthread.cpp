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
  QHttpResponse* m_response;
  const std::set<std::string>& m_langs;
  std::string m_mediaType;
};

AnalysisThreadPrivate::AnalysisThreadPrivate(Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* analyzer, 
                    QHttpRequest *req, QHttpResponse *resp,
                    const std::set<std::string>& langs) :
    m_analyzer(analyzer),
    m_request(req), m_response(resp),
    m_langs(langs),
    m_mediaType((req->header("content-type")).toStdString())
{
}

AnalysisThread::AnalysisThread (Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* analyzer, 
                  QHttpRequest *req, QHttpResponse *resp, 
                  const std::set<std::string>& langs, QObject* parent ):
    QThread(parent),
    m_d(new AnalysisThreadPrivate(analyzer,req,resp,langs))
{
  CORECLIENTLOGINIT;
  LDEBUG << "AnalysisThread::AnalysisThread()...";
  connect(this,SIGNAL(started()),this,SLOT(slotStarted()),Qt::QueuedConnection);
}

AnalysisThread::~AnalysisThread()
{
  delete m_d;
}

void AnalysisThread::slotStarted()
{
  CORECLIENTLOGINIT;
  LDEBUG << "AnalysisThread::slotStarted";

}

void AnalysisThread::startAnalysis()
{
  CORECLIENTLOGINIT;
  LDEBUG << "AnalysisThread::startAnalysis" << m_d->m_request->methodString() << m_d->m_request->url().path();
  LDEBUG << "AnalysisThread::startAnalysis mediaType=" << m_d->m_mediaType;
  if (m_d->m_request->methodString() == "HTTP_GET" && m_d->m_request->url().path() == "/extractEN")
  {
    QString language, pipeline, text;
    QPair<QString, QString> item;
    std::map<std::string,std::string> metaData;
    std::set<std::string> inactiveUnits;

    LDEBUG << "AnalysisThread::startAnalysis: process extractEN request (mode HTTP_GET)";
    
    std::map<std::string, AbstractAnalysisHandler*> handlers;
    LinguisticProcessing::SimpleStreamHandler* seLogWriter = new LinguisticProcessing::SimpleStreamHandler();
    handlers.insert(std::make_pair("se", seLogWriter));

    // OME?? Dont forget to delete it!!!
    std::ostringstream* oss = new std::ostringstream();
    seLogWriter->setOut(oss);
   
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    Q_FOREACH( item, m_d->m_request->url().queryItems(QUrl::FullyDecoded))
#else
    Q_FOREACH( item, QUrlQuery(m_d->m_request->url()).queryItems(QUrl::FullyDecoded))
#endif
    {
      QTemporaryFile tempFile;
      metaData["FileName"]=tempFile.fileName().toUtf8().constData();
      if (item.first == "lang")
      {
        language = item.second;
        metaData["Lang"]=language.toUtf8().data();
        LDEBUG << "AnalysisThread::startAnalysis: " << "language=" << language;
      }
      if (item.first == "pipeline")
      {
        pipeline = item.second;
        LDEBUG << "AnalysisThread::startAnalysis: " << "pipeline=" << pipeline;
      }
      if (item.first == "meta")
      {
        QString metadataValues= item.second;
        LDEBUG << "AnalysisThread::startAnalysis: " << "meta=" << metadataValues;
        std::string metaString = metadataValues.toStdString();
        std::string::size_type k=0;
        do {
          k=metaString.find(",");
          //if (k==std::string::npos) continue;
          std::string str(metaString,0,k);
          std::string::size_type i=str.find(":");
          if (i==std::string::npos) {
            std::cerr << "meta argument '"<< str <<"' is not of the form XXX:YYY: ignored" << std::endl;
          }
          else {
            //std::cout << "add metadata " << std::string(str,0,i) << "=>" << std::string(str,i+1) << std::endl;
            metaData.insert(std::make_pair(std::string(str,0,i),std::string(str,i+1)));
          }
          if (k!=std::string::npos) {
            metaString=std::string(metaString,k+1);
          }
        }  while (k!=std::string::npos);
        
      }
      if (item.first == "text")
      {
        text = item.second;
        LDEBUG << "AnalysisThread::startAnalysis: " << "text='" << text << "'";
      }
    }
    if( m_d->m_langs.find(metaData["Lang"]) == m_d->m_langs.end() )
    {
        m_d->m_response->writeHead(400);
        QString errorMessage = QString(tr("Language %1 is no initialized")).arg(language);
        m_d->m_response->end(errorMessage.toUtf8());
    }
    else if( !language.isEmpty() && !text.isEmpty() )
    {
      LDEBUG << "Analyzing" << language << text;
      std::ostringstream ots;
      std::string pipe = pipeline.toUtf8().data();
      pipe = "limaserver";
      m_d->m_analyzer->analyze(text,metaData, pipe, handlers, inactiveUnits);

      std::string resultString;
      if( m_d->m_mediaType.compare("text/xml") == 0) {
        resultString.append("<?xml version='1.0' encoding='UTF-8'?>");
        resultString.append(oss->str());
      }
      else if( m_d->m_mediaType.compare("application/json") == 0) {
        resultString.append(ConllToJson( oss->str()));
      }
      else
        resultString.append(ConllToJson( oss->str()));

      LDEBUG << "AnalysisThread::startAnalysis (GET): seLogger output is " << QString::fromUtf8(resultString.c_str());

      if( m_d->m_mediaType.compare("text/xml") == 0) {
        m_d->m_response->setHeader("Content-Type", "text/xml; charset=utf-8");
      }
      if( m_d->m_mediaType.compare("application/json") == 0) {
        m_d->m_response->setHeader("Content-Type", "application/json; charset=utf-8");
      }
      m_d->m_response->writeHead(200);

//      QString body = QString::fromUtf8(resultString.c_str());
//      m_d->m_response->end(body.toUtf8());
      m_d->m_response->end(QByteArray(resultString.c_str()));
      m_d->m_request->deleteLater();
      deleteLater();
      quit();
    }
    else
    {
        m_d->m_response->writeHead(400);
        m_d->m_response->end(QByteArray("Empty language or text"));
    }
    LDEBUG << "AnalysisThread::startAnalysis: delete oss... ";
    delete seLogWriter;
    delete oss; oss = 0;
  }
  // commande HTTP_POST
  else if (m_d->m_request->methodString() == "HTTP_POST" && m_d->m_request->url().path() == "/extractEN")
  {
    LDEBUG << "AnalysisThread::startAnalysis: process extractEN request (mode HTTP_POST)";

    QString language, pipeline, text_QS;
    std::string text_s;
    QPair<QString, QString> item;
    std::map<std::string,std::string> metaData;
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    Q_FOREACH( item, m_d->m_request->url().queryItems(QUrl::FullyDecoded))
#else
    Q_FOREACH( item, QUrlQuery(m_d->m_request->url()).queryItems(QUrl::FullyDecoded))
#endif
    {
      if (item.first == "lang")
      {
        language = item.second;
        metaData["Lang"]=language.toUtf8().data();
        LDEBUG << "AnalysisThread::startAnalysis: " << "language=" << language;
      }
      if (item.first == "pipeline")
      {
        pipeline = item.second;
        LDEBUG << "AnalysisThread::startAnalysis: " << "pipeline=" << pipeline;
      }
      if (item.first == "meta")
      {
        QString metadataValues= item.second;
        LDEBUG << "AnalysisThread::startAnalysis: " << "meta=" << metadataValues;
        std::string metaString = metadataValues.toStdString();
        std::string::size_type k=0;
        do {
          k=metaString.find(",");
          //if (k==std::string::npos) continue;
          std::string str(metaString,0,k);
          std::string::size_type i=str.find(":");
          if (i==std::string::npos) {
            std::cerr << "meta argument '"<< str <<"' is not of the form XXX:YYY: ignored" << std::endl;
          }
          else {
            //std::cout << "add metadata " << std::string(str,0,i) << "=>" << std::string(str,i+1) << std::endl;
            metaData.insert(std::make_pair(std::string(str,0,i),std::string(str,i+1)));
          }
          if (k!=std::string::npos) {
            metaString=std::string(metaString,k+1);
          }
        }  while (k!=std::string::npos);
        
      }
    }

    std::set<std::string> inactiveUnits;
    const QByteArray& body = m_d->m_request->body();
    text_s = std::string(body.data());

    std::map<std::string, AbstractAnalysisHandler*> handlers;
    LinguisticProcessing::SimpleStreamHandler* seLogWriter = new LinguisticProcessing::SimpleStreamHandler();
    handlers.insert(std::make_pair("se", seLogWriter));
    std::ostringstream* oss = new std::ostringstream();
    seLogWriter->setOut(oss);
   
    if( language.isEmpty() )
    {
      m_d->m_response->writeHead(400);
      m_d->m_response->end(QByteArray("Empty language"));
      return;
    }
    else if( m_d->m_langs.find(metaData["Lang"]) == m_d->m_langs.end() )
    {
      m_d->m_response->writeHead(400);
      QString errorMessage = QString(QLatin1String("Language %1 no initialized")).arg(language);
//  errorMessage << "language " << language " is no initialized"));
      m_d->m_response->end(errorMessage.toUtf8());
    }
    else if( !language.isEmpty() && !text_s.empty() )
    {
      metaData["Lang"]=language.toUtf8().data();
      metaData["Lang"]=language.toUtf8().data();
      QTemporaryFile tempFile;
      metaData["FileName"]=tempFile.fileName().toUtf8().constData();
      std::string pipe = pipeline.toUtf8().data();
      text_QS = Misc::utf8stdstring2limastring(text_s);

      m_d->m_analyzer->analyze(text_QS,metaData, pipe, handlers, inactiveUnits);
      std::string resultString;
      if( m_d->m_mediaType.compare("text/xml") == 0) {
        resultString.append("<?xml version='1.0' encoding='UTF-8'?>");
        resultString.append(oss->str());
      }
      else if( m_d->m_mediaType.compare("application/json") == 0) {
        resultString.append(ConllToJson(oss->str()));
      }
      else
        resultString.append(ConllToJson( oss->str()));
      LDEBUG << "AnalysisThread::startAnalysis (POST): seLogger output is " << QString::fromUtf8(resultString.c_str());

      if( m_d->m_mediaType.compare("text/xml") == 0) {
        m_d->m_response->setHeader("Content-Type", "text/xml; charset=utf-8");
      }
      else if( m_d->m_mediaType.compare("application/json") == 0) {
        m_d->m_response->setHeader("Content-Type", "text/json; charset=utf-8");
      }
      m_d->m_response->writeHead(200);
//      QString body = QString::fromUtf8(resultString.c_str());
//      m_d->m_response->end(body.toUtf8());
      m_d->m_response->end(QByteArray(resultString.c_str()));
    }
    else
    {
      m_d->m_response->writeHead(400);
      m_d->m_response->end(QByteArray("Empty language or text"));
      return;
    }
    delete seLogWriter;
    delete oss; oss = 0;
  }
  else
  {
    m_d->m_response->writeHead(405);
    m_d->m_response->setHeader("Allow","GET,POST");
    m_d->m_response->end(QByteArray("Only GET and POST search queries are currently allowed."));
  }
}

std::string  AnalysisThread::ConllToJson( const std::string & str )
{
  CORECLIENTLOGINIT;
  LDEBUG << "AnalysisThread::ConllToJson str=" <<str;
  
  // Array of tokens
  QJsonArray array;
  std::string::size_type pos = 0;
  std::string::size_type i=0;
  for(; i != std::string::npos ;) {
    // search for EOL: each line represents a token (or end of sentence for an empty line)
    i = str.find('\n', pos);
    LDEBUG << "AnalysisThread::ConllToJson: pos:" << pos << ", i:"<< i;
    QJsonValue qval;
    if( pos == i ) {
      // empty line = end of sentence
      qval = QJsonValue("");
    }
    else {
      std::string line;
      if( i == std::string::npos ) {
        line = std::string(str,pos);
      }
      else {
        line = std::string(str,pos,i-pos);
      }
      qval = QJsonValue(line.c_str());
    }
    array.append(qval);
    pos = i+1;
  }
  QJsonObject object;
  object["tokens"]=array;
  QJsonDocument doc(object);
  std::string result(doc.toJson().data());

  LDEBUG << "AnalysisThread::ConllToJson: result=" << result;
  return result;
}
