/*
    Copyright 2002-2022 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
/**
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  */

#include "xmlConfigurationFileParser.h"

#include "common/LimaCommon.h"

#include "xmlConfigurationFileExceptions.h"
#include "xmlConfigurationFileReader.h"
#include "configurationStructure.h"

#include <QFile>
#include <iostream>
#include <memory>
#include <string>

namespace Lima {
namespace Common {
namespace XMLConfigurationFiles {

class XMLConfigurationFileParserPrivate
{
public:
  friend class std::unique_ptr<XMLConfigurationFileParserPrivate>;
  friend class XMLConfigurationFileParser;
  friend std::ostream& operator<<(std::ostream& os,
                                  XMLConfigurationFileParser& parser);

  XMLConfigurationFileParserPrivate(const QString &configurationFileName);
  XMLConfigurationFileParserPrivate(const XMLConfigurationFileParserPrivate& config);
  ~XMLConfigurationFileParserPrivate() = default;
  XMLConfigurationFileParserPrivate() = delete;

  ConfigurationStructure m_configuration;
  XmlConfigurationFileReader m_parser;
  QString m_configurationFileName;
};

XMLConfigurationFileParserPrivate::XMLConfigurationFileParserPrivate(
  const XMLConfigurationFileParserPrivate& configuration) :
    m_configuration(configuration.m_configuration),
    m_parser(m_configuration),
    m_configurationFileName(configuration.m_configurationFileName)
{
}

XMLConfigurationFileParserPrivate::XMLConfigurationFileParserPrivate(
  const QString &configurationFileName) :
    m_configuration(),
    m_parser(m_configuration),
    m_configurationFileName(configurationFileName)
{
    XMLCFGLOGINIT;
    LINFO << "XMLConfigurationFileParser creating parser for: "
          << configurationFileName;

    QFile file(m_configurationFileName);
    if (!file.open(QFile::ReadOnly))
    {
      XMLCFGLOGINIT;
      LERROR << "Error opening " << m_configurationFileName;
      throw std::runtime_error(std::string("XMLConfigurationFileParser Unable to open ")
          + m_configurationFileName.toUtf8().constData());
    }
    if (!m_parser.parse(&file))
    {
      LERROR << "Error parsing " << m_configurationFileName;
      throw XMLException(std::string("XMLConfigurationFileParser Unable to parse ")
          + m_configurationFileName.toUtf8().constData() + " : "
          + m_parser.errorString().toUtf8().constData());
    }
    {
      LOGINIT("FilesReporting");
      LINFO << "File parsed:" << m_configurationFileName;
    }

}

// XMLConfigurationFileParser::XMLConfigurationFileParser() :
//     m_d(new XMLConfigurationFileParserPrivate())
// {
// }

XMLConfigurationFileParser::XMLConfigurationFileParser(const XMLConfigurationFileParser& p) :
    m_d(new XMLConfigurationFileParserPrivate(*p.m_d))
{
}

// XMLConfigurationFileParser::XMLConfigurationFileParser(
//   const std::string &configurationFileName) :
//     m_d(std::make_unique<XMLConfigurationFileParserPrivate>(
//       QString::fromUtf8(configurationFileName.c_str())))
// {
// }

XMLConfigurationFileParser::XMLConfigurationFileParser(
  const QString &configurationFileName) :
    m_d(std::make_unique<XMLConfigurationFileParserPrivate>(
      configurationFileName))
{
}

XMLConfigurationFileParser::~XMLConfigurationFileParser()
{
}

ConfigurationStructure& XMLConfigurationFileParser::getConfiguration(void)
{
    return m_d->m_configuration;
}

ModuleConfigurationStructure& XMLConfigurationFileParser::getModuleConfiguration(
  const std::string& moduleName)
{
  auto it = m_d->m_configuration.find(moduleName);

  if (it == m_d->m_configuration.end())
  {
    XMLCFGLOGINIT;
    LDEBUG << "XMLConfigurationFileParser::getModuleConfiguration no such module"
            << moduleName << "in" << m_d->m_configurationFileName;
    throw NoSuchModule(std::string(m_d->m_configurationFileName.toStdString())+":["+moduleName+"]");
  }
  return (*it).second;
}

GroupConfigurationStructure& XMLConfigurationFileParser::getModuleGroupConfiguration(
    const std::string& moduleName,const std::string& groupName)
{
  return (getModuleConfiguration(moduleName).getGroupNamed(groupName));
}

std::string& XMLConfigurationFileParser::getModuleGroupParamValue(
    const std::string& moduleName,
    const std::string& groupName,
    const std::string& key)
{
  XMLCFGLOGINIT;
  try
  {
    return (getModuleConfiguration(moduleName).getParamValueAtKeyOfGroupNamed(key, groupName));
  }
  catch(NoSuchModule& nsm)
  {
    //std::cerr << nsm.what() << " "
    //          << m_d->m_configurationFileName.toUtf8().constData() << std::endl;
    LWARN << nsm.what() << " " << m_d->m_configurationFileName;
      //not LERROR because user may want the module to be optional -> no error
      throw NoSuchModule(std::string(m_d->m_configurationFileName.toUtf8().constData())
              +":["+moduleName
              +"]["+groupName+"]["+key+"]");
  }
  catch(NoSuchGroup& nsg)
  {
    //std::cerr << nsg.what() << " "
    //          << m_d->m_configurationFileName.toUtf8().constData() << std::endl;
    LWARN << nsg.what() << " " << m_d->m_configurationFileName;
      throw NoSuchGroup(std::string(m_d->m_configurationFileName.toUtf8().constData())
              +":["+moduleName+"]["
              +groupName+"]["+key+"]");
  }
  catch(NoSuchParam& nsp)
  {
    //std::cerr << nsp.what() << " "
    //          << m_d->m_configurationFileName.toUtf8().constData() << std::endl;
    LWARN << nsp.what() << " " << m_d->m_configurationFileName;
      throw NoSuchParam(std::string(m_d->m_configurationFileName.toUtf8().constData())+":["
              +moduleName+"]["+groupName+"]["+key+"]");
  }
  catch(...)
  {
      throw;
  }
}

std::deque< std::string >& XMLConfigurationFileParser::getModuleGroupListValues(
    const std::string& moduleName,
    const std::string& groupName,
    const std::string& key)
{
  return (getModuleConfiguration(moduleName).getListValuesAtKeyOfGroupNamed(
            key, groupName));
}

const QString& XMLConfigurationFileParser::getConfigurationFileName() const
{
  return m_d->m_configurationFileName;
}

std::ostream& operator<<(std::ostream& os, XMLConfigurationFileParser& parser)
{
    return (os << parser.m_d->m_configurationFileName.toUtf8().constData()
                << ":" << parser.m_d->m_configuration);
}

} // closing namespace XMLConfigurationFiles
} // closing namesapce Common
} // closing namespace Lima
