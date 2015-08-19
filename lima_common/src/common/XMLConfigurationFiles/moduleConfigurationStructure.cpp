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
  * @file       moduleConfigurationStructure.cpp
  * @brief      originally detectModuleConfigurationStructure.h in detectlibraries
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             copyright (C) 2002-2003 by CEA
  */


#include "common/LimaCommon.h"
#include "xmlConfigurationFileExceptions.h"
#include "moduleConfigurationStructure.h"

#include <iostream>

using namespace std;

namespace Lima
{
namespace Common
{
namespace XMLConfigurationFiles
{

class ModuleConfigurationStructurePrivate
{
  friend class ModuleConfigurationStructure;
  
  ModuleConfigurationStructurePrivate(const std::string& name);
  ModuleConfigurationStructurePrivate(const ModuleConfigurationStructurePrivate& mod);
  virtual ~ModuleConfigurationStructurePrivate();

  std::string m_name;
};

ModuleConfigurationStructurePrivate::ModuleConfigurationStructurePrivate(const std::string& name) :
    m_name(name)
{}

ModuleConfigurationStructurePrivate::ModuleConfigurationStructurePrivate(const ModuleConfigurationStructurePrivate& mod) :
    m_name(mod.m_name)
{}

ModuleConfigurationStructurePrivate::~ModuleConfigurationStructurePrivate()
{
}

ModuleConfigurationStructure::ModuleConfigurationStructure() :
    std::map< std::string, GroupConfigurationStructure >(),
    m_d(new ModuleConfigurationStructurePrivate(""))
{}

ModuleConfigurationStructure::ModuleConfigurationStructure(const std::string& name) :
    std::map< std::string, GroupConfigurationStructure >(),
    m_d(new ModuleConfigurationStructurePrivate(name))
{}

ModuleConfigurationStructure::ModuleConfigurationStructure(const ModuleConfigurationStructure& mod) :
    std::map< std::string, GroupConfigurationStructure >(mod),
    m_d(new ModuleConfigurationStructurePrivate(*mod.m_d))
{}

ModuleConfigurationStructure::~ModuleConfigurationStructure()
{
  delete m_d;
}

const std::string& ModuleConfigurationStructure::getName() const
{
  return m_d->m_name;
}

GroupConfigurationStructure& ModuleConfigurationStructure::getGroupNamed(const std::string& name)
{
  XMLCFGLOGINIT;
  ModuleConfigurationStructure::iterator it = find(name);
  if (it == end())
  {
    LWARN << "Error ("<<this<<"): no such group '" << name.c_str() << "' !";
    throw NoSuchGroup(name);
  }
  return ((*it).second);
}

string& ModuleConfigurationStructure::getParamValueAtKeyOfGroupNamed(const std::string& key, const std::string& groupName)
{
  XMLCFGLOGINIT;
  try
  {
    return (getGroupNamed(groupName).getParamsValueAtKey(key));
  }
  catch (NoSuchGroup& nsg)
  {
    LWARN << "Getting param '"<<key.c_str()<<"' value for group '"<<groupName.c_str()<<"': "<< nsg.what().c_str();
    throw;
  }
  catch (NoSuchParam& nsp)
  {
    LWARN << "Getting param '"<<key.c_str()<<"' value for group '"<<groupName.c_str() << nsp.what().c_str();
    throw;
  }
  catch (...)
  {
    throw;
  }
}

deque< string >& ModuleConfigurationStructure::getListValuesAtKeyOfGroupNamed(const std::string& key, const std::string& groupName)
{
  return (getGroupNamed(groupName).getListsValueAtKey(key));
}


map< string , string >& ModuleConfigurationStructure::getMapAtKeyOfGroupNamed(const std::string& key, const std::string& groupName)
{
  return (getGroupNamed(groupName).getMapAtKey(key));
}


void ModuleConfigurationStructure::addParamValuePairForGroup(const std::string& param,
    const std::string& value, const std::string& groupName)
{
  getGroupNamed(groupName).addParamValuePair(param, value);
}


void ModuleConfigurationStructure::addListNamedForGroup(const std::string& listName,
    const std::string& groupName)
{
  getGroupNamed(groupName).addListNamed(listName);
}

void ModuleConfigurationStructure::addItemInListNamedForGroup(const std::string& item,
    const std::string& listName, const std::string& groupName)
{
  getGroupNamed(groupName).addItemInListNamed(item, listName);
}

void ModuleConfigurationStructure::addGroupNamed(const string& groupName)
{
  XMLCFGLOGINIT;
#ifdef DEBUG_CD
  LDEBUG << "ModuleConfigurationStructure::addGroupNamed " << this << " " << groupName.c_str();
#endif
  if (find(groupName) == end())
  {
    insert(make_pair(groupName, GroupConfigurationStructure(groupName)));
  }
  else {
    XMLCFGLOGINIT;
    LWARN << "group " << groupName.c_str() << " not added: already exists";
  }
}

void ModuleConfigurationStructure::addMapInGroup(
  const std::string& mapName,const std::string& groupName)
{
  getGroupNamed(groupName).addMap(mapName);
}

void ModuleConfigurationStructure::addEntryInMapInGroup(
  const std::string& entryKey,const std::string& entryValue,const std::string& mapName,const std::string& groupName)
{
  getGroupNamed(groupName).addEntryInMap(mapName,entryKey,entryValue);
}

void ModuleConfigurationStructure::addAttributeInGroup(
  const std::string& attKey,const std::string& attValue,const std::string& groupName)
{
  getGroupNamed(groupName).addAttribute(attKey,attValue);
}

void ModuleConfigurationStructure::addModule(const ModuleConfigurationStructure& mod)
{
  for (ModuleConfigurationStructure::const_iterator it=mod.begin(),
         it_end=mod.end(); it!=it_end; it++) {
    if (find((*it).first) == end()) {
      (*this)[(*it).first]=(*it).second;
    }
    else {
      XMLCFGLOGINIT;
      LWARN << "group " << (*it).first.c_str() << " not added: already exists";
    }
  }
}


std::ostream& operator<<(std::ostream &os, const ModuleConfigurationStructure& dmcs)
{
  for (ModuleConfigurationStructure::const_iterator it = dmcs.begin(); it != dmcs.end(); it++)
  {
    os << "  " << (*it).first << endl;
    os << ((*it).second);
  }
  return os;
}


} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima
