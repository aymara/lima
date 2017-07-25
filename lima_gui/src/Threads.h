#ifndef lima_gui_analysis_thread_h
#define lima_gui_analysis_thread_h

#include "LimaGui.h"
#include <sstream>

#include <QThread>

namespace Lima {
namespace Gui {

class LimaGuiApplication;

/// \class LimaGuiThread
///
/// \brief This class is meant to be related with a LimaGuiApplication object.
class LimaGuiThread : public QThread {
  Q_OBJECT
public:
  LimaGuiThread(LimaGuiApplication* app);

  ///
  /// \brief method inherited from QThread
  virtual void run() = 0;


protected:
  LimaGuiApplication* m_application = nullptr;
};

/// \class AnalysisThread
/// \brief This thread starts the analysis with preferred parameters.
class AnalysisThread : public LimaGuiThread {
  
public:

  ///
  /// \brief Bare constructor. The text is empty by default
  AnalysisThread(LimaGuiApplication* app);

  ///
  /// \param text : text to be analyzed
  AnalysisThread(LimaGuiApplication* app, const QString& text);

  ///
  void run();
  
  std::ostream& getOut() { return out; }

  /// SETTERS

  void setText(const QString&);
  void setResultView(QObject*);
  void setName(const std::string&);

  /// \brief Send the results to the view.
  void notifyView();

private:

  ///< the ostream of the thread to receive the results
  std::stringstream out;

  // identify the analysis
  // ideally the name of the file
  // or a generated name
  std::string m_name;

  ///< the QML QObject (a ResultView object) to be notified once the analysis is completed
  QObject* m_resultView = nullptr;

  ///< content to analyze
  QString m_text;


};

/// \class InitializeThread
/// \brief Thread to initialize lima
class InitializeThread : public LimaGuiThread {
  
public:
  InitializeThread(LimaGuiApplication* a);

  /// \brief This simply calls LimaGuiApplication::initializeLimaAnalyzer
  void run();

  void doTheThing();
};

class TestThread : public LimaGuiThread {
public:
  TestThread(LimaGuiApplication* application);
  void run();
};

} // END namespace Gui
} // END namespace Lima

#endif // lima_gui_analysis_thread_h
