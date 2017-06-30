#include <thread>

#include "Threads.h"
#include "LimaGuiApplication.h"

LimaGuiThread::LimaGuiThread(LimaGuiApplication* app) : QThread() {
  m_application = app;
  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

//void LimaGuiThread::clear() {
//  m_application->destroyThread(this);
//}

///
///
///



///
///
///
///

AnalysisThread::AnalysisThread(LimaGuiApplication* app) : LimaGuiThread(app) {

}

AnalysisThread::AnalysisThread(LimaGuiApplication *app, const QString& s) : LimaGuiThread(app) {
  text = s;
}

void AnalysisThread::run() {
  m_application->setOut(&out);
  m_application->analyze(text);
  m_application->setTextBuffer(out.str());
  m_application->setOut(&std::cout);
}

void AnalysisThread::setText(const QString& s) {
  text = s;
}

//////
///
///

InitializeThread::InitializeThread(LimaGuiApplication* app) : LimaGuiThread(app) {
}

void InitializeThread::run() {
//  std::thread mythread(&InitializeThread::doTheThing, this);
//  mythread.join();
  doTheThing();
}

void InitializeThread::doTheThing() {
  std::stringstream buffer;
  std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

//  for (int i = 0; i < 200000000; i++) {
//    m_application->setTextBuffer("Hello patient n°" + std::to_string(i) + "!");
//  }

  m_application->initializeLimaAnalyzer();

  m_application->setTextBuffer(buffer.str());

  m_application->m_AnalyzerReady = true;

  std::cout.rdbuf(old);
}
