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

class LimaGuiApplication : public QObject {
  Q_OBJECT
  
  /// BUFFER PROPERTIES
  Q_PROPERTY(QString fileContent MEMBER m_fileContent)
  Q_PROPERTY(QString text MEMBER m_text)
  Q_PROPERTY(QString fileName MEMBER m_fileName)
  Q_PROPERTY(QString fileUrl MEMBER m_fileUrl)
  
public:
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
  
  /// analyze raw text
  /// An open file <that was edited but not saved> (it may not even be the case, all open files may be as well treated as text) will be passed to this function instead of analyzeFileFromUrl (then analyzeFile may as well call analyzeText)
  Q_INVOKABLE void analyzeText(QString content);
  
  Q_INVOKABLE void analyzeFile(QString filename);
  
  /// Analyze file directly from an url, without opening the file in the text editor ; (saved file content)
  Q_INVOKABLE void analyzeFileFromUrl(QString url);
  
private:
  
  /// BUFFERS
  
  QString m_fileContent;
  QString m_fileName;
  QString m_fileUrl;
  
  QString m_text;
  
  /// MEMBERS
  
  ///< list of open files; short_name -> url
  std::vector<LimaGuiFile> m_openFiles;
  
  std::shared_ptr< Lima::LinguisticProcessing::AbstractLinguisticProcessingClient > m_analyzer;
  
  /// PRIVATE METHODS
  
  /// initialize Lima::m_analyzer
  void initializeLimaAnalyzer();
  
  /// for the moment, to reset Lima configuration, we can only reinstantiate m_analyzer
  void resetLimaAnalyzer();
};

#endif // LIMA_GUI_APPLICATION_H
