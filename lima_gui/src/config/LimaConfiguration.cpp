/*
    Copyright 2017 CEA LIST

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
 * \file    LimaConfiguration.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#include "LimaConfiguration.h"
#include "LimaGuiCommon.h"

//#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "common/LimaCommon.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/configurationStructure.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"

#include <iostream>

#include <QXmlStreamWriter>
#include <QFile>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;
// using namespace Lima::LinguisticProcessing;

namespace Lima 
{
namespace Gui 
{
namespace Config 
{
const char* neutralString(const std::string& str);
const char* neutralString(const char[]);

/// \brief writes a list element from a groupStructure in the referenced QXMLStreamWriter. Is supposed to be called within writeGroup.
void writeList(const std::string& name, const std::deque<std::string>& list, QXmlStreamWriter& xml);

/// \brief writes a map element from a groupStructure in the referenced QXMLStreamWriter. Is supposed to be called within writeGroup.
void writeMap(const std::string& name, const std::map<std::string, std::string>& map, QXmlStreamWriter& xml);

/// \brief writes a group element from a moduleStructure in the referenced QXMLStreamWriter. Is supposed to be called within writeModule.
void writeGroup(const Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& p_group, QXmlStreamWriter& xml);

/// \brief writes a group element from a moduleStructure in the referenced QXMLStreamWriter. Is supposed to be called within writeFile.
void writeModule(const Lima::Common::XMLConfigurationFiles::ModuleConfigurationStructure& p_module, QXmlStreamWriter& xml);

/// \brief with this function, you can write a ConfigurationStructure
/// to a new xml configuration file.
void writeFile(const Lima::Common::XMLConfigurationFiles::ConfigurationStructure& cstruct, const std::string& path);


LimaConfiguration::LimaConfiguration(const QFileInfo& fileInfo, 
                                     QObject* parent) :
  QObject(parent),
  m_name(fileInfo.fileName()),
  m_path(fileInfo.absoluteFilePath())
{
  XMLConfigurationFileParser parser(m_path.toStdString());
  m_configuration = parser.getConfiguration();
}

const Common::XMLConfigurationFiles::ConfigurationStructure& LimaConfiguration::configuration() const
{
  return m_configuration;
}

  /// \brief This function was meant to bypass the QString constructor need for
/// a c_str, for the macro 'XMLWRITE_ATTRIBUTE' and 'XMLWRITE_ELEMENT'
const char* neutralString(const std::string& str) 
{
  return str.c_str();
}

const char* neutralString(const char c[]) 
{
  return c;
}

// for syntactic sugar

#define XML_DOCUMENT      xml.writeStartDocument();
#define END_XML_DOCUMENT  xml.writeEndDocument();

#define XMLWRITE_ELEMENT(str) xml.writeStartElement(neutralString(str))

#define XMLWRITE_GROUP    XMLWRITE_ELEMENT("group");
#define XMLWRITE_LIST     XMLWRITE_ELEMENT("list");
#define XMLWRITE_MAP      XMLWRITE_ELEMENT("map");
#define XMLWRITE_ITEM     XMLWRITE_ELEMENT("item");
#define XMLWRITE_PARAM    XMLWRITE_ELEMENT("param");
#define XMLWRITE_MODULE   XMLWRITE_ELEMENT("module");
#define XMLWRITE_ENTRY    XMLWRITE_ELEMENT("entry");

#define XMLWRITE_ATTRIBUTE(name, value) xml.writeAttribute(neutralString(name),neutralString(value))

#define XMLWRITE_NAME(str)  XMLWRITE_ATTRIBUTE("name", str);
#define XMLWRITE_VALUE(str) XMLWRITE_ATTRIBUTE("value", str);
#define XMLWRITE_KEY(str)   XMLWRITE_ATTRIBUTE("key", str);
#define XMLWRITE_CLASS(str) XMLWRITE_ATTRIBUTE("class", str);

#define XMLWRITE_END xml.writeEndElement();

#define XMLWRITE_END_GROUP   XMLWRITE_END
#define XMLWRITE_END_LIST    XMLWRITE_END
#define XMLWRITE_END_MODULE  XMLWRITE_END
#define XMLWRITE_END_MAP     XMLWRITE_END

void writeList(const std::string& name, const std::deque<std::string>& list, QXmlStreamWriter& xml) 
{
  CONFLOGINIT;
  LINFO << "\t\twriting list<" << name << ">:";

  XMLWRITE_LIST XMLWRITE_NAME(name)
      ///
      for (auto& str : list) {
//        LDEBUG << str;
        XMLWRITE_ITEM XMLWRITE_VALUE(str) XMLWRITE_END
      }
  XMLWRITE_END_LIST
}

void writeMap(const std::string& name, const std::map<std::string, std::string>& map, QXmlStreamWriter& xml) 
{

  CONFLOGINIT;
  LINFO << "\t\twriting map<" << name << ">:";

  XMLWRITE_MAP XMLWRITE_NAME(name)
    ///
    for (auto& pair : map) {
      XMLWRITE_ENTRY XMLWRITE_KEY(pair.first) XMLWRITE_VALUE(pair.second) XMLWRITE_END
    }

  XMLWRITE_END_MAP
}

void writeGroup(const GroupConfigurationStructure& p_group, QXmlStreamWriter& xml) 
{
  GroupConfigurationStructure group(p_group);

  CONFLOGINIT;
  LINFO << "\twriting group<" << group.getName() << ">:";

  XMLWRITE_GROUP XMLWRITE_NAME(group.getName()) 
  try
  {
    XMLWRITE_CLASS(group.getAttribute("class")) 
  } 
  catch(std::exception& nsa)
  { 
    LERROR << "No class for this group"; 
  }

      for (auto& pair : group.getParams()) 
      {
        XMLWRITE_PARAM XMLWRITE_KEY(pair.first) XMLWRITE_VALUE(pair.second) XMLWRITE_END
      }

      for (auto& pair : group.getLists()) 
      {
        writeList(pair.first, pair.second, xml);
      }

      for (auto& pair : group.getMaps()) 
      {
        writeMap(pair.first, pair.second, xml);
      }

  XMLWRITE_END_GROUP
}

void writeModule(const ModuleConfigurationStructure& p_module, QXmlStreamWriter& xml) 
{
  ModuleConfigurationStructure module(p_module);

  CONFLOGINIT;
  LINFO << "writing module<" << module.getName() << ">:";

  XMLWRITE_MODULE XMLWRITE_NAME(module.getName())

      for (auto& pair : module) {
        writeGroup(pair.second, xml);
      }

  XMLWRITE_END_MODULE
}

void writeFile(const ConfigurationStructure& cstruct, const std::string& path) 
{
  QString output;

  CONFLOGINIT;
  LINFO << "writing file<" << path << ">:";

  QXmlStreamWriter xml(&output);

  xml.setCodec("UTF-8"); // oddly, qxmlstreamwriter does not write encoding information ...
  // even though the doc says it should

  xml.setAutoFormatting(true); // auto indentation

  XML_DOCUMENT;

  XMLWRITE_ELEMENT("modulesConfig");

    for (const auto& pair : cstruct) 
    {
      writeModule(pair.second, xml);
    }

  XMLWRITE_END;

  END_XML_DOCUMENT;

//   LINFO << output.toStdString().substr(0,300);
}

} // Config
} // Gui
} // Lima
