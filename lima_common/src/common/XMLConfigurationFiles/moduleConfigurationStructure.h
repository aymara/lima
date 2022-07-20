// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    ModuleConfigurationStructure(const QString& name);
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

    void addGroupNamed(const QString& group);
    void addParamValuePairForGroup(const QString& param, const QString& value, const QString& group);
    void addListNamedForGroup(const QString& listName, const QString& group);
    void addItemInListNamedForGroup(const QString& item, const QString& listName, const QString& group);
    void addMapInGroup(const QString& mapName,const QString& groupName);
    void addEntryInMapInGroup(const QString& entryKey,const QString& entryValue,const QString& mapName,const QString& groupName);
    void addAttributeInGroup(const QString& attKey,const QString& attValue,const QString& groupName);

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
