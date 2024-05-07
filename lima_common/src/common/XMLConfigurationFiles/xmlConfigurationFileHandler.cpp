// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    m_moduleName(), m_groupName(""), m_listName(""), m_configuration(theConfiguration)
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
  if (stringName == std::string("module"))
  {
    m_moduleName = attributes.value("name").toUtf8().constData();
    LTRACE << "XMLConfigurationFileHandler::startElement module name is " << m_moduleName;
    if ((m_configuration.find(m_moduleName)) == (m_configuration.end()))
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
  }
  else if (stringName == "item")
  {
    uint32_t nbAtt=attributes.length();
    if (m_firstItem) {
      LTRACE << "add simple list "<< m_listName;
      m_configuration.addListNamedForModuleAndGroup(m_listName, m_moduleName, m_groupName);
      m_firstItem=false;
    }

    string value = toString(attributes.value("value"));
    m_configuration.addItemInListNamedForModuleAndGroup(value, m_listName, m_moduleName, m_groupName);
  }
  else if (stringName == "map")
  {
    m_mapName = toString(attributes.value("name"));
    LTRACE << "map name is " << m_mapName;
  }
  else if (stringName == "entry")
  {
    LTRACE << "entry in map";

    uint32_t nbAtt=attributes.length();
    if (m_firstItem) {
      // decide if map is simple map or map of entries with attributes
      LTRACE << "add map list "<< m_mapName.c_str();
      m_configuration.addMapNamedForModuleAndGroup(m_mapName,m_moduleName,m_groupName);
      m_firstItem=false;
    }

    string key = toString(attributes.value("key"));
    string value = toString(attributes.value("value"));
    m_configuration.addEntryInMapNamedForModuleAndGroup(key,value,m_mapName,m_moduleName,m_groupName);
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
