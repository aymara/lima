/**
 * \file    LimaConfiguration.h
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#ifndef LIMACONFIGURATION_H
#define LIMACONFIGURATION_H

#include "LimaGuiExport.h"

#include <QObject>
#include <QString>
#include <QAbstractListModel>

#include <map>
#include <vector>
#include <deque>


// https://github.com/aymara/lima/wiki/Process-Units#flattokenizer

class QXmlStreamWriter;

namespace Lima 
{
namespace Common 
{
  namespace XMLConfigurationFiles 
  {
    class XMLConfigurationFileParser;
    class ConfigurationStructure;
    class ModuleConfigurationStructure;
    class GroupConfigurationStructure;
  }
}

namespace Gui 
{
  
namespace Config 
{

///
/// \brief This class figures a custom configuration; name and path
/// 
/// The idea was to have a class the configuration gui would instantiate to
/// create a new configuration. For each language, write the corresponding
/// config file with the writeFile function.
class LIMA_GUI_EXPORT LimaConfiguration : public QObject 
{
Q_OBJECT
public:
  LimaConfiguration();

  void loadFromFile(const std::string& path);

  std::string name() const { return m_name; }
  std::string path() const { return m_path; }

  void setName(const std::string& n) {m_name = n;}
  void setPath(const std::string& p) {m_path = p;}

private:

  // for a future more complete configuration with process units
  // and resources parameters
//  std::map<std::string, ProcessUnitPipelineConfiguration> m_processUnitPipelinesConfiguration;

//  std::map<std::string, ProcessUnitConfiguration> m_processUnitsConfigurations;

  std::string m_name;
  std::string m_path;

};

const char* neutralString(const std::string& str);
const char* neutralString(const char[]);

/// \brief writes a list element from a groupStructure in the referenced QXMLStreamWriter. Is supposed to be called within writeGroup.
void writeList(const std::string& name, const std::deque<std::string>& list, QXmlStreamWriter& xml);

/// \brief writes a map element from a groupStructure in the referenced QXMLStreamWriter. Is supposed to be called within writeGroup.
void writeMap(const std::string& name, const std::map<std::string, std::string>& map, QXmlStreamWriter& xml);

/// \brief writes a group element from a moduleStructure in the referenced QXMLStreamWriter. Is supposed to be called within writeModule.
void writeGroup(const Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& p_group, QXmlStreamWriter& xml);

/// \brief writes a group element from a moduleStructure in the referenced QXMLStreamWriter. Is supposed to be called within writeFile.
void writeModule(const Lima::Common::XMLConfigurationFiles::ModuleConfigurationStructure& p_module, QXmlStreamWriter& xml);

/// \brief with this function, you can write a ConfigurationStructure
/// to a new xml configuration file.
void writeFile(const Lima::Common::XMLConfigurationFiles::ConfigurationStructure& cstruct, const std::string& path);

#endif // LIMACONFIGURATION_H

} // Config
} // Gui
} // Lima
