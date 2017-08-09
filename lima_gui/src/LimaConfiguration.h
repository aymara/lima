#ifndef LIMACONFIGURATION_H
#define LIMACONFIGURATION_H

#include "LimaGuiExport.h"

#include <QObject>
#include <QString>

#include <map>
#include <vector>

#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "common/Handler/AbstractAnalysisHandler.h"

// https://github.com/aymara/lima/wiki/Process-Units#flattokenizer

namespace Lima {
  namespace Common {
    namespace XMLConfigurationFiles {
      class XMLConfigurationFileParser;
      class GroupConfigurationStructure;
    }
  }
namespace Gui {

#define GConfigStruct Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure

struct ConfigurationFile {

};

enum NODE_TYPE {
  NONE,
  MODULE,
  PROCESS_UNIT,
  SEQUENCE_UNIT
};

struct ConfigNode {
  NODE_TYPE _type = NONE;

  bool _enabled;
  bool _enableable;

  std::map<std::string, std::string> _attributes;
  /// name, value, class, etc.

  std::vector<ConfigNode*> children;

  void add(ConfigNode* c) { children.push_back(c); }

  std::string toXML() {
    return "";
  }
};

struct ConfigTree {

  ConfigNode root;

  std::string toXML();

};

struct ProcessUnitConfiguration {

  ProcessUnitConfiguration(GConfigStruct& group) {
//    m_xmlGroup = &group;
    m_name = group.getName();
//    m_class = group.getAttribute("class");
  }

  std::string m_name;
  std::string m_class;

  GConfigStruct* m_xmlGroup;

  GConfigStruct asGroupConfigurationStructure();
};

struct ProcessUnitPipelineConfiguration {

  ProcessUnitPipelineConfiguration(const GConfigStruct& g);
//  {
   //m_name = g.getName();

//    auto& processUnitList = g.getListsValueAtKey("processUnitSequence");

//    for (auto& str : processUnitList) {
//      m_processUnits.push_back(str);
//    }

//  }

  void addProcessUnit(const std::string& str) {
    m_processUnits.push_back(str);
  }

  std::string m_name;

  std::vector<std::string> m_processUnits;

  GConfigStruct asGroupConfigurationStructure();

};



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

  std::string getName() const { return m_name; }

  void writeFile(Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser& xmlcfgparser, const std::string& path);

  std::map<std::string, std::string> m_langFiles;

//  Q_INVOKABLE void loadFromDirectory(const QString& path);



private:

  /// main -> ...
  /// easy -> ...
  /// even if only main interests us
  std::map<std::string, ProcessUnitPipelineConfiguration> m_processUnitPipelinesConfiguration;

  std::map<std::string, ProcessUnitConfiguration> m_processUnitsConfigurations;

  std::string m_name;

//  std::map<std::string, ConfigTree> content;

};

#endif // LIMACONFIGURATION_H

/// My problems :

/// You cannot edit easily the configuration structure. You cannot overwrite a module or a group that already exists without
/// modifying the source code of the class. Doing so may create udnefined behavior in the rest of Lima;
/// One solution would be to create a class specifically for this; still needs to see if xmlConfigurationFileParser
/// and its structures classes have a compatible visibility police.

/// In any case, it would require to create specific xml configuration files for the custom configurations.
/// Not with their own syntax, but the syntax found in files lima-lp-<lang>.xml for instance.
/// The thing is, do we want a configuration that is specific  to one language ? Because that's where
/// we are headed for now.
///
/// Or we could have the custom configurations have their own
///     lima-analysis.xml
///     X lima-lp-<lang>.xml
/// in config/custom/<name>/
///
/// But configuration files are separated by language ; this means that their configuration may differ.
///
/// Or you can just edit the groups one by one, but this may not be an ideal solution

/// On configuration files :
/// =======================================
///
/// - As far as I can attest, you can't hold only the files that interest you (lima-analysis/lima-lp) in one
/// directory and keep the rest in another. As far as it goes, you thus need to copy the entire config
/// directory even though most files won't change.
///
/// - The question is : do we create custom pipelines or custom config files ? I guess it's pipeline,
/// and in this case this should mean that it is independant of the selected language.
/// Then, what form would it take ?

/// You cannot specialize one -- Wait. It's possible. If you give it a different name while the same class,
/// it will work;
/// So you can have different versions of one pipeline unit with different names and parameters.
/// Same with dumpers and resources
/// /
/// - Ressources are called as parameters in pipeline units.
///
/// So, final questions :
///
///
/// How to use the least amount of files and the least amount of duplicates ?
/// How to organise custom configurations directories ?
/// How to render language and pipelines independant?
///
/// Creating a custom configuration will create a new set of configuration files.
/// Loading a custom configuration will require building on top of default configuration files and creating
/// a new analyser.
///
/// We need to have a class that enables overwriting a configuration structure with another.
///
/// Depends what files are required for resources and for CONF.

/// As for the configuration gui, we need:
/// - a list of all available process units and their description;
/// - a list of all parameters for each process unit
/// - a list of all possible values for every parameter they have;
/// - a list of all dependencies for each process unit

/// We may not have to overwrite configuration structures. Just load entire new ones.


} // Gui
} // Lima
