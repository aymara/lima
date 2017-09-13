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
  * @file       configurationStructure.cpp
  * @brief      originally detectConfigurationStructure.cpp in detectlibraries
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             copyright (C) 2002-2003 by CEA
  */

#include "common/LimaCommon.h"
#include "xmlConfigurationFileExceptions.h"
#include "configurationStructure.h"

#include <iostream>

using namespace std;

namespace Lima
{
namespace Common
{
namespace XMLConfigurationFiles
{

ConfigurationStructure::ConfigurationStructure() : std::map< std::string, ModuleConfigurationStructure >()
{}

ConfigurationStructure::ConfigurationStructure(const ConfigurationStructure& config) :
    std::map< std::string, ModuleConfigurationStructure >(config)
{
}

ConfigurationStructure& ConfigurationStructure::operator=(const ConfigurationStructure& config)
{
  std::map< std::string, ModuleConfigurationStructure >::operator=(config);
  return *this;
}

ConfigurationStructure::~ConfigurationStructure()
{}

void ConfigurationStructure::addParamValuePairForModuleAndGroup(const string &param,
    const string &value, const string &moduleName, const string & group)
{
  XMLCFGLOGINIT;
  iterator itC = find(moduleName);
  if (itC == end())
  {
    LERROR << "Error adding param '" << moduleName.c_str() << "': no such module !" ;
    throw NoSuchModule(moduleName+"["+ group+"]");
  }
  ModuleConfigurationStructure& moduleGroups = (*itC).second;
  moduleGroups.addParamValuePairForGroup(param, value, group);
//   erase(moduleName);
//   insert(make_pair(moduleName, moduleGroups));
}

void ConfigurationStructure::addListNamedForModuleAndGroup(const string &listName,
    const string &moduleName, const string & groupName)

{
  XMLCFGLOGINIT;
  iterator itC = find(moduleName);
  if (itC == end())
  {
    LERROR << "Error adding param '" << moduleName.c_str() << "': no such module !" ;
    throw NoSuchModule(moduleName+"["+ groupName+"]");
  }
  ModuleConfigurationStructure& moduleGroups = (*itC).second;
  moduleGroups.addListNamedForGroup(listName, groupName);
//   erase(moduleName);
//   insert(make_pair(moduleName, moduleGroups));
}

void ConfigurationStructure::addItemInListNamedForModuleAndGroup(const string& item,
    const string &listName, const string &moduleName, const string & groupName)
{
  XMLCFGLOGINIT;
  iterator itC = find(moduleName);
  if (itC == end())
  {
    LERROR << "Error adding param '" << moduleName.c_str() << "': no such module !" ;
    throw NoSuchModule(moduleName+"["+ groupName+"]["+listName+"]");
  }
  ModuleConfigurationStructure& moduleGroups = (*itC).second;
  moduleGroups.addItemInListNamedForGroup(item, listName, groupName);
//   erase(moduleName);
//   insert(make_pair(moduleName, moduleGroups));
}

void ConfigurationStructure::addGroupNamedForModuleNamed(const string& groupName,
    const string& moduleName)
{
  XMLCFGLOGINIT;
  iterator itC = find(moduleName);
  if (itC == end())
  {
    LERROR << "Error adding group '" << groupName.c_str() << "': no such module '" << moduleName.c_str() << "' !" ;
    throw NoSuchModule(moduleName);
  }
  ModuleConfigurationStructure& moduleGroups = (*itC).second;
  moduleGroups.addGroupNamed(groupName);
//   erase(moduleName);
//   insert(make_pair(moduleName, moduleGroups));
}

void ConfigurationStructure::addMapNamedForModuleAndGroup(const std::string &mapName, const std::string& moduleName, const std::string& groupName)
{
  XMLCFGLOGINIT;
  iterator itC = find(moduleName);
  if (itC == end())
  {
    LERROR << "Error adding map '" << mapName.c_str() << "' in group '" << groupName.c_str() << "': no such module '" << moduleName.c_str() << "' !" ;
    throw NoSuchModule(moduleName+"["+ groupName+"]");
  }
  itC->second.addMapInGroup(mapName,groupName);
}

void ConfigurationStructure::addEntryInMapNamedForModuleAndGroup(const std::string& entryKey,const std::string& entryValue,const std::string& mapName,const std::string& moduleName,const std::string& groupName)
{
  XMLCFGLOGINIT;
  iterator itC = find(moduleName);
  if (itC == end())
  {
    LERROR << "Error adding mapEntry in '" << mapName.c_str() << "' in group '" << groupName.c_str() << "': no such module '" << moduleName.c_str() << "' !" ;
    throw NoSuchModule(moduleName+"["+ groupName+"]["+mapName+"]");
  }
  itC->second.addEntryInMapInGroup(entryKey,entryValue,mapName,groupName);
}

void ConfigurationStructure::addAttributeForGroupInModule(const std::string& attKey,const std::string& attValue,const std::string& groupName,const std::string& moduleName)
{
  XMLCFGLOGINIT;
  iterator itC = find(moduleName);
  if (itC == end())
  {
    LERROR << "Error adding mapattribute in group '" << groupName.c_str() << "': no such module '" << moduleName.c_str() << "' !" ;
    throw NoSuchModule(moduleName+"["+ groupName+"]");
  }
#ifdef DEBUG_CD
  LDEBUG << "add attribute: module='"<<moduleName.c_str()<<"' group='"<<groupName.c_str()<<"' attribute='"<<attKey.c_str()<<"' value='"<<attValue.c_str()<<"'" ;
#endif
  itC->second.addAttributeInGroup(attKey,attValue,groupName);
}

GroupConfigurationStructure& ConfigurationStructure::
getGroupConf(const std::string& moduleName, 
             const std::string& group)
{
  iterator itC = find(moduleName);
  if (itC == end())
  {
    XMLCFGLOGINIT;
    LERROR << "'No such module '" << moduleName.c_str() << "'" ;
    throw NoSuchModule(moduleName+"["+ group +"]");
  }
  return (*itC).second.getGroupNamed(group) ;
}

void ConfigurationStructure::
addListOfItemsForModuleAndGroup(const std::string &listName, 
                                const std::string& moduleName, 
                                const std::string& group)
{
  getGroupConf(moduleName,group).addListOfItems(listName);
}

void ConfigurationStructure::
addItemInListOfItemsForModuleAndGroup(const ItemWithAttributes& item, 
                                      const std::string &listName, 
                                      const std::string& moduleName, 
                                      const std::string& group)
{
  getGroupConf(moduleName,group).addItemInListOfItems(listName,item);
}

void ConfigurationStructure::
addMapOfItemsForModuleAndGroup(const std::string &mapName, 
                               const std::string& moduleName, 
                               const std::string& group)
{
  getGroupConf(moduleName,group).addMapOfItems(mapName);
}

void ConfigurationStructure::
addEntryInMapOfItemsForModuleAndGroup(const std::string& key,
                                      const ItemWithAttributes& item,
                                      const std::string& mapName,
                                      const std::string& moduleName,
                                      const std::string& group)
{
  getGroupConf(moduleName,group).addEntryInMapOfItems(mapName,key,item);
}

std::ostream& operator<<(std::ostream &os, const ConfigurationStructure& conf)
{
  for (ConfigurationStructure::const_iterator it = conf.begin(); it != conf.end(); it++)
  {
    const ModuleConfigurationStructure& dmcs((*it).second);
    os << (*it).first << std::endl << dmcs;
  }
  return os;
}


void ConfigurationStructure::
changeListToListOfItems(const std::string &listName, 
                        const std::string& moduleName, 
                        const std::string& group)
{
  getGroupConf(moduleName,group).changeListToListOfItems(listName);
}

void ConfigurationStructure::
changeMapToMapOfItems(const std::string &mapName, 
                      const std::string& moduleName, 
                      const std::string& group)
{
  getGroupConf(moduleName,group).changeMapToMapOfItems(mapName);
}


} //closing namespace XMLConfigurationFiles
} //closing namespace Common
} //closing namespace Lima
