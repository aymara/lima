#ifndef LIMACONFIGURATION_H
#define LIMACONFIGURATION_H

#include "LimaGuiExport.h"

#include <QObject>
#include <QString>
#include <QAbstractListModel>

#include <map>
#include <vector>

#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "common/Handler/AbstractAnalysisHandler.h"

// https://github.com/aymara/lima/wiki/Process-Units#flattokenizer

namespace Lima {

namespace Common {
  namespace XMLConfigurationFiles {
    class XMLConfigurationFileParser;
    class ConfigurationStructure;
    class ModuleConfigurationStructure;
    class GroupConfigurationStructure;
  }
}

namespace Gui {
  
namespace Config {

/// How it could be done :
///
/// First load the default configuration files (lima-analysis.xml)
///
/// This file contains references to language-specific

class LIMA_GUI_EXPORT LimaConfiguration : public QObject {
Q_OBJECT
public:
  LimaConfiguration();

  void saveAsXml();

  void loadFromFile(const std::string& path);

  std::string name() const { return m_name; }
  std::string path() const { return m_path; }

  void setName(const std::string& n) {m_name = n;}
  void setPath(const std::string& p) {m_path = p;}

  void writeFile(Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser& xmlcfgparser, const std::string& path);


//  Q_INVOKABLE void loadFromDirectory(const QString& path);

private:

//  std::map<std::string, ProcessUnitPipelineConfiguration> m_processUnitPipelinesConfiguration;

//  std::map<std::string, ProcessUnitConfiguration> m_processUnitsConfigurations;

  std::string m_name;
  std::string m_path;

//  std::map<std::string, ConfigTree> content;

};

#endif // LIMACONFIGURATION_H

} // Config
} // Gui
} // Lima
