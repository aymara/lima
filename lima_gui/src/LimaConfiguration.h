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

enum CONFIGURATION_NODE_TYPE {
  NONE,
  FILE,
  MODULE,
  LIST,
  GROUP,
  MAP,
  ITEM,
  ENTRY,
  PARAM
};

std::string typeName(CONFIGURATION_NODE_TYPE);

struct ConfigurationNode {
  CONFIGURATION_NODE_TYPE type;
  int id;
  static int pid;

  ConfigurationNode();
  ConfigurationNode(const Lima::Common::XMLConfigurationFiles::ConfigurationStructure&);
  ConfigurationNode(const Lima::Common::XMLConfigurationFiles::ModuleConfigurationStructure&);
  ConfigurationNode(const Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure&);

  std::map<std::string, std::string> attributes;
  void addAttribute(const std::string& key, const std::string& value);
  bool hasAttribute(const std::string& key);
  std::string getName();
  std::string getAttribute(const std::string& key);
  // std::map<std::string, std::deque<std::string>> attributesChoices;

  std::vector<std::shared_ptr<ConfigurationNode>> children;
  void addChild(ConfigurationNode* cn);

  bool checked;

  bool checkable();
  bool visible();

  bool toggleById(int);

  void fromMap(const std::string& name, const std::map<std::string, std::string>& map);
  void fromEntry(const std::string& key, const std::string& value);
  void fromList(const std::string& name, const std::deque<std::string>& list);
  void fromItem(const std::string& item);
  void fromParam(const std::string& key, const std::string& value);

  void mask(const Lima::Common::XMLConfigurationFiles::ConfigurationStructure& cstruct);

  Lima::Common::XMLConfigurationFiles::ConfigurationStructure toConfigurationStructure();

};

class ConfigurationTree {
  CONFIGURATION_NODE_TYPE type;

  ConfigurationNode root;

//  ConfigurationTree(const CS& cstruct);

};

class LimaConfigurationNodeModel : public QAbstractListModel {
  Q_OBJECT
public:
  LimaConfigurationNodeModel(QObject* p = 0);
  LimaConfigurationNodeModel(ConfigurationNode*, QObject* p = 0);
  void fromConfigurationNode(ConfigurationNode*);

  /// REIMPLEMENTED METHODS

  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  QHash<int, QByteArray> roleNames() const;


private:
  ConfigurationNode* node;

  QList<std::shared_ptr<LimaConfigurationNodeModel>> children;

  enum {
    ID = Qt::UserRole + 1,
    NAME,
    CHECKED,
    CONTENTS
  };
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
//  std::map<std::string, ProcessUnitPipelineConfiguration> m_processUnitPipelinesConfiguration;

//  std::map<std::string, ProcessUnitConfiguration> m_processUnitsConfigurations;

  std::string m_name;

//  std::map<std::string, ConfigTree> content;

};

#endif // LIMACONFIGURATION_H

} // Gui
} // Lima
