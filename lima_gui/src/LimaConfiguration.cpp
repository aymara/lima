#include "LimaConfiguration.h"

#include "LimaGui.h"

//#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/configurationStructure.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
//#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"

#include <iostream>

#include <QXmlStreamWriter>
#include <QFile>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;

namespace Lima {
namespace Gui {

LimaConfiguration::LimaConfiguration() {

//  std::string path = "~/xmldummy.xml";
//  QString tump;
//  QFile file(QString(path.c_str()));
//  if (file.open(QFile::ReadOnly)) {
////    QTextStream qts(&file);
////    tump = qts.readAll();
//    file.close();
//  }
//  else {
//    return;
//  }

  saveAsXml();


}

const char* neutralString(const std::string& str) {
  return str.c_str();
}

const char* neutralString(const char c[]) {
  return c;
}

// for syntactic sugar

#define XML_DOCUMENT xml.writeStartDocument();
#define END_XML_DOCUMENT xml.writeEndDocument();

#define ELEMENT(str) xml.writeStartElement(neutralString(str))

#define GROUP ELEMENT("group");
#define LIST ELEMENT("list");
#define MAP ELEMENT("map");
#define ITEM ELEMENT("item");
#define PARAM ELEMENT("param");
#define MODULE ELEMENT("module");

#define ATTRIBUTE(name, value) xml.writeAttribute(neutralString(name),neutralString(value))

#define NAME(str) ATTRIBUTE("name", str);
#define VALUE(str) ATTRIBUTE("value", str);
#define KEY(str) ATTRIBUTE("key", str);
#define CLASS(str) ATTRIBUTE("class", str);

#define END xml.writeEndElement();

#define END_GROUP   END
#define END_LIST    END
#define END_MODULE  END
#define END_MAP     END

void LimaConfiguration::saveAsXml() {

  QString str;

  QXmlStreamWriter xml(&str);

//  xml.setAutoFormatting(true);
//  xml.writeStartDocument();

//  xml.writeStartElement("bookmark");
//  xml.writeAttribute("href", "http://qt-project.org/");
//  xml.writeTextElement("title", "Qt Project");
//  xml.writeEndElement(); // bookmark

//  xml.writeEndDocument();

  xml.setAutoFormatting(true);

  XML_DOCUMENT;

  ///

  ELEMENT("modulesConfig");
    MODULE NAME("entities")
      GROUP NAME("include")
        LIST NAME("includeList")
          ITEM VALUE("SpecificEntities-modex.xml/entities/") END
        END_LIST
      END_GROUP
    END_MODULE

        /// So, entities are enableable ?

    MODULE NAME("Processors")
      GROUP NAME("include")
        LIST NAME("includeList")
          ITEM VALUE("SpecificEntities-modex.xml/Processors/")  END
          ITEM VALUE("VerbNet-modex.xml/Processors/")           END
          ITEM VALUE("FrameNet-modex.xml/Processors/")          END
        END_LIST
      END_GROUP

      /// Pipelines

      GROUP NAME("main") CLASS("ProcessUnitPipeline")
        LIST NAME("processUnitSequence")

          /// call function to write config trees

        END_LIST
      END_GROUP

      GROUP NAME("limaserver") CLASS("ProcessUnitPipeline")
        LIST NAME("processUnitSequence")
        /// There's nothing here obviously ?
        /// Only main will be modified
        END_LIST
      END_GROUP

      GROUP NAME("easy") CLASS("ProcessUnitPipeline")
        LIST NAME("processUnitSequence")

        END_LIST
      END_GROUP

      /// Process Units


  ///

  END_XML_DOCUMENT;

  std::cout << "str=" << str.toStdString() << std::endl;

}

void LimaConfiguration::process(const std::string& configPath) {
  /// quelles entités sont activées par défaut ?
  ///
  /// quelles pipelines unit sont définies par défaut ?
  /// liste des noms des pipelines -> choix disponibles
  ///
  /// dans l'idéal, on va lire lima-analysis.xml, et de là on saura quelles langues implémenter
  ///
  /// la configuration sera la même pour les deux languages
  /// seul le pipeline main sera édité
  ///

//  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser ma_a(configPath + "/" + "lima-analysis.xml");

//  ConfigurationStructure& cstruct = ma_a.getConfiguration();

//  GroupConfigurationStructure& groupstruct = cstruct.getGroupConf("lima-coreclient", "mediaProcessingDefinitionFiles");

//  std::map<std::string, std::string>& params = groupstruct.getParams();

//  LTELL("params:");
//  for (auto& pair : params) {
//    LTELL(pair.first << " : " << pair.second);

//    langFiles.insert(pair.first, XMLConfigurationFileParser(configPath + "/" + pair.second));
//  }



}

void writeList(const std::string& name, const std::deque<std::string>& list, QXmlStreamWriter& xml) {
  LIST NAME(name)
      ///
      for (auto& str : list) {
        LTELL(str);
      }
  END_LIST
}

void writeMap(const std::string& name, const std::map<std::string, std::string>& map, QXmlStreamWriter& xml) {
  MAP NAME(name)
    ///
    for (auto& pair : map) {
      LTELL(pair.first << " << " << pair.second);

    }

  END_MAP
}

void writeGroup(const GroupConfigurationStructure& p_group, QXmlStreamWriter& xml) {

  GroupConfigurationStructure group(p_group);

  GROUP NAME(group.getName()) try { CLASS(group.getAttribute("class")) } catch(std::exception& nsa) { LTELL("No class for this group"); }
      /// Meh ... for the class attribute
      ///

      for (auto& pair : group.getParams()) {
        PARAM KEY(pair.first) VALUE(pair.second)
      }

      for (auto& pair : group.getLists()) {
        writeList(pair.first, pair.second, xml);
      }

//      for (auto& pair : group.getMaps()) {
//        writeMap(pair.first, pair.second, xml);
//      }

  END_GROUP
}

void writeModule(const ModuleConfigurationStructure& p_module, QXmlStreamWriter& xml) {
  ModuleConfigurationStructure module(p_module);

  MODULE NAME(module.getName())

      for (auto& pair : module) {
        writeGroup(pair.second, xml);
      }

  END_MODULE
}

void LimaConfiguration::writeFile(XMLConfigurationFileParser& xmlcfgparser, const std::string& path) {
  QString output;

  QXmlStreamWriter xml(&output);

  auto& cstruct = xmlcfgparser.getConfiguration();

  for (const auto& pair : cstruct) {
    writeModule(pair.second, xml);
  }

  LTELL(output.toStdString());
}

} // Gui
} // Lima
