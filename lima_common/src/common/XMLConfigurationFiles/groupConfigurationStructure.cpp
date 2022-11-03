// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @file       groupConfigurationStructure.cpp
  * @brief      originally detectGroupConfigurationStructure.cpp in detectlibraries
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>

  *             copyright (C) 2002-2003 by CEA
  */

#include "common/LimaCommon.h"

#include "xmlConfigurationFileExceptions.h"
#include "groupConfigurationStructure.h"

#include <iostream>
#include <stdlib.h>


using namespace std;

namespace Lima
{
namespace Common
{
namespace XMLConfigurationFiles
{

/**The objects of this class hold the configuration of one  module
in the form of a list of group names associated to lists of key/value pairs
*@author Gael de Chalendar
*/
typedef std::map< std::string, std::string > MSS;
std::ostream& operator<<(std::ostream& os, const MSS& mss);

typedef std::map< std::string, std::deque<std::string > > MSDS;
std::ostream& operator<<(std::ostream& os, const MSDS& mss);

typedef std::map< std::string, std::deque<ItemWithAttributes> > MSDI;
std::ostream& operator<<(std::ostream& os, const MSDS& mss);

typedef std::map< std::string, std::map<std::string,std::string > > MSMSS;
std::ostream& operator<<(std::ostream& os, const MSMSS& msmss);

typedef std::map< std::string, std::map<std::string,ItemWithAttributes> > MSMSI;
std::ostream& operator<<(std::ostream& os, const MSMSS& msmss);


class GroupConfigurationStructurePrivate
{
  friend class GroupConfigurationStructure;
  friend std::ostream& operator<<(std::ostream& os, const GroupConfigurationStructure& dgcs);

  GroupConfigurationStructurePrivate() {}
  GroupConfigurationStructurePrivate(const std::string& name);
  GroupConfigurationStructurePrivate(const GroupConfigurationStructurePrivate& group);
  GroupConfigurationStructurePrivate& operator=(const GroupConfigurationStructurePrivate& group);
  ~GroupConfigurationStructurePrivate();

  MSS m_params;
  MSDS m_lists;
  MSS m_attributes;
  MSMSS m_maps;
  MSDI m_listsOfItems;
  MSMSI m_mapsOfItems;
  std::string m_groupName;

  friend class XMLConfigurationFileHandler;
};

GroupConfigurationStructurePrivate::GroupConfigurationStructurePrivate(const std::string& name) :
    m_params(),
    m_lists(),
    m_attributes(),
    m_maps(),
    m_listsOfItems(),
    m_mapsOfItems(),
    m_groupName(name)
{}

GroupConfigurationStructurePrivate::GroupConfigurationStructurePrivate(const GroupConfigurationStructurePrivate& group) :
    m_params(group.m_params),
    m_lists(group.m_lists),
    m_attributes(group.m_attributes),
    m_maps(group.m_maps),
    m_listsOfItems(group.m_listsOfItems),
    m_mapsOfItems(group.m_mapsOfItems),
    m_groupName(group.m_groupName)
{}

GroupConfigurationStructurePrivate& GroupConfigurationStructurePrivate::operator=(const GroupConfigurationStructurePrivate& group)
{
    m_params=group.m_params;
    m_lists=group.m_lists;
    m_attributes=group.m_attributes;
    m_maps=group.m_maps;
    m_listsOfItems=group.m_listsOfItems;
    m_mapsOfItems=group.m_mapsOfItems;
    m_groupName=group.m_groupName;
    return *this;
}

GroupConfigurationStructurePrivate::~GroupConfigurationStructurePrivate()
{

}


GroupConfigurationStructure::GroupConfigurationStructure() : m_d(new GroupConfigurationStructurePrivate())
{

}

GroupConfigurationStructure::GroupConfigurationStructure(const std::string& name) :
  m_d(new GroupConfigurationStructurePrivate(name))
{}

GroupConfigurationStructure::GroupConfigurationStructure(const GroupConfigurationStructure& group) :
  m_d(new GroupConfigurationStructurePrivate(*group.m_d))
{}

GroupConfigurationStructure& GroupConfigurationStructure::operator=(const GroupConfigurationStructure& group)
{
  *m_d = *group.m_d;
  return *this;
}

GroupConfigurationStructure::~GroupConfigurationStructure()
{
  delete m_d;
}

std::string& GroupConfigurationStructure::getName()
{
  return m_d->m_groupName;
}


std::string& GroupConfigurationStructure::getAttribute(const std::string& key)
{
  MSS::iterator it=m_d->m_attributes.find(key);
  if (it == m_d->m_attributes.end())
    throw NoSuchAttribute(m_d->m_groupName+"["+key+"]");
  return (it->second);
}

string& GroupConfigurationStructure::getParamsValueAtKey(const string& key)
{
  MSS::iterator it = m_d->m_params.find(key);
  if (it == m_d->m_params.end())
    throw NoSuchParam(m_d->m_groupName+"["+key+"]");
  //    cerr << "Found param value " << ((*it).second) << endl;
  return ((*it).second);
}

bool GroupConfigurationStructure::getParamsValueAtKey(const string& key, string& value)
{
  MSS::iterator it = m_d->m_params.find(key);
  if (it == m_d->m_params.end())
  {
    return false;
  }
  value = it->second;
  return true;
}

// QString GroupConfigurationStructure::getNameQ()
// {
//   return QString::fromStdString(m_d->m_groupName);
// }
//
//
// QString GroupConfigurationStructure::getAttribute(const QString& key)
// {
//   return QString::fromStdString(getAttribute(key.toStdString()));
// }
//
// QString GroupConfigurationStructure::getParamsValueAtKey(const QString& key)
// {
//   return QString::fromStdString(getParamsValueAtKey(key.toStdString()));
// }
//
// bool GroupConfigurationStructure::getParamsValueAtKey(const QString& key, QString& value)
// {
//   std::string svalue;
//   if (!getParamsValueAtKey(key.toStdString(), svalue))
//   {
//     return false;
//   }
//   value = QString::fromStdString(svalue);
//   return true;
// }

bool GroupConfigurationStructure::getBooleanParameter(const std::string& key)
{
  string& str=getParamsValueAtKey(key);
  if (str=="yes" || str=="true" || str=="1") {
    return true;
  }
  return false;
}

uint32_t GroupConfigurationStructure::getIntParameter(const std::string& key)
{
  return atoi(getParamsValueAtKey(key).c_str());
}

double GroupConfigurationStructure::getDoubleParameter(const std::string& key)
{
  return QString::fromUtf8(getParamsValueAtKey(key).c_str()).toDouble();
}

std::string GroupConfigurationStructure::getStringParameter(const std::string& key)
{
  return getParamsValueAtKey(key);
}

// bool GroupConfigurationStructure::getBooleanParameter(const QString& key)
// {
//   auto& str = getParamsValueAtKey(key);
//   if (str=="yes" || str=="true" || str=="1")
//   {
//     return true;
//   }
//   return false;
// }
//
// uint32_t GroupConfigurationStructure::getIntParameter(const QString& key)
// {
//   return getParamsValueAtKey(key).toUInt();
// }
//
// double GroupConfigurationStructure::getDoubleParameter(const QString& key)
// {
//   return getParamsValueAtKey(key).toDouble();
// }
//
// QString GroupConfigurationStructure::getStringParameter(const QString& key)
// {
//   return getParamsValueAtKey(key);
// }

std::deque<std::string>& GroupConfigurationStructure::getListsValueAtKey(const std::string& key)
{
  auto it = m_d->m_lists.find(key);
  if (it == m_d->m_lists.end())
    throw NoSuchList(m_d->m_groupName+"["+key+"]");
  return (*it).second;
}

std::map<std::string, std::string>& GroupConfigurationStructure::getMapAtKey(const std::string& key)
{
  MSMSS::iterator it = m_d->m_maps.find(key);
  if (it == m_d->m_maps.end())
  {
    XMLCFGLOGINIT;
    LWARN << "GroupConfigurationStructure::getMapAtKey No such map"
          << m_d->m_groupName+"["+key+"]";
    throw NoSuchMap(m_d->m_groupName+"["+key+"]");
  }
  return it->second;
}

// std::deque<std::string>& GroupConfigurationStructure::getListsValueAtKey(const QString& key)
// {
//   return getListsValueAtKey(key.toStdString());
// }
//
// std::map<std::string, std::string>& GroupConfigurationStructure::getMapAtKey(const QString& key)
// {
//   return getMapAtKey(key.toStdString());
// }

const map<string,string>& GroupConfigurationStructure::getParams()
{ return m_d->m_params;}

const std::map<std::string,std::map<std::string,std::string> >& GroupConfigurationStructure::getMaps()
{ return m_d->m_maps; }

const std::map<std::string,std::deque<std::string> >& GroupConfigurationStructure::getLists()
{ return m_d->m_lists; }

void GroupConfigurationStructure::addAttribute(const std::string& key,const std::string& value)
{
  m_d->m_attributes[key]=value;
}

void GroupConfigurationStructure::addParamValuePair(const string& key, const string& value)
{
  m_d->m_params.insert(make_pair(key,value));
}

void GroupConfigurationStructure::addListNamed(const string& listName)
{
  m_d->m_lists.insert(make_pair(listName,deque<string>()));
}

void GroupConfigurationStructure::addItemInListNamed(const string& item, const string& listName)
{
  MSDS::iterator it = m_d->m_lists.find(listName);
  if (it == m_d->m_lists.end())
    throw NoSuchList(m_d->m_groupName+"["+listName+"]");
  /*
  deque<string> list((*it).second);
  list.push_back(item);
  m_lists.erase(listName);
  m_lists.insert(make_pair(listName, list));
  */
  (*it).second.push_back(item);
}

void GroupConfigurationStructure::addMap(const std::string& mapName)
{
  m_d->m_maps.insert(make_pair(mapName,MSS()));
}

void GroupConfigurationStructure::addEntryInMap(const std::string& mapName,const std::string& key,const std::string& value)
{
  MSMSS::iterator it=m_d->m_maps.find(mapName);
  if (it == m_d->m_maps.end())
    throw NoSuchMap(m_d->m_groupName+"["+mapName+"]");
  (it->second)[key]=value;
}

std::deque<ItemWithAttributes>& GroupConfigurationStructure::getListOfItems(const std::string& key)
{
  MSDI::iterator it = m_d->m_listsOfItems.find(key);
  if (it == m_d->m_listsOfItems.end())
    throw NoSuchList(m_d->m_groupName+"["+key+"]");
  return (*it).second;
}

std::map<std::string,ItemWithAttributes>& GroupConfigurationStructure::getMapOfItems(const std::string& key)
{
  MSMSI::iterator it = m_d->m_mapsOfItems.find(key);
  if (it == m_d->m_mapsOfItems.end())
    throw NoSuchMap(m_d->m_groupName+"["+key+"]");
  return it->second;
}

void GroupConfigurationStructure::addListOfItems(const std::string& listName)
{
  m_d->m_listsOfItems[listName]=std::deque<ItemWithAttributes>(0);
}

void GroupConfigurationStructure::addItemInListOfItems(const std::string& key,
                     const ItemWithAttributes& item)
{
  MSDI::iterator it = m_d->m_listsOfItems.find(key);
  if (it == m_d->m_listsOfItems.end())
    throw NoSuchList(m_d->m_groupName+"["+key+"]");
  (*it).second.push_back(item);
}

void GroupConfigurationStructure::addMapOfItems(const std::string& mapName)
{
  m_d->m_mapsOfItems[mapName]=std::map<std::string,ItemWithAttributes>();
}

void GroupConfigurationStructure::addEntryInMapOfItems(const std::string& mapName,
                     const std::string& key,
                     const ItemWithAttributes& item)
{
  MSMSI::iterator it = m_d->m_mapsOfItems.find(mapName);
  if (it == m_d->m_mapsOfItems.end())
    throw NoSuchMap(m_d->m_groupName+"["+mapName+"]");
  ((*it).second)[key]=item;
}

void GroupConfigurationStructure::changeListToListOfItems(const std::string &listName)
{
  MSDS::iterator it = m_d->m_lists.find(listName);
  if (it == m_d->m_lists.end())
    throw NoSuchList(m_d->m_groupName+"["+listName+"]");

  // create new list of items
  m_d->m_listsOfItems[listName]=deque<ItemWithAttributes>(0);
  deque<ItemWithAttributes>& newList=m_d->m_listsOfItems[listName];
  for (deque<string>::const_iterator
         entry=(*it).second.begin(),entry_end=(*it).second.end();
       entry!=entry_end; entry++) {
    newList.push_back(ItemWithAttributes((*entry)));
  }

  // erase old map
 m_d-> m_lists.erase(it);
}

void GroupConfigurationStructure::changeMapToMapOfItems(const std::string &mapName)
{
  MSMSS::iterator it = m_d->m_maps.find(mapName);
  if (it == m_d->m_maps.end())
    throw NoSuchMap(m_d->m_groupName+"["+mapName+"]");

  // create new map of items
  m_d->m_mapsOfItems[mapName]=map<string,ItemWithAttributes>();
  map<string,ItemWithAttributes>& newMap=m_d->m_mapsOfItems[mapName];
  for (map<string,string>::const_iterator
         entry=(*it).second.begin(),entry_end=(*it).second.end();
       entry!=entry_end; entry++) {
    newMap[(*entry).first]=ItemWithAttributes((*entry).second);
  }

  // erase old map
  m_d->m_maps.erase(it);
}

// void GroupConfigurationStructure::addAttribute(const QString& key,const QString& value)
// {
//   addAttribute(key.toStdString(), value.toStdString());
// }
//
// void GroupConfigurationStructure::addParamValuePair(const QString& key, const QString& value)
// {
//   addParamValuePair(key.toStdString(), value.toStdString());
// }
//
// void GroupConfigurationStructure::addListNamed(const QString& listName)
// {
//   addListNamed(listName.toStdString());
// }
//
// void GroupConfigurationStructure::addItemInListNamed(const QString& item, const QString& listName)
// {
//   addItemInListNamed(item.toStdString(), listName.toStdString());
// }
//
// void GroupConfigurationStructure::addMap(const QString& mapName)
// {
//   addMap(mapName.toStdString());;
// }
//
// void GroupConfigurationStructure::addEntryInMap(const QString& mapName,const QString& key,const QString& value)
// {
//   addEntryInMap(mapName.toStdString(), key.toStdString(), value.toStdString());
// }
//
// std::deque<ItemWithAttributes>& GroupConfigurationStructure::getListOfItems(const QString& key)
// {
//   return getListOfItems(key.toStdString());
// }
//
// std::map<std::string,ItemWithAttributes>& GroupConfigurationStructure::getMapOfItems(const QString& key)
// {
//   return getMapOfItems(key.toStdString());
// }
//
// void GroupConfigurationStructure::addListOfItems(const QString& listName)
// {
//   addListOfItems(listName.toStdString());
// }
//
// void GroupConfigurationStructure::addItemInListOfItems(const QString& key, const ItemWithAttributes& item)
// {
//   addItemInListOfItems(key.toStdString(), item);
// }
//
// void GroupConfigurationStructure::addMapOfItems(const QString& mapName)
// {
//   addMapOfItems(mapName.toStdString());
// }
//
// void GroupConfigurationStructure::addEntryInMapOfItems(const QString& mapName,
//                                                        const QString& key,
//                                                        const ItemWithAttributes& item)
// {
//   addEntryInMapOfItems(mapName.toStdString(), key.toStdString(), item);
// }
//
// void GroupConfigurationStructure::changeListToListOfItems(const QString &listName)
// {
//   changeListToListOfItems(listName.toStdString());
// }
//
// void GroupConfigurationStructure::changeMapToMapOfItems(const QString &mapName)
// {
//   changeMapToMapOfItems(mapName.toStdString());
// }

std::ostream& operator<<(std::ostream& os, const GroupConfigurationStructure& dgcs)
{
  return os << "    Attributes :     " << dgcs.m_d->m_attributes << endl
         << "    Params :     " << dgcs.m_d->m_params << endl
         << "    Lists :             " << dgcs.m_d->m_lists << endl
         << "    Maps :             " << dgcs.m_d->m_maps << endl;
}

std::ostream& operator<<(std::ostream& os, const MSS& mss)
{
  for (MSS::const_iterator it = mss.begin(); it != mss.end(); it++)
  {
    os << (*it).first << " : " << (*it).second << endl;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const MSDS& msds)
{
  for (MSDS::const_iterator it = msds.begin(); it != msds.end(); it++)
  {
    os << (*it).first << " : [" ;
    const deque<string>& ds = (*it).second;
    for (deque<string>::const_iterator dit = ds.begin(); dit != ds.end(); dit++)
    {
      os << (*dit) << ", ";
    }
    os << "]" << endl;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const MSMSS& msmss)
{
  for (MSMSS::const_iterator it=msmss.begin();it!=msmss.end();it++)
  {
    os << it->first << " { ";
    const MSS& mss=it->second;
    for (MSS::const_iterator it2=mss.begin();it2!=mss.end();it2++)
    {
      os << "[" << it2->first << "->" << it2->second << "]";
    }
    os << endl;
  }
  return os;
}

} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima
