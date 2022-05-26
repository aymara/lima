/*
    Copyright 2002-2013 CEA LIST

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
  * @file       xmlConfigurationFileParser.h
  * @brief      originally in detectlibraries
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>

  *             copyright (C) 2002-2003 by CEA
  */

#ifndef XMLCONFIGURATIONFILEPARSER_H
#define XMLCONFIGURATIONFILEPARSER_H

#include "common/LimaCommon.h"

#include <string>
#include <deque>

namespace Lima {
namespace Common {
namespace XMLConfigurationFiles {

class ConfigurationStructure;
class ModuleConfigurationStructure;
class GroupConfigurationStructure;

class XMLConfigurationFileParserPrivate;
/**
  * @brief Parser class for the lima's xml configuration files
  * @author Gael de Chalendar
  */
class LIMA_XMLCONFIGURATIONFILES_EXPORT XMLConfigurationFileParser
{
public:
  XMLConfigurationFileParser(const QString &configurationFileName);
//   /// @deprecated
//   XMLConfigurationFileParser(const std::string &configurationFileName);
  XMLConfigurationFileParser(const XMLConfigurationFileParser& p);
  ~XMLConfigurationFileParser();
  XMLConfigurationFileParser() = delete;
  XMLConfigurationFileParser& operator=(const XMLConfigurationFileParser& p) = delete;

    ConfigurationStructure& getConfiguration(void);

    /** @throw NoSuchModule*/
    ModuleConfigurationStructure& getModuleConfiguration(
      const std::string& moduleName);

    /** @throw NoSuchModule, @throw NoSuchGroup*/
    GroupConfigurationStructure& getModuleGroupConfiguration(
      const std::string& moduleName,
      const std::string& groupName);

    /** @throw NoSuchModule, @throw NoSuchGroup, @throw NoSuchParam */
    std::string& getModuleGroupParamValue(const std::string& moduleName,
                                          const std::string& groupName,
                                          const std::string& key);

    /** @throw NoSuchModule, @throw NoSuchGroup, @throw NoSuchList */
    std::deque< std::string >& getModuleGroupListValues(
      const std::string& moduleName,
      const std::string& groupName,
      const std::string& key);

    friend LIMA_XMLCONFIGURATIONFILES_EXPORT std::ostream& operator<<(
      std::ostream& os, XMLConfigurationFileParser& parser);

    const QString& getConfigurationFileName() const;

private:
  std::unique_ptr<XMLConfigurationFileParserPrivate> m_d;
};


} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima

#endif
