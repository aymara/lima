#ifndef text_analyzer_h
#define text_analyzer_h

// factories
#include "LimaGui.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "common/Handler/AbstractAnalysisHandler.h"

#include <QObject> 
#include <QString>

class TextAnalyzer : public QObject {
  Q_OBJECT
  
  Q_PROPERTY(QString filepath MEMBER m_file)
  Q_PROPERTY(QString text MEMBER m_text)
  
  void initializeAnalyzer();
  void resetAnalyzer();
  
public:
  TextAnalyzer(QObject* p = 0);
  
  Q_INVOKABLE void analyzeText();
  Q_INVOKABLE void analyzeFile();
  Q_INVOKABLE void tr_analyzeFile();
  
private:
  QString m_file;
  QString m_text;
  
  // LIMA RELATED
  
  std::shared_ptr< Lima::LinguisticProcessing::AbstractLinguisticProcessingClient > m_analyzer;
  
  std::map<std::string, Lima::AbstractAnalysisHandler*> generateHandlers();

  // config files:
  //  ~/Lima/Dist/lima-gui/debug/share/config/lima
  
};

#endif // text_analyzer_h
