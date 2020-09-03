/*
    Copyright 2002-2020 CEA LIST

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


    friend LIMA_XMLCONFIGURATIONFILES_EXPORT std::ostream& operator<<(std::ostream& os, const GroupConfigurationStructure& dgcs);

    // functions to access typed values
    bool getBooleanParameter(const std::string& key);
    uint32_t getIntParameter(const std::string& key);
    double getDoubleParameter(const std::string& key);
    std::string getStringParameter(const std::string& key);

private:
  GroupConfigurationStructurePrivate* m_d;
};

LIMA_XMLCONFIGURATIONFILES_EXPORT std::ostream& operator<<(std::ostream& os, const GroupConfigurationStructure& dgcs);

} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima

#endif
