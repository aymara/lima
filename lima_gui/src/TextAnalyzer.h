#ifndef text_analyzer_h
#define text_analyzer_h

#include <QObject> 
#include <QString>

class TextAnalyzer : public QObject {
  Q_OBJECT
  
  Q_PROPERTY(QString filepath MEMBER m_file)
  Q_PROPERTY(QString text MEMBER m_text)
  
public:
  TextAnalyzer(QObject* p = 0) : QObject(p) {}
  
  Q_INVOKABLE void analyzeText();
  Q_INVOKABLE void analyzeFile();
  Q_INVOKABLE void tr_analyzeFile();
  
private:
  QString m_file;
  QString m_text;
  

  // config files:
  //  ~/Lima/Dist/lima-gui/debug/share/config/lima
  
};

#endif // text_analyzer_h
