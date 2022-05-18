// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @file       configurationStructure.h
  * @brief      originally detectConfigurationStructure.h in detectlibraries
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             copyright (C) 2002-2003 by CEA
  */

#ifndef CONFIGURATIONSTRUCTURE_H
#define CONFIGURATIONSTRUCTURE_H

#include "common/LimaCommon.h"
#include "moduleConfigurationStructure.h"

#include <map>
#include <string>

namespace Lima {
namespace Common {
namespace XMLConfigurationFiles {


/**
  * The instances of this class hold configuration informations of the  modules
  * Its design is currently very simple. It should be improved in the future in
  * order to give a better API
  *
  * @author Gael de Chalendar
  */

class LIMA_XMLCONFIGURATIONFILES_EXPORT ConfigurationStructure : public std::map< std::string, ModuleConfigurationStructure >
{
public: 
    ConfigurationStructure();
    ~ConfigurationStructure();
   ConfigurationStructure(const ConfigurationStructure& config);
   ConfigurationStructure& operator=(const ConfigurationStructure& config);

    void addParamValuePairForModuleAndGroup(const std::string &param,const std::string &value, const std::string &module, const std::string & group);
    void addGroupNamedForModuleNamed(const std::string& groupName, const std::string& moduleName);
    void addListNamedForModuleAndGroup(const std::string &listName, const std::string& moduleName, const std::string& group);
    void addItemInListNamedForModuleAndGroup(const std::string& item, const std::string &listName, const std::string& moduleName, const std::string& group);
    void addMapNamedForModuleAndGroup(const std::string &mapName, const std::string& moduleName, const std::string& group);
    void addEntryInMapNamedForModuleAndGroup(const std::string& entryKey,const std::string& entryValue,const std::string& mapName,const std::string& moduleName,const std::string& groupName);
    void addAttributeForGroupInModule(const std::string& attKey,const std::string& attValue,const std::string& groupName,const std::string& moduleName);


    void addListOfItemsForModuleAndGroup(const std::string &listName, 
                                         const std::string& moduleName, 
                                         const std::string& group);
    void addItemInListOfItemsForModuleAndGroup(const ItemWithAttributes& item, 
                                               const std::string &listName, 
                                               const std::string& moduleName, 
                                               const std::string& group);
    void addMapOfItemsForModuleAndGroup(const std::string &mapName, 
                                        const std::string& moduleName, 
                                        const std::string& group);
    void addEntryInMapOfItemsForModuleAndGroup(const std::string& entryKey,
                                               const ItemWithAttributes& entryValue,
                                               const std::string& mapName,
                                               const std::string& moduleName,
                                               const std::string& groupName);

    void changeListToListOfItems(const std::string &listName, 
                                 const std::string& moduleName, 
                                 const std::string& group);
    void changeMapToMapOfItems(const std::string &mapName, 
                               const std::string& moduleName, 
                               const std::string& group);


 private:
   
    GroupConfigurationStructure& 
      getGroupConf(const std::string& moduleName, 
                   const std::string& group);

      friend LIMA_XMLCONFIGURATIONFILES_EXPORT std::ostream& operator<<(std::ostream &os, const ConfigurationStructure& conf);
};

} //closing namespace XMLConfigurationFiles
} //closing namespace Common
} //closing namespace Lima 


#endif
