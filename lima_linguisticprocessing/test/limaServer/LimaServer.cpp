// Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "LimaServer.h"
#include "analysisthread.h"

// #ifdef WIN32
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
// #endif

#include "common/Data/strwstrtools.h"
#include "common/MediaProcessors/MediaAnalysisDumper.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"

// definition of logger
#include "linguisticProcessing/LinguisticProcessingCommon.h"
// factories
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/client/AbstractLinguisticProcessingClient.h"

#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QCoreApplication>
#include <QTimer>

#include <stdlib.h>
#include <boost/graph/buffer_concepts.hpp>

#include "../cmd_line_helpers.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing;

LimaServer::LimaServer( const QString& configPath,
                        const QString& commonConfigFile,
                        const QString& lpConfigFile,
                        const QString& resourcesPath,
                        const std::deque<std::string>& langs,
                        const std::deque<std::string>& pipelines,
                        const QString& meta,
                        int port,
                        QObject *parent,
                        QTimer* t)
     : QObject(parent), m_langs(langs.begin(),langs.end()), m_timer(t)
{
  LIMASERVERLOGINIT;
  LDEBUG << "::LimaServer::LimaServer()...";

  std::ostringstream oss;
  std::ostream_iterator<std::string> out_it (oss,", ");
  std::copy ( langs.begin(), langs.end(), out_it );
  LDEBUG << "LimaServer::LimaServer: init MediaticData("
           << "resourcesPath=" << resourcesPath
           << "configPath=" << configPath
           << "commonConfigFile=" << commonConfigFile
           << "lpConfigFile=" << lpConfigFile
           << "langs=" << oss.str()
           << "meta=" << meta;

  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath.toUtf8().constData(),
    configPath.toUtf8().constData(),
    commonConfigFile.toUtf8().constData(),
    langs,
    parse_options_line(meta, ',', ':', {{"lazy-init", "true"}}));

  // initialize linguistic processing
  QString fullLpConfigFile = findFileInPaths(configPath, lpConfigFile);
  std::string clientId("lima-coreclient");
  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(fullLpConfigFile.toUtf8().constData());

  LDEBUG << "LimaServer::LimaServer: configureClientFactory...";
  LinguisticProcessingClientFactory::changeable().configureClientFactory(
    clientId,
    lpconfig,
    langs,
    pipelines);

  LDEBUG << "LimaServer::LimaServer: createClient...";
  m_analyzer = std::dynamic_pointer_cast<AbstractLinguisticProcessingClient>(LinguisticProcessingClientFactory::single().createClient(clientId));

  LDEBUG << "LimaServer::LimaServer: create QHttpServer...";
  m_server = new QHttpServer(this);
  LDEBUG << "LimaServer::LimaServer: connect...";
  QMetaObject::Connection connection = connect(m_server,
                                               &QHttpServer::newRequest,
                                               this,
                                               &LimaServer::handleRequest);
  if( !connection ) {
    LERROR << "LimaServer::LimaServer: could not connect signal/slot.";
    LERROR << "LimaServer::LimaServer: Either signal / slot were not found, or the arguments did not match.";
    throw InvalidConfiguration("could not open connection");
  }
  LDEBUG << "LimaServer::LimaServer: connection signal/slot succesfull";

  LINFO << "LimaServer::LimaServer: server listen...";
  bool st = m_server->listen(QHostAddress::Any, port);
  if(!st) {
    LERROR << "LimaServer::LimaServer: could not listen on configured host and port (" << port << ")";
    LERROR << "LimaServer::LimaServer: this port might be already in use";
    throw InvalidConfiguration("could not listen on configured host and port");
  } else {
    LINFO << "Server listening on host" << QHostAddress::Any
          << "and port" << port;
  }
 }

LimaServer::~LimaServer()
{
}

void LimaServer::quit() {
  // free httpserver ?
  LIMASERVERLOGINIT;
  LINFO << "LimaServer::quit()...";
  m_timer->stop();
  m_server->close();
  LINFO << "LimaServer::quit(): ask close to tcpServer";
  QCoreApplication *app = (QCoreApplication *)parent();
  app->quit();
}


void LimaServer::handleRequest(QHttpRequest *req, QHttpResponse *resp)
{
  LIMASERVERLOGINIT;
  req->storeBody();
  LDEBUG << "LimaServer::handleRequest:insert" << resp << " at " << req << "...";
  // Need to get response from request in slotHandleEndedRequest
  m_responses.insert(std::pair<QHttpRequest*, QHttpResponse *>(req,resp));

#ifdef MULTITHREAD
  connect(req,SIGNAL(end()),this,SLOT(slotHandleEndedRequest()));
  connect(resp,SIGNAL(done()),this,SLOT(slotResponseDone()));
#else
#endif
}

void LimaServer::slotHandleEndedRequest()
{
  LIMASERVERLOGINIT;
  LDEBUG << "LimaServer::slotHandleEndedRequest";
  QHttpRequest *req = static_cast<QHttpRequest*>(sender());
  QHttpResponse *resp = m_responses[req];

  LDEBUG << "LimaServer::slotHandleEndedRequest: create AnalysisThread...";
  AnalysisThread *thread = new AnalysisThread(m_analyzer.get(), req, resp, m_langs, this );
  // Need to get request from thread in sendResults
  m_requests.insert(std::pair<QThread*,QHttpRequest*>(thread,req));
  m_threads.insert(std::pair<QHttpResponse*,QThread*>(resp,thread));

#ifdef MULTITHREAD
  connect(thread, SIGNAL(finished()), this, SLOT(sendResults()));
//  connect(thread,SIGNAL(finished()),thread, SLOT(deleteLater()));
  thread->start();
  LDEBUG << "LimaServer::slotHandleEndedRequest: my job is done...";
#else
#endif
}


void LimaServer::slotResponseDone()
{
  QHttpResponse* response = static_cast<QHttpResponse*>(sender());
  LIMASERVERLOGINIT;
  LDEBUG << "LimaServer::slotResponseDone";
  QThread* thread = m_threads[response];
  if (thread->isRunning())
    thread->quit();
  LDEBUG << "LimaServer::slotResponseDone done";
}

void LimaServer::sendResults()
{
  LIMASERVERLOGINIT;
  QThread* thread = static_cast<QThread*>(sender());
  LDEBUG << "LimaServer::sendResults for thread" << thread;
  QHttpRequest *req = m_requests[thread];
  QHttpResponse *resp = m_responses[req];
  AnalysisThread* analysisthread = static_cast<AnalysisThread*>(sender());

  const std::map<QString,QString>& headers = analysisthread->response_header();
  for( std::map<QString,QString>::const_iterator headerIt = headers.begin() ; headerIt != headers.end() ; headerIt++ ) {
    LDEBUG << "LimaServer::sendResults headers : " << headerIt->first << ": " << headerIt->second ;
    resp->setHeader(headerIt->first, headerIt->second);
  }
  resp->writeHead(analysisthread->response_code());

  resp->end(analysisthread->response_body());

  // req->deleteLater();
  // thread->deleteLater();

  // TODO: clean: remove element from m_requests and m_responses
  LDEBUG << "LimaServer::sendResults done";

}
