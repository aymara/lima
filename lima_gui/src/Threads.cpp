/**
 * \file    Threads.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#include "Threads.h"
#include "LimaGuiApplication.h"
#include "ConllListModel.h"

#include "common/LimaCommon.h"


#define ANALYSISTHREADLOGINIT LOGINIT("Lima::Gui::AnalysisThread");
#define INITTHREADLOGINIT LOGINIT("Lima::Gui::InitializeThread");

namespace Lima 
{
namespace Gui 
{

LimaGuiThread::LimaGuiThread(LimaGuiApplication* app) : QThread() 
{
  m_application = app;
  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

///
///
///
///
///

AnalysisThread::AnalysisThread(LimaGuiApplication* app) : LimaGuiThread(app) 
{

}

AnalysisThread::AnalysisThread(LimaGuiApplication *app, const QString& s) : LimaGuiThread(app) 
{
  m_text = s;
}

void AnalysisThread::run() 
{
  if (m_application->available()) 
  {
    std::stringstream buffer;
//     std::streambuf* old_cout = std::cout.rdbuf(buffer.rdbuf());
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());

    m_application->setOut(&out);
    m_application->analyze(m_text);

    // reset app out
    m_application->setOut(&std::cout);
    m_application->setTextBuffer(buffer.str());
//     std::cout.rdbuf(old_cout);
    std::cerr.rdbuf(old_cerr);

    notifyView();
  }
  else 
  {
    ANALYSISTHREADLOGINIT;
    LINFO << ("Can't analyze : Analyzer is not available.");
  }
}

void AnalysisThread::notifyView()
{
  ANALYSISTHREADLOGINIT;
  LDEBUG << "AnalysisThread::notifyView";
  // create model from out.str()
  QObject* view;
  if (m_resultView)
  {
    view = m_resultView;
  }
  else 
  {
    view = m_application->getQmlObject("resultView"); // specific name
  }
  if (view) 
  {
//      related: https://stackoverflow.com/questions/27092756/call-qml-function-from-c-with-another-qml-object-as-parameter

    QString qstr(out.str().c_str());
    LDEBUG << "AnalysisThread::notifyView invoke displayResults with" 
            << m_text << "and" << qstr;
    QMetaObject::invokeMethod(view, 
                              "displayResults", 
                              Q_ARG(QVariant, QVariant::fromValue(m_text)), 
                              Q_ARG(QVariant, QVariant::fromValue(qstr)));
//    QMetaObject::invokeMethod(view, "tableUp", Q_ARG(QVariant, QVariant::fromValue(clmodel)));
  }
  else 
  {
    ANALYSISTHREADLOGINIT;
    LERROR << "Error : No result view specified.";
  }
}

void AnalysisThread::setText(const QString& s) 
{
  m_text = s;
}

void AnalysisThread::setName(const std::string& name) 
{
  m_name = name;
}

void AnalysisThread::setResultView(QObject* o) 
{
  m_resultView = o;
}

//////

InitializeThread::InitializeThread(LimaGuiApplication* app) : LimaGuiThread(app) 
{
}

void InitializeThread::run() 
{
  std::stringstream buffer;
  /// This won't work though
  // std::streambuf* old_cout = std::cout.rdbuf(buffer.rdbuf());
  // std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
  
  //  for (int i = 0; i < 200000000; i++) {
  //    m_application->setTextBuffer("Hello patient n°" + std::to_string(i) + "!");
  //  }
  
  m_application->initializeLimaAnalyzer();
  
  //  m_application->setTextBuffer(buffer.str());
  
  m_application->setAnalyzerState(1);
  
  // restore previous streams
  // std::cout.rdbuf(old_cout);
  // std::cerr.rdbuf(old_cerr);
}

} // END namespace Gui
} // END namespace Lima
