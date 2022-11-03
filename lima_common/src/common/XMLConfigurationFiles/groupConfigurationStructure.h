// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @file       groupConfigurationStructure.h
  * @brief      originally detectGroupConfigurationStructure.h in detectlibraries
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>

  *             copyright (C) 2002-2003 by CEA
  */


#ifndef GROUPCONFIGURATIONSTRUCTURE_H
#define GROUPCONFIGURATIONSTRUCTURE_H

#include "common/LimaCommon.h"
#include "xmlConfigurationFileExceptions.h"
#include "itemWithAttributes.h"
#include <map>
#include <deque>
#include <string>

namespace Lima {
namespace Common {
namespace XMLConfigurationFiles {

class GroupConfigurationStructurePrivate;
class LIMA_XMLCONFIGURATIONFILES_EXPORT GroupConfigurationStructure {
public:
    GroupConfigurationStructure();
    GroupConfigurationStructure(const std::string& name);
    GroupConfigurationStructure(const QString& name);
    GroupConfigurationStructure(const GroupConfigurationStructure& group);
    GroupConfigurationStructure& operator=(const GroupConfigurationStructure& group);
    virtual ~GroupConfigurationStructure();

    std::string& getName();
    std::string& getAttribute(const std::string& key);
    std::string& getParamsValueAtKey(const std::string& key);
    bool getParamsValueAtKey(const std::string& key, std::string& value);
    std::deque< std::string >& getListsValueAtKey(const std::string& key);
    std::map<std::string,std::string>& getMapAtKey(const std::string& key);
    const std::map<std::string,std::string>& getParams();
    const std::map<std::string,std::deque<std::string> >& getLists();
    const std::map<std::string,std::map<std::string,std::string> >& getMaps();
    void addAttribute(const std::string& key,const std::string& value);
    void addParamValuePair(const std::string& key, const std::string& value);
    void addListNamed(const std::string& listName);
    void addItemInListNamed(const std::string& item, const std::string& listName);
    void addMap(const std::string& mapName);
    void addEntryInMap(const std::string& mapName,const std::string& key,const std::string& value);

    std::deque<ItemWithAttributes>& getListOfItems(const std::string& key);
    std::map<std::string,ItemWithAttributes>& getMapOfItems(const std::string& key);
    void addListOfItems(const std::string& listName);
    void addItemInListOfItems(const std::string& listName,
                              const ItemWithAttributes& item);
    void addMapOfItems(const std::string& mapName);
    void addEntryInMapOfItems(const std::string& mapName,
                              const std::string& key,
                              const ItemWithAttributes& item);

    void changeListToListOfItems(const std::string &listName);
    void changeMapToMapOfItems(const std::string &mapName);

//     QString getNameQ();
//     QString getAttribute(const QString& key);
//     QString getParamsValueAtKey(const QString& key);
//     bool getParamsValueAtKey(const QString& key, QString& value);
//     std::deque< std::string >& getListsValueAtKey(const QString& key);
//     std::map<std::string, std::string>& getMapAtKey(const QString& key);
//     void addAttribute(const QString& key,const QString& value);
//     void addParamValuePair(const QString& key, const QString& value);
//     void addListNamed(const QString& listName);
//     void addItemInListNamed(const QString& item, const QString& listName);
//     void addMap(const QString& mapName);
//     void addEntryInMap(const QString& mapName,const QString& key,const QString& value);
//
//     std::deque<ItemWithAttributes>& getListOfItems(const QString& key);
//     std::map<std::string, ItemWithAttributes>& getMapOfItems(const QString& key);
//     void addListOfItems(const QString& listName);
//     void addItemInListOfItems(const QString& listName,
//                               const ItemWithAttributes& item);
//     void addMapOfItems(const QString& mapName);
//     void addEntryInMapOfItems(const QString& mapName,
//                               const QString& key,
//                               const ItemWithAttributes& item);
//
//     void changeListToListOfItems(const QString &listName);
//     void changeMapToMapOfItems(const QString &mapName);

    friend LIMA_XMLCONFIGURATIONFILES_EXPORT std::ostream& operator<<(std::ostream& os, const GroupConfigurationStructure& dgcs);

    // functions to access typed values
    bool getBooleanParameter(const std::string& key);
    uint32_t getIntParameter(const std::string& key);
    double getDoubleParameter(const std::string& key);
    std::string getStringParameter(const std::string& key);

//     bool getBooleanParameter(const QString& key);
//     uint32_t getIntParameter(const QString& key);
//     double getDoubleParameter(const QString& key);
//     QString getStringParameter(const QString& key);

private:
  GroupConfigurationStructurePrivate* m_d;
};

LIMA_XMLCONFIGURATIONFILES_EXPORT std::ostream& operator<<(std::ostream& os, const GroupConfigurationStructure& dgcs);

} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima

#endif
