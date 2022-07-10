// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  ModuleConfigurationStructurePrivate& operator=(const ModuleConfigurationStructurePrivate& mod);
  ~ModuleConfigurationStructurePrivate();

  std::string m_name;
};

ModuleConfigurationStructurePrivate::ModuleConfigurationStructurePrivate(const std::string& name) :
    m_name(name)
{}

ModuleConfigurationStructurePrivate::ModuleConfigurationStructurePrivate(const ModuleConfigurationStructurePrivate& mod) :
    m_name(mod.m_name)
{}

ModuleConfigurationStructurePrivate& ModuleConfigurationStructurePrivate::operator=(const ModuleConfigurationStructurePrivate& mod)
{
  m_name = mod.m_name;
  return *this;
}

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

ModuleConfigurationStructure::ModuleConfigurationStructure(const QString& name) :
    std::map< std::string, GroupConfigurationStructure >(),
    m_d(new ModuleConfigurationStructurePrivate(name.toStdString()))
{}

ModuleConfigurationStructure::ModuleConfigurationStructure(const ModuleConfigurationStructure& mod) :
    std::map< std::string, GroupConfigurationStructure >(mod),
    m_d(new ModuleConfigurationStructurePrivate(*mod.m_d))
{
#ifdef DEBUG_CD
  XMLCFGLOGINIT;
  LDEBUG << "ModuleConfigurationStructure::ModuleConfigurationStructure" << this << m_d->m_name;
#endif

}

ModuleConfigurationStructure& ModuleConfigurationStructure::operator=(const ModuleConfigurationStructure& mod)
{
  clear();
  for (auto it = mod.cbegin(); it != mod.cend(); it++)
  {
    insert(std::make_pair((*it).first, (*it).second));
  }
  *m_d = *mod.m_d;
#ifdef DEBUG_CD
  XMLCFGLOGINIT;
  LDEBUG << "ModuleConfigurationStructure::operator=" << this << m_d->m_name;
#endif
  return *this;
}

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
  ModuleConfigurationStructure::iterator it = find(name);
  if (it == end())
  {
    XMLCFGLOGINIT;
    LWARN << "Error ("<<this<<"): no such group '" << name.c_str() << "' !";
    throw NoSuchGroup(m_d->m_name+"["+name+"]");
  }
  return ((*it).second);
}

string& ModuleConfigurationStructure::getParamValueAtKeyOfGroupNamed(const std::string& key, const std::string& groupName)
{
  try
  {
    return (getGroupNamed(groupName).getParamsValueAtKey(key));
  }
  catch (NoSuchGroup& nsg)
  {
    XMLCFGLOGINIT;
    LWARN << "Getting param '"<<key.c_str()<<"' value for group '"<<groupName.c_str()<<"': "<< nsg.what();
    throw NoSuchGroup(m_d->m_name+"["+groupName+"]["+key+"]");
  }
  catch (NoSuchParam& nsp)
  {
    XMLCFGLOGINIT;
    LWARN << "Getting param '"<<key.c_str()<<"' value for group '"<<groupName.c_str()<<"': " << nsp.what();
    throw NoSuchParam(m_d->m_name+"["+groupName+"]["+key+"]");
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
#ifdef DEBUG_CD
  XMLCFGLOGINIT;
  LDEBUG << "ModuleConfigurationStructure::addGroupNamed" << this << groupName;
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
  for (auto it = mod.cbegin(), it_end = mod.cend(); it!=it_end; it++)
  {
    if (find((*it).first) == end())
    {
      insert(*it);
    }
    else
    {
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


void ModuleConfigurationStructure::addGroupNamed(const QString& group)
{
  addGroupNamed(group.toStdString());
}

void ModuleConfigurationStructure::addParamValuePairForGroup(const QString& param, const QString& value, const QString& group)
{
  addParamValuePairForGroup(param.toStdString(), value.toStdString(), group.toStdString());
}

void ModuleConfigurationStructure::addListNamedForGroup(const QString& listName, const QString& group)
{
  addListNamedForGroup(listName.toStdString(), group.toStdString());
}

void ModuleConfigurationStructure::addItemInListNamedForGroup(const QString& item, const QString& listName, const QString& group)
{
  addItemInListNamedForGroup(item.toStdString(), listName.toStdString(), group.toStdString());
}

void ModuleConfigurationStructure::addMapInGroup(const QString& mapName,const QString& groupName)
{
  addMapInGroup(mapName.toStdString(), groupName.toStdString());
}

void ModuleConfigurationStructure::addEntryInMapInGroup(const QString& entryKey, const QString& entryValue,
                                                        const QString& mapName, const QString& groupName)
{
  addEntryInMapInGroup(entryKey.toStdString(), entryValue.toStdString(),
                       mapName.toStdString(), groupName.toStdString());
}

void ModuleConfigurationStructure::addAttributeInGroup(const QString& attKey, const QString& attValue,
                                                       const QString& groupName)
{
  addAttributeInGroup(attKey.toStdString(), attValue.toStdString(), groupName.toStdString());
}

} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima
