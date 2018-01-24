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
  * @file       xmlConfigurationFileHandler.cpp
  * @brief      originally in detectlibraries
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             copyright (C) 2002-2003 by CEA
  */

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "common/LimaCommon.h"
#include <common/Data/strwstrtools.h>
#include "xmlConfigurationFileHandler.h"
#include "xmlConfigurationFileExceptions.h"

using namespace std;

namespace Lima
{
namespace Common
{
namespace XMLConfigurationFiles
{

// ---------------------------------------------------------------------------
//  XMLConfigurationFileHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
XMLConfigurationFileHandler::XMLConfigurationFileHandler(ConfigurationStructure& theConfiguration) :
    QXmlDefaultHandler(),
    m_moduleName(""), m_groupName(""), m_listName(""), m_configuration(theConfiguration)
{}

XMLConfigurationFileHandler::~XMLConfigurationFileHandler()
{}


// ---------------------------------------------------------------------------
//  XMLConfigurationFileHandler: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
bool XMLConfigurationFileHandler::error(const QXmlParseException& e)
{
  XMLCFGLOGINIT;
  LERROR << "Error at file " << e.systemId()
  << ", line " << e.lineNumber()
  << ", char " << e.columnNumber()
  << "  Message: " << e.message();
  return false;
}

bool XMLConfigurationFileHandler::fatalError(const QXmlParseException& e)
{
  XMLCFGLOGINIT;
  LERROR << "Fatal Error at file " << e.systemId()
  << ", line " << e.lineNumber()
  << ", char " << e.columnNumber()
  << "  Message: " << e.message();
  return false;
}

bool XMLConfigurationFileHandler::warning(const QXmlParseException& e)
{
  XMLCFGLOGINIT;
  LWARN << "Warning at file " << e.systemId()
  << ", line " << e.lineNumber()
  << ", char " << e.columnNumber()
  << "  Message: " << e.message();
  return true;
}


bool XMLConfigurationFileHandler::endElement(const QString & , const QString & eltName, const QString & )
{
  string name=toString(eltName);
  if (name == "Module") {
    m_moduleName = "";
  }
  else if (name == "group") {
    m_groupName = "";
  }
  else if (name == "list") {
    if (m_firstItem) {
      // empty list: must be added anyway
      m_configuration.addListNamedForModuleAndGroup(m_listName, m_moduleName, m_groupName);
      m_firstItem=false;
    }
    m_listName = "";
  }
  else if (name == "map") {
    if (m_firstItem) {
      // empty map: must be added anyway
      m_firstItem=false;
      m_configuration.addMapNamedForModuleAndGroup(m_mapName,m_moduleName,m_groupName);
    }
    m_mapName = "";
  }
  return true;
}


bool XMLConfigurationFileHandler::startElement( const QString & , const QString & name, const QString & , const QXmlAttributes & attributes)
{
  string stringName(toString(name));

  XMLCFGLOGINIT;
  LTRACE << "start element " << stringName;

  // set the current module name and create its entry if necessary
  if (stringName == string("module"))
  {
    m_moduleName = attributes.value("name").toUtf8().constData();
    LTRACE << "XMLConfigurationFileHandler::startElement module name is " << m_moduleName;
    if ((m_configuration. find(m_moduleName)) == (m_configuration. end()))
    {
      m_configuration.insert(make_pair(m_moduleName, ModuleConfigurationStructure(m_moduleName)));
    }
  }
  // set the current group name inside moduleName and creates its entry if necessary
  else if (stringName == "group")
  {
    m_groupName = toString(attributes.value("name"));
    LTRACE << "group name is " << m_groupName;
    int32_t indName=attributes.index("name"); // index for the attribute 'name' (not always the first)
    
    m_configuration.addGroupNamedForModuleNamed(m_groupName, m_moduleName);
    for (int32_t i=0;i<attributes.length();i++)
    {
      if (i==indName) { // if attribute 'name', ignored (already treated)
        continue;
      }
      string key=toString(attributes.localName(i));
      string value=toString(attributes.value(i));
      m_configuration.addAttributeForGroupInModule(key,value,m_groupName,m_moduleName);
    }
  }
  // creates a new parameter inside the current module and group
  else if (stringName == "param")
  {
    string key = toString(attributes.value("key"));
    string value = toString(attributes.value("value"));
    LTRACE << "param key is " << key;
    m_configuration.addParamValuePairForModuleAndGroup(key, value, m_moduleName, m_groupName);
  }
  else if (stringName == "list")
  {
    m_listName = toString(attributes.value("name"));
    LTRACE << "list name is " << m_listName;

    m_firstItem=true;
    m_itemWithAttributes=false;
  }
  else if (stringName == "item")
  {
    uint32_t nbAtt=attributes.length();
    if (m_firstItem) {
      // decide if list is simple list or list of items with attributes
      if (nbAtt==1) {
        LTRACE << "add simple list "<< m_listName;
        m_configuration.addListNamedForModuleAndGroup(m_listName, m_moduleName, m_groupName);
      }
      else {
        LTRACE << "add list of items with attributes"<< m_listName;
        m_configuration.addListOfItemsForModuleAndGroup(m_listName, m_moduleName, m_groupName);
        m_itemWithAttributes=true;
      }
      m_firstItem=false;
    }
    else if (nbAtt>1 && !m_itemWithAttributes) {
      // was indeed in list of item with attributes => has to change     
      m_configuration.changeListToListOfItems(m_listName,m_moduleName,m_groupName);
      m_itemWithAttributes=true;
    }
    
    if (m_itemWithAttributes) {
      string itemName=toString(attributes.value("value"));
      ItemWithAttributes item(itemName);
      for (uint32_t i=1; i<nbAtt; i++) {
        item.addAttribute(toString(attributes.localName(i)),
                          toString(attributes.value(i)));
      }
      m_configuration.addItemInListOfItemsForModuleAndGroup(item, m_listName, m_moduleName, m_groupName);
    }
    else {
      string value = toString(attributes.value("value"));
      m_configuration.addItemInListNamedForModuleAndGroup(value, m_listName, m_moduleName, m_groupName);
    }
  }
  else if (stringName == "map")
  {
    m_mapName = toString(attributes.value("name"));
    LTRACE << "map name is " << m_mapName;
    m_firstItem=true;
    m_itemWithAttributes=false;
  }
  else if (stringName == "entry")
  {
    LTRACE << "entry in map";

    uint32_t nbAtt=attributes.length();
    if (m_firstItem) {
      // decide if map is simple map or map of entries with attributes
      if (nbAtt==2) { // name+value => simple map
        LTRACE << "add map list "<< m_mapName.c_str();
        m_configuration.addMapNamedForModuleAndGroup(m_mapName,m_moduleName,m_groupName);
      }
      else {
        LTRACE << "add map of items with attributes "<< m_mapName;
        m_configuration.addMapOfItemsForModuleAndGroup(m_mapName,m_moduleName,m_groupName);
        m_itemWithAttributes=true;
      }
      m_firstItem=false;
    }
    else if (nbAtt>2 && !m_itemWithAttributes) {
      // was indeed in list of item with attributes => has to change     
      m_configuration.changeMapToMapOfItems(m_mapName,m_moduleName,m_groupName);
      m_itemWithAttributes=true;
    }
    
    if (m_itemWithAttributes) {
      string key=toString(attributes.value("key"));
      string value=toString(attributes.value("value"));
      ItemWithAttributes item(value);
      for (uint32_t i=1; i<nbAtt; i++) {
        string attName=toString(attributes.localName(i));
        if (attName != "key" && attName != "value") {
          item.addAttribute(attName,
                            toString(attributes.value(i)));
        }
      }
      m_configuration.addEntryInMapOfItemsForModuleAndGroup(key,item,m_mapName,m_moduleName,m_groupName);
    }
    else {
      string key = toString(attributes.value("key"));
      string value = toString(attributes.value("value"));
      m_configuration.addEntryInMapNamedForModuleAndGroup(key,value,m_mapName,m_moduleName,m_groupName);      
    }
  }
  return true;
}

std::string XMLConfigurationFileHandler::toString(const QString& xercesString)
{
  return Misc::limastring2utf8stdstring(xercesString);
}

} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima
