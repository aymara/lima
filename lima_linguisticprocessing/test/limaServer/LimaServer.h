// Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    LimaServer( const QString& configPath,
                const QString& commonConfigFile,
                const QString& lpConfigFile,
                const QString& resourcesPath,
                const std::deque<std::string>& langs,
                const std::deque<std::string>& pipelines,
                const QString& meta,
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

