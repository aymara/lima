// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

void ConfigurationStructure::addParamValuePairForModuleAndGroup(const QString& param, const QString& value,
                                                                const QString& moduleName, const QString& group)
{
  addParamValuePairForModuleAndGroup(param.toStdString(), value.toStdString(),
                                     moduleName.toStdString(), group.toStdString());
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

void ConfigurationStructure::addListNamedForModuleAndGroup(const QString& listName, const QString& moduleName,
                                                           const QString& groupName)

{
  addListNamedForModuleAndGroup(listName.toStdString(), moduleName.toStdString(), groupName.toStdString());
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

void ConfigurationStructure::addItemInListNamedForModuleAndGroup(const QString& item, const QString& listName,
                                                                 const QString& moduleName, const QString& groupName)
{
  addItemInListNamedForModuleAndGroup(item.toStdString(), listName.toStdString(),
                                      moduleName.toStdString(), groupName.toStdString());
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

void ConfigurationStructure::addGroupNamedForModuleNamed(const QString& groupName, const QString& moduleName)
{
  addGroupNamedForModuleNamed(groupName.toStdString(), moduleName.toStdString());
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

void ConfigurationStructure::addMapNamedForModuleAndGroup(const QString& mapName, const QString& moduleName,
                                                          const QString& groupName)
{
  addMapNamedForModuleAndGroup(mapName.toStdString(), moduleName.toStdString(), groupName.toStdString());
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

void ConfigurationStructure::addEntryInMapNamedForModuleAndGroup(const QString& entryKey, const QString& entryValue,
                                                                 const QString& mapName, const QString& moduleName,
                                                                 const QString& groupName)
{
  addEntryInMapNamedForModuleAndGroup(entryKey.toStdString(), entryValue.toStdString(),
                                      mapName.toStdString(), moduleName.toStdString(),
                                      groupName.toStdString());
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

void ConfigurationStructure::addAttributeForGroupInModule(const QString& attKey, const QString& attValue,
                                                          const QString& groupName,const QString& moduleName)
{
  addAttributeForGroupInModule(attKey.toStdString(), attValue.toStdString(),
                               groupName.toStdString(), moduleName.toStdString());
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

std::ostream& operator<<(std::ostream &os, const ConfigurationStructure& conf)
{
  for (ConfigurationStructure::const_iterator it = conf.begin(); it != conf.end(); it++)
  {
    const ModuleConfigurationStructure& dmcs((*it).second);
    os << (*it).first << std::endl << dmcs;
  }
  return os;
}

} //closing namespace XMLConfigurationFiles
} //closing namespace Common
} //closing namespace Lima
