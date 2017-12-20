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
  std::string ConllToJson( const std::string & str );

};

#endif // ANALYSISTHREAD_H
