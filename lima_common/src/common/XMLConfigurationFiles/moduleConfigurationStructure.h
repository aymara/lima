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
  * @file       moduleConfigurationStructure.h
  * @brief      originally detectModuleConfigurationStructure.h in detectlibraries
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             copyright (C) 2002-2003 by CEA
  */


#ifndef MODULECONFIGURATIONSTRUCTURE_H
#define MODULECONFIGURATIONSTRUCTURE_H

#include "common/LimaCommon.h"
#include "groupConfigurationStructure.h"

#include <map>
#include <deque>
#include <string>

namespace Lima {
namespace Common {
namespace XMLConfigurationFiles {

  class GroupConfigurationStructure;
/**
  * @brief  The objects of this class hold the configuration of one  module
  *         in the form of a list of group names associated to lists of key/value
  *         pairs
  * @author Gael de Chalendar
  */
class ModuleConfigurationStructurePrivate;
class LIMA_XMLCONFIGURATIONFILES_EXPORT ModuleConfigurationStructure : public std::map< std::string, GroupConfigurationStructure >
{
public:
    ModuleConfigurationStructure();
    ModuleConfigurationStructure(const std::string& name);
    ModuleConfigurationStructure(const ModuleConfigurationStructure& mod);
    ModuleConfigurationStructure& operator=(const ModuleConfigurationStructure& mod);
    virtual ~ModuleConfigurationStructure();

    const std::string& getName() const;
    
    GroupConfigurationStructure& getGroupNamed(const std::string& name) ;
    std::string& getParamValueAtKeyOfGroupNamed(const std::string& key, const std::string& name) ;
    std::deque<std::string>& getListValuesAtKeyOfGroupNamed(const std::string& key, const std::string& name) ;
    std::map<std::string,std::string>& getMapAtKeyOfGroupNamed(const std::string& key, const std::string& groupName);

    void addGroupNamed(const std::string& group);
    void addParamValuePairForGroup(const std::string& param, const std::string& value, const std::string& group);
    void addListNamedForGroup(const std::string& listName, const std::string& group);
    void addItemInListNamedForGroup(const std::string& item, const std::string& listName, const std::string& group);
    void addMapInGroup(const std::string& mapName,const std::string& groupName);
    void addEntryInMapInGroup(const std::string& entryKey,const std::string& entryValue,const std::string& mapName,const std::string& groupName);
    void addAttributeInGroup(const std::string& attKey,const std::string& attValue,const std::string& groupName);

    void addModule(const ModuleConfigurationStructure& mod);

    friend LIMA_XMLCONFIGURATIONFILES_EXPORT std::ostream& operator<<(std::ostream &os, const ModuleConfigurationStructure& dmcs);

private :
  ModuleConfigurationStructurePrivate* m_d;
};

LIMA_XMLCONFIGURATIONFILES_EXPORT std::ostream& operator<<(std::ostream &os, const ModuleConfigurationStructure& dmcs);

} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima

#endif
