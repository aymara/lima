// Copyright (C) <year>  <name of author>
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef ANALYSISTHREAD_H
#define ANALYSISTHREAD_H

#include <set>
#include <QtCore/QThread>

class QObject;
class QHttpRequest;
class QHttpResponse;

namespace Lima
{
  namespace LinguisticProcessing
  {
    class AbstractLinguisticProcessingClient;
  }
}

#define MULTITHREAD 1

class AnalysisThreadPrivate;
/**
 * @brief Analyser thread
 */
#ifdef MULTITHREAD
class AnalysisThread : public QThread
#else
class AnalysisThread : public QObject
#endif
{
  Q_OBJECT
public:
    AnalysisThread (Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* m_analyzer,
                  QHttpRequest *req, QHttpResponse *resp,
                  const std::set<std::string>& langs, QObject* parent = 0 );
    virtual ~AnalysisThread();

    virtual void  run () override;

    const QByteArray& response_body() const;
    int response_code() const;
    const std::map<QString,QString>& response_header() const;

Q_SIGNALS:
  void anlysisFinished();
  void ready();

public Q_SLOTS:
    void startAnalysis();
    // void slotStarted();

private:
  AnalysisThreadPrivate* m_d;
};

#endif // ANALYSISTHREAD_H
