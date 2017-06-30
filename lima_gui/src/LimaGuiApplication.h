#ifndef LIMA_GUI_APPLICATION_H
#define LIMA_GUI_APPLICATION_H

#include "LimaGui.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "common/Handler/AbstractAnalysisHandler.h"

#include <QObject> 
#include <QString>

struct LimaGuiFile {
  std::string name;
  std::string url;
  bool modified = false;
};

class LimaGuiThread;

///
/// \brief Main class of Lima Gui.
///
class LimaGuiApplication : public QObject {
  Q_OBJECT
  
  /// BUFFER PROPERTIES
  Q_PROPERTY(QString fileContent MEMBER m_fileContent READ fileContent WRITE setFileContent)
  Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged READ text WRITE setText)
  Q_PROPERTY(QString fileName MEMBER m_fileName READ fileName WRITE setFileName)
  Q_PROPERTY(QString fileUrl MEMBER m_fileUrl READ fileUrl WRITE setFileUrl)
  Q_PROPERTY(bool ready MEMBER m_AnalyzerReady NOTIFY readyChanged)
  Q_PROPERTY(QString console MEMBER m_consoleOutput NOTIFY consoleChanged READ consoleOutput WRITE setConsoleOuput)
  
public:
  ///
  /// \brief LimaGuiApplication
  /// \param parent
  ///
  LimaGuiApplication(QObject* parent = 0);
  
  /// open file in application
  /// add a new entry to open_files
  Q_INVOKABLE bool openFile(QString filepath);
  
  Q_INVOKABLE bool openMultipleFiles(QStringList urls);
  
  /// save file registered in open_files
  Q_INVOKABLE bool saveFile(QString filename);
  
  /// save file registered in open_files, with a new url
  Q_INVOKABLE bool saveFileAs(QString filename, QString newUrl);
  
  /// close file registered in open files, save if modified and requested
  Q_INVOKABLE void closeFile(QString filename, bool save = false);
  
  LimaGuiFile* getFile(std::string name);
  
  /// if the file is open, set buffers file content, name and url to selected file's
  /// opening a file sets is as selected
  Q_INVOKABLE bool selectFile(QString filename);
  
  /// ANALYZER METHODS
  
  /// This is the main analysis method, ideally called in a thread
  void analyze(QString);

  /// This creates the thread for the previous function
  void beginNewAnalysis(QString);

  /// Those are all gui functions for ::analyze()

  /// analyze raw text
  /// An open file <that was edited but not saved> (it may not even be the case, all open files may be as well treated as text) will be passed to this function instead of analyzeFileFromUrl (then analyzeFile may as well call analyzeText)
  Q_INVOKABLE void analyzeText(QString content);
  
  Q_INVOKABLE void analyzeFile(QString filename);
  
  /// Analyze file directly from an url, without opening the file in the text editor ; (saved file content)
  Q_INVOKABLE void analyzeFileFromUrl(QString url);
  
  /// INITIALIZING
  
  /// initialize Lima::m_analyzer
  void initializeLimaAnalyzer();
  
  /// for the moment, to reset Lima configuration, we can only reinstantiate m_analyzer
  void resetLimaAnalyzer();
  
  /// THREADS MANAGEMENT
  
  void setOut(std::ostream* o) { out = o; }

//  LimaGuiThread* getThread(std::string name);

//  void newThread(std::string, LimaGuiThread*);

//  void destroyThread(std::string);
//  void destroyThread(LimaGuiThread*);
  
  friend class InitializeThread;

  void setTextBuffer(std::string str);

  /// actual qml console.
  /// we'll need to bind the qapplication with this class
  /// and then maybe we'll be able to access the related qml object from the root object
  /// https://stackoverflow.com/questions/9062189/how-to-modify-a-qml-text-from-c
  /// or
  /// https://stackoverflow.com/questions/35204281/use-signals-or-q-property-to-update-qml-objects
  void writeInConsole(std::string str);

  Q_INVOKABLE void test();

  QString fileContent() const;
  QString fileName() const;
  QString fileUrl() const;
  QString text() const;
  QString consoleOutput() const;

  void setFileContent(const QString& s);
  void setFileName(const QString& s);
  void setFileUrl(const QString& s);
  void setText(const QString& s);
  void setConsoleOuput(const QString& s);

Q_SIGNALS:
  void textChanged();
  void consoleChanged();
  void readyChanged();
  
private:
  
  /// BUFFERS
  
  QString m_fileContent;
  QString m_fileName;
  QString m_fileUrl;
  
  QString m_text;

  QString m_consoleOutput;
  
  bool m_AnalyzerReady = false;
  
  /// MEMBERS

  ///< list of open files;
  std::vector<LimaGuiFile> m_openFiles;
  
  std::shared_ptr< Lima::LinguisticProcessing::AbstractLinguisticProcessingClient > m_analyzer;
  
//  std::map<std::string, LimaGuiThread*> threads;
  std::ostream* out = &std::cout;
  
  /// PRIVATE METHODS
};

#endif // LIMA_GUI_APPLICATION_H
