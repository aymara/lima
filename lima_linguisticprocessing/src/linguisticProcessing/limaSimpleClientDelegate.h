/************************************************************************
 *
 * @file       limaSimpleClientDelegate.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Dec 19 2017
 * copyright   Copyright (C) 2017 by CEA - LIST
 * Project     Lima
 * 
 * @brief      Internal implementation of the lima simple client (in separate header because needed by Q_OBJECT)
 * 
 * 
 ***********************************************************************/

#ifndef LIMASIMPLECLIENTDELEGATE_H
#define LIMASIMPLECLIENTDELEGATE_H

#include "linguisticProcessing/client/AbstractLinguisticProcessingClient.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include <iostream>
#include <QObject>
#include <QCoreApplication>
#include <QThread>
#include <thread>
#include <boost/thread.hpp>

Q_DECLARE_METATYPE(std::string)

namespace Lima {

// the LimaWorker does the job (launches lima analysis on a text)
// the LimaController tells LimaWorker to do the job
// the LimaSimpleClientDelegate holds the thread in which the lima worker and controller run + the QCoreApplication
// LimaController and LimaWorker communicate using Qt signals and slots (only way to be taken into account in the
// main event loop of the QCoreApplication): both are QObject. In order to avoid the WARNING (QApplication was not created in the main() thread), 
// all QObjects must be created in the same thread as the QCoreApplication (hence the need of a controller in the thread, different
// from the delegate).

class LimaWorker: public QObject
{
Q_OBJECT
public:
  LimaWorker(QObject* parent=Q_NULLPTR);
  ~LimaWorker();

  const std::string& getResult() const { return m_result; }
  
public Q_SLOTS:
  void initialize(const std::string& language, const std::string& pipeline);
  void analyze(const std::string& text);
  void quit();

Q_SIGNALS:
  void finishedInit();
  void finishedAnalyze();
  
public:  
  std::string m_language;
  std::string m_pipeline;
  std::shared_ptr< Lima::LinguisticProcessing::AbstractLinguisticProcessingClient > m_client;
  Lima::LinguisticProcessing::SimpleStreamHandler* m_handler; // the handler to use
  std::map<std::string, Lima::AbstractAnalysisHandler*> m_handlers; // the list of handlers (contains only m_handler, but keep it to make it easy)

  // if first initialization, has to initialize common data and configure factory, otherwise, only
  // generates the client
  bool m_firstInitialization;

private:
  std::string m_result;  
};

class LimaController: public QObject
{
Q_OBJECT
public:
   LimaController(QObject* parent=Q_NULLPTR);
  ~LimaController();

  bool hasFinishedInit() { return m_finishedInit; }
  bool hasFinishedAnalyze() { return m_finishedAnalyze; }
  
  void initialize(const std::string& language, const std::string& pipeline);
  void analyze(const std::string& text);
  
  void stop();

public Q_SLOTS:
  void endInit() { m_finishedInit=true; }
  void endAnalyze() { m_finishedAnalyze=true; }
  
Q_SIGNALS:
  void doInitialize(const std::string& language, const std::string& pipeline);
  void doAnalyze(const std::string& text);
  void closeApp(); // to stop the app
  void closeWorker(); // to stop the worker properly

private:
  bool m_finishedInit;
  bool m_finishedAnalyze;
};

class LimaSimpleClientDelegate 
{
public:
  LimaSimpleClientDelegate();
  ~LimaSimpleClientDelegate();

  void initialize(const std::string& language, const std::string& pipeline);
  std::string analyze(const std::string& text);
  
private:
  static void onStarted();
  
  static LimaWorker* m_worker;
  static LimaController* m_controller;
  
  // internal thread to run QCoreApplication
  static int argc;
  static char* argv[2];
  static QCoreApplication* app;
  // must use a std::thread (or boost), not a QThread, to run the QApplication (otherwise, have a 
  // warning: QApplication was not created in the main() thread.)
  static std::thread* thread;
};

} // end namespace

#endif
