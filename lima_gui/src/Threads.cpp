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
  m_text = s;
}

void AnalysisThread::run() {
  if (m_application->available()) {
    m_application->setAnalyzerState(0);

    m_application->setOut(&out);

    m_application->analyze(m_text);

    // reset app out
    m_application->setOut(&std::cout);

    m_application->setAnalyzerState(1);

    // push results to app/gui
    m_application->setTextBuffer(out.str());

    // create model from out.str()
    QObject* view;
    if (m_resultView) {
      view = m_resultView;
    }
    else {
      view = m_application->getQmlObject("resultView"); // specific name
    }
    if (view) {
//      related: https://stackoverflow.com/questions/27092756/call-qml-function-from-c-with-another-qml-object-as-parameter

//      QObject * root = engine.rootObjects().at(0);
//      QQmlComponent comp(&engine, QUrl("qrc:/Test.qml"));
//      QMetaObject::invokeMethod(root, "addTab", Q_ARG(QVariant, QVariant::fromValue(&comp)));
      QString qstr(out.str().c_str());
      QMetaObject::invokeMethod(view, "displayResults", Q_ARG(QVariant, QVariant::fromValue(qstr)));
    }
    else {
      LTELL("No result view specified.");
    }
  }
  else {
    LTELL("Can't analyze : Analyzer is not available.");
  }
}

void AnalysisThread::setText(const QString& s) {
  m_text = s;
}

void AnalysisThread::setName(std::string name) {
  m_name = name;
}

void AnalysisThread::setResultView(QObject* o) {
  m_resultView = o;
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

  m_application->setAnalyzerState(1);

  std::cout.rdbuf(old);
}
