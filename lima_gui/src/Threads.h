#ifndef lima_gui_analysis_thread_h
#define lima_gui_analysis_thread_h

#include "LimaGui.h"
#include <sstream>

#include <QThread>

class LimaGuiApplication;

class LimaGuiThread : public QThread {
  Q_OBJECT
public:
  LimaGuiThread(LimaGuiApplication*);
  virtual void run() = 0;

  // vetuste ? you should call it to have the thread deleted properly in run()
  void clear();

protected:
  LimaGuiApplication* m_application;
};

class AnalysisThread : public LimaGuiThread {
  
public:
  AnalysisThread(LimaGuiApplication*);
  AnalysisThread(LimaGuiApplication*, const QString&);
  void run();
  
  std::ostream& getOut() { return out; }

  void setText(const QString&);
  void setResultView(QObject*);
  void setName(const std::string&);

  void notifyView();

private:
  std::stringstream out;

  // identify the analysis
  // ideally the name of the file
  // or a generated name
  std::string m_name;

  QObject* m_resultView = nullptr;

  // content to analyze
  QString m_text;
  // configuration;
  //
};

// Thread to initialize lima ?
class InitializeThread : public LimaGuiThread {
  
public:
  InitializeThread(LimaGuiApplication* a);
  void run();
  void doTheThing();
};

class TestThread : public LimaGuiThread {
public:
  TestThread(LimaGuiApplication* application);
  void run();
};

#endif // lima_gui_analysis_thread_h
