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

 #ifndef LIMASERVER_H
 #define LIMASERVER_H

#include "qhttpserver.h"

#include <deque>
#include <vector>
#include <set>
#include <string>
#include <memory>

class QTimer;

namespace Lima {
  namespace LinguisticProcessing {
    class AbstractLinguisticProcessingClient;
  }
}

class LimaServer : public QObject
{
    Q_OBJECT

public:
    LimaServer( const std::string& limaServerConfigFile,
                const std::deque<std::string>& langs,
                const std::deque<std::string>& pipelines,
                int port,
                QObject *parent,
                QTimer* t);
    virtual ~LimaServer();

private Q_SLOTS:
    void quit();
    void handleRequest(QHttpRequest* req, QHttpResponse* resp);
    void slotHandleEndedRequest();
    void sendResults();
    void slotResponseDone();

private:
  const std::set<std::string> m_langs;
  QHttpServer *m_server;
  QTimer* m_timer;
  std::map<QHttpRequest*,QHttpResponse*> m_responses;
  std::map<QThread*,QHttpRequest*> m_requests;
  std::map<QHttpResponse*,QThread*>m_threads;


  std::shared_ptr< Lima::LinguisticProcessing::AbstractLinguisticProcessingClient > m_analyzer;
};

 #endif
 
