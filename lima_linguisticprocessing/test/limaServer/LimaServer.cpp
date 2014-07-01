/****************************************************************************
 **
 ** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
 **     of its contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include "LimaServer.h"
#include "analysisthread.h"

// #ifdef WIN32
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
// #endif

#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/misc/gregoriannowarn.hpp"
#include "common/misc/posix_timenowarn.hpp"
#include "common/time/traceUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/MediaProcessors/MediaAnalysisDumper.h"

// definition of logger
#include "linguisticProcessing/LinguisticProcessingCommon.h"
// factories
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"

#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

#include <QtCore/QSettings>
#include <QtCore/QStringList>

#include <stdlib.h>
#include <boost/graph/buffer_concepts.hpp>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing;

LimaServer::LimaServer( const std::string& configDir,
		   const std::deque<std::string>& langs,
		   const std::vector<std::string>& vinactiveUnits,
		   const std::deque<std::string>& pipelines,
		   int port,
		   QObject *parent)
     : QObject(parent), m_langs(langs.begin(),langs.end())
{
  CORECLIENTLOGINIT;
  qDebug() << "::LimaServer::LimaServer()...";
  
  // initialize common
  std::string resourcesPath = qgetenv("LIMA_RESOURCES").constData();
  if( resourcesPath.empty() )
    resourcesPath = "/usr/share/apps/lima/resources/";
  std::string commonConfigFile("lima-common.xml");

  std::ostringstream oss;
  std::ostream_iterator<std::string> out_it (oss,", ");
  std::copy ( langs.begin(), langs.end(), out_it );
  qDebug() << "LimaServer::LimaServer: init MediaticData("
           << "resourcesPath=" << resourcesPath
           << "configDir=" << configDir
           << "commonConfigFile=" << commonConfigFile
           << "langs=" << oss.str();
  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);
  
  // initialize linguistic processing
  std::string clientId("lima-coreclient");
  std::string lpConfigFile("lima-analysis.xml");
  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configDir + "/" + lpConfigFile);

  qDebug() << "LimaServer::LimaServer: configureClientFactory...";
  LinguisticProcessingClientFactory::changeable().configureClientFactory(
    clientId,
    lpconfig,
    langs,
    pipelines);
  
  qDebug() << "LimaServer::LimaServer: createClient...";
  m_analyzer=static_cast<AbstractLinguisticProcessingClient*>(LinguisticProcessingClientFactory::single().createClient(clientId));
  
  
  qDebug() << "LimaServer::LimaServer: create QHttpServer...";
  QHttpServer *server = new QHttpServer(this);
  qDebug() << "LimaServer::LimaServer: connect...";
  connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
          this, SLOT(handleRequest(QHttpRequest*, QHttpResponse*)));

  qDebug() << "LimaServer::LimaServer: server listen...";
  server->listen(QHostAddress::Any, port);
  qDebug() << "Server listening";
 }

LimaServer::~LimaServer()
{
  qDebug() << "LimaServer::~LimaServer";
}

void LimaServer::handleRequest(QHttpRequest *req, QHttpResponse *resp)
{
  req->storeBody();
  qDebug() << "LimaServer::handleRequest: create AnalysisThread...";
  AnalysisThread *thread = new AnalysisThread(m_analyzer, req, resp, m_langs, this );
  connect(req,SIGNAL(end()),thread,SLOT(startAnalysis()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  thread->start();
 }

