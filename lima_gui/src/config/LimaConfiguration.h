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

  void process(const std::string& path);

  std::string name() const { return m_name; }

  void writeFile(Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser& xmlcfgparser, const std::string& path);

  std::map<std::string, std::string> m_langFiles;

//  Q_INVOKABLE void loadFromDirectory(const QString& path);

private:

  /// main -> ...
  /// easy -> ...
  /// even if only main interests us
//  std::map<std::string, ProcessUnitPipelineConfiguration> m_processUnitPipelinesConfiguration;

//  std::map<std::string, ProcessUnitConfiguration> m_processUnitsConfigurations;

  std::string m_name;

//  std::map<std::string, ConfigTree> content;

};

#endif // LIMACONFIGURATION_H

} // Config
} // Gui
} // Lima
