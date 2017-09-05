/**
 * \file    LimaConfiguration.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#include "LimaConfiguration.h"
#include "LimaGui.h"

//#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "common/LimaCommon.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/configurationStructure.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"

#include <iostream>

#include <QXmlStreamWriter>
#include <QFile>

#define LIMAGUICONFLOGINIT LOGINIT("Lima::Gui::Config")

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

LimaConfiguration::LimaConfiguration() 
{

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
  LIMAGUICONFLOGINIT;
  LINFO << "\t\twriting list<" << name << ">:";

  XMLWRITE_LIST XMLWRITE_NAME(name)
      ///
      for (auto& str : list) {
//        LTELL(str);
        XMLWRITE_ITEM XMLWRITE_VALUE(str) XMLWRITE_END
      }
  XMLWRITE_END_LIST
}

void writeMap(const std::string& name, const std::map<std::string, std::string>& map, QXmlStreamWriter& xml) 
{

  LIMAGUICONFLOGINIT;
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

  LIMAGUICONFLOGINIT;
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

  LIMAGUICONFLOGINIT;
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

  LIMAGUICONFLOGINIT;
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
