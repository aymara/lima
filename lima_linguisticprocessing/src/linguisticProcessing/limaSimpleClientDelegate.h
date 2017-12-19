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

namespace Lima {

class Q_DECL_EXPORT LimaSimpleClientDelegate :public QObject 
{
Q_OBJECT
public:
  LimaSimpleClientDelegate();
  ~LimaSimpleClientDelegate();

  void initialize(const std::string& language, const std::string& pipeline);
  std::string analyze(const std::string& text);
  
  std::string m_language;
  std::string m_pipeline;
  std::shared_ptr< Lima::LinguisticProcessing::AbstractLinguisticProcessingClient > m_client;
  Lima::LinguisticProcessing::SimpleStreamHandler* m_handler; // the handler to use
  std::map<std::string, Lima::AbstractAnalysisHandler*> m_handlers; // the list of handlers (contains only m_handler, but keep it to make it easy)

  // if first initialization, has to initialize common data and configure factory, otherwise, only
  // generates the client
  bool m_firstInitialization;
  
private Q_SLOTS:
  void onStarted();

private:
  // internal thread to run QCoreApplication
  static int argc;
  static char* argv[2];
  static QCoreApplication* app;
  static QThread* thread;

};

} // end namespace

#endif
