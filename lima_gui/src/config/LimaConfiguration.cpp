#include "LimaConfiguration.h"

#include "LimaGui.h"

//#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
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
using namespace Lima::LinguisticProcessing;

namespace Lima {
namespace Gui {
namespace Config {

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

  //saveAsXml();


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

#define WRITE_ELEMENT(str) xml.writeStartElement(neutralString(str))

#define WRITE_GROUP WRITE_ELEMENT("group");
#define WRITE_LIST WRITE_ELEMENT("list");
#define WRITE_MAP WRITE_ELEMENT("map");
#define WRITE_ITEM WRITE_ELEMENT("item");
#define WRITE_PARAM WRITE_ELEMENT("param");
#define WRITE_MODULE WRITE_ELEMENT("module");
#define WRITE_ENTRY WRITE_ELEMENT("entry");

#define WRITE_ATTRIBUTE(name, value) xml.writeAttribute(neutralString(name),neutralString(value))

#define WRITE_NAME(str) WRITE_ATTRIBUTE("name", str);
#define WRITE_VALUE(str) WRITE_ATTRIBUTE("value", str);
#define WRITE_KEY(str) WRITE_ATTRIBUTE("key", str);
#define WRITE_CLASS(str) WRITE_ATTRIBUTE("class", str);

#define WRITE_END xml.writeEndElement();

#define WRITE_END_GROUP   WRITE_END
#define WRITE_END_LIST    WRITE_END
#define WRITE_END_MODULE  WRITE_END
#define WRITE_END_MAP     WRITE_END

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
//  ELEMENT("modulesConfig");
//    MODULE NAME("entities")
//      GROUP NAME("include")
//        LIST NAME("includeList")
//          ITEM VALUE("SpecificEntities-modex.xml/entities/") END
//        END_LIST
//      END_GROUP
//    END_MODULE

//        /// So, entities are enableable ?

//    MODULE NAME("Processors")
//      GROUP NAME("include")
//        LIST NAME("includeList")
//          ITEM VALUE("SpecificEntities-modex.xml/Processors/")  END
//          ITEM VALUE("VerbNet-modex.xml/Processors/")           END
//          ITEM VALUE("FrameNet-modex.xml/Processors/")          END
//        END_LIST
//      END_GROUP

//      /// Pipelines

//      GROUP NAME("main") CLASS("ProcessUnitPipeline")
//        LIST NAME("processUnitSequence")

//          /// call function to write config trees

//        END_LIST
//      END_GROUP

//      GROUP NAME("limaserver") CLASS("ProcessUnitPipeline")
//        LIST NAME("processUnitSequence")
//        /// There's nothing here obviously ?
//        /// Only main will be modified
//        END_LIST
//      END_GROUP

//      GROUP NAME("easy") CLASS("ProcessUnitPipeline")
//        LIST NAME("processUnitSequence")

//        END_LIST
//      END_GROUP

//      /// Process Units


//  ///

  END_XML_DOCUMENT;

  std::cout << "str=" << str.toStdString() << std::endl;

}

//void LimaConfiguration::process(const std::string& configPath) {
//  /// quelles entités sont activées par défaut ?
//  ///
//  /// quelles pipelines unit sont définies par défaut ?
//  /// liste des noms des pipelines -> choix disponibles
//  ///
//  /// dans l'idéal, on va lire lima-analysis.xml, et de là on saura quelles langues implémenter
//  ///
//  /// la configuration sera la même pour les deux languages
//  /// seul le pipeline main sera édité
//  ///

////  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser ma_a(configPath + "/" + "lima-analysis.xml");

////  ConfigurationStructure& cstruct = ma_a.getConfiguration();

////  GroupConfigurationStructure& groupstruct = cstruct.getGroupConf("lima-coreclient", "mediaProcessingDefinitionFiles");

////  std::map<std::string, std::string>& params = groupstruct.getParams();

////  LTELL("params:");
////  for (auto& pair : params) {
////    LTELL(pair.first << " : " << pair.second);

////    langFiles.insert(pair.first, XMLConfigurationFileParser(configPath + "/" + pair.second));
////  }



//}

void writeList(const std::string& name, const std::deque<std::string>& list, QXmlStreamWriter& xml) {
  LTELL("\t\twriting list<" << name << ">:");

  WRITE_LIST WRITE_NAME(name)
      ///
      for (auto& str : list) {
//        LTELL(str);
        WRITE_ITEM WRITE_VALUE(str) WRITE_END
      }
  WRITE_END_LIST
}

void writeMap(const std::string& name, const std::map<std::string, std::string>& map, QXmlStreamWriter& xml) {


  LTELL("\t\twriting map<" << name << ">:");

  WRITE_MAP WRITE_NAME(name)
    ///
    for (auto& pair : map) {
      WRITE_ENTRY WRITE_KEY(pair.first) WRITE_VALUE(pair.second) WRITE_END
    }

  WRITE_END_MAP
}

void writeGroup(const GroupConfigurationStructure& p_group, QXmlStreamWriter& xml) {

  GroupConfigurationStructure group(p_group);


  LTELL("\twriting group<" << group.getName() << ">:");

  WRITE_GROUP WRITE_NAME(group.getName()) try { WRITE_CLASS(group.getAttribute("class")) } catch(std::exception& nsa) { LTELL("No class for this group"); }
      /// Meh ... for the class attribute
      ///

      for (auto& pair : group.getParams()) {
        WRITE_PARAM WRITE_KEY(pair.first) WRITE_VALUE(pair.second) WRITE_END
      }

      for (auto& pair : group.getLists()) {
        writeList(pair.first, pair.second, xml);
      }

      for (auto& pair : group.getMaps()) {
        writeMap(pair.first, pair.second, xml);
      }

  WRITE_END_GROUP
}

void writeModule(const ModuleConfigurationStructure& p_module, QXmlStreamWriter& xml) {
  ModuleConfigurationStructure module(p_module);

  LTELL("writing module<" << module.getName() << ">:");

  WRITE_MODULE WRITE_NAME(module.getName())

      for (auto& pair : module) {
        writeGroup(pair.second, xml);
      }

  WRITE_END_MODULE
}

//void overrideList(const std::string& name, const std::deque<std::string>& p_list, QXmlStreamWriter& xml, GroupConfigurationStructure* sub) {

//  std::deque<std::string>* target = nullptr;

//  if (sub && sub->getLists().find(name) != sub->getLists().end()) {
//    target = &sub->getLists()[name];
//    std::cout << "(sub)";
//  }

//  std::deque<std::string>& list = (target ? *target : p_list);

//  LTELL("\t\toverride writing list<" << name << ">:");

//  LIST NAME(name)
//      ///
//      for (auto& str : list) {
////        LTELL(str);
//        ITEM VALUE(str) END
//      }
//  END_LIST
//}

//void overrideMap(const std::string& name, const std::map<std::string, std::string>& p_map, QXmlStreamWriter& xml, GroupConfigurationStructure* sub) {

//  std::map<std::string, std::string>* target = nullptr;

//  if (sub && sub->getMaps().find(name) != sub->getMaps().end()) {
//    target = &sub->getMaps()[name];
//    std::cout << "(sub)";
//  }

//  std::map<std::string, std::string>& map = (target ? *target : p_map);

//  LTELL("\t\toverride writing map<" << name << ">:");

//  MAP NAME(name)
//    ///
//    for (auto& pair : map) {
//      ENTRY KEY(pair.first) VALUE(pair.second) END
//    }

//  END_MAP
//}

//void overrideGroup(const GroupConfigurationStructure& p_group, QXmlStreamWriter& xml, ModuleConfigurationStructure* sub) {

//  GroupConfigurationStructure* target = nullptr;

//  if (sub && sub->find(p_group.getName()) != sub->end()) {
//    target = &(*sub)[p_group.getName()];
//    std::cout << "(sub)";
//  }

//  GroupConfigurationStructure& group = (target ? *target : p_group);

//  LTELL("\toverride writing group<" << group.getName() << ">:");

//  GROUP NAME(group.getName()) try { CLASS(group.getAttribute("class")) } catch(std::exception& nsa) { LTELL("No class for this group"); }
//      /// Meh ... for the class attribute
//      ///

//      for (auto& pair : group.getParams()) {
//        PARAM KEY(pair.first) VALUE(pair.second) END
//      }

//      for (auto& pair : group.getLists()) {
//        overrideList(pair.first, pair.second, xml, target);
//      }

//      for (auto& pair : group.getMaps()) {
//        overrideMap(pair.first, pair.second, xml, target);
//      }

//  END_GROUP

//}

//void overrideModule(const ModuleConfigurationStructure& p_module, QXmlStreamWriter& xml, const ConfigurationStructure& sub) {

//  ModuleConfigurationStructure* target = nullptr;

//  if (sub.find(p_module.getName()) != sub.end()) {
////    overrideWriteModule(sub[p_module.getName()], xml, sub);
//    target = &sub[p_module.getName()];
//    std::cout << "(sub)";
//  }

//  ModuleConfigurationStructure& module = (target ? *target : p_module);

//  LTELL("override writing module<" << module.getName() << ">:");

//  MODULE NAME(module.getName())

//      for (auto& pair : module) {
//        overrideGroup(pair.second, xml, target);
//      }

//  END_MODULE

//}

//QString overrideWriteConfigurationStructure(const ConfigurationStructure& original, const ConfigurationStructure& substitute, const std::string& path) {
//  QString output;

//  QXmlStreamWriter xml(&output);

//  xml.setCodec("UTF-8"); // oddly, qxmlstreamwriter does not write encoding information ...
//  // even though the doc says it should

//  xml.setAutoFormatting(true);

////  auto& cstruct = xmlcfgparser.getConfiguration();

//  XML_DOCUMENT;

//  ELEMENT("modulesConfig");

//  for (const auto& pair : original) {
//    overrideModule(pair.second, xml, substitute);
//  }

//  END;

//  END_XML_DOCUMENT;


//  return output;
//}


///// override 'original' cs with the content of 'substitute'
///// unfinished
//ConfigurationStructure overrideConfigurationStructure(const ConfigurationStructure& original, const ConfigurationStructure& sub) {

//  ConfigurationStructure result;

//  for (auto& module : original) {

//    result.insert(std::pair<std::string, ModuleConfigurationStructure>(module.getName(), ModuleConfigurationStructure()));

//    if (substitute.find(module.getName()) != substitute.end()) {
////      result[module.getName()] = module;
//      ModuleConfigurationStructure& moduleSubstitute = substitute[model.getName()];
//      ModuleConfigurationStructure& moduleTarget = result[model.getName()];

//      for (auto& group : module) {

//        if (modelSubstitute.find(group.getName()) != modelSubstitute.end()) {

//        }
//        else {

//        }

//      }

//    }
//    else {
//      result[module.getName()] = substitute[module.getName()];
//    }
//  }

//  /// add remaining content in substitute that does not exist in original

//  for (auto& module : substitute) {
//    if (result.find(module.getName()) != result.end()) {

//      auto& resultModule = result[model.getName()];

//      for (auto& group : module) {

//        if (resultModule.find(group.getName()) != resultModule.end()) {

//          auto& resultGroup = resultModule[group.getName()];

//          for (auto& pair : group.getMaps()) {

//            if (resultGroup.getMaps().find(pair.first) != resultGroup.getMaps().end()) {

//              auto& resultMap = resultGroup.getMaps()[pair.first];

//              for (auto& item : pair.second) {
//                if (resultMap.find(item.first) == resultMap.end()) {
//                  resultGroup.addEntryInMap(pair.first, item.first, item.second);
//                }
//              }
//            }

//            else {
//              resultGroup.addMap(pair.first);
//              for (auto& item : pair.second) {
//                resultGroup.addEntryInMap(pair.first, item.first, item.second);
//              }
//            }

//          }

//          for (auto& pair : group.getLists()) {

//            if (resultGroup.getLists().find(pair.first) != resultGroup.getLists().end()) {

//              auto& resultList = resultGroup.getLists()[pair.first];

//              resultGroup.addListNamed(pair.first);
//              for (auto& item : pair.second) {
//                if (std::find(resultList.begin(), resultList.end(), item) != resultList.end()) {
//                  resultGroup.addItemInListNamed(item, pair.first);
//                }
//              }

//            }

//            else {
//              resultGroup.addListNamed(pair.first);
//              for (auto& item : pair.second) {
//                resultGroup.addItemInListNamed(item, pair.first);
//              }
//            }

//          }

//          for (auto& pair : group.getParams()) {

//            if () {
//              /// can't get a list of attributes though ....
//            }

//            else {

//            }

//          }

//        }

//        else {
//          resultModule[group.getName()] = group;
//        }

//      }

//    }
//    else {
//      result[module.getName()] = module;
//    }
//  }

//  return result;

//}

void LimaConfiguration::writeFile(XMLConfigurationFileParser& xmlcfgparser, const std::string& path) {
  QString output;

  LTELL("writing file<" << path << ">:");

  QXmlStreamWriter xml(&output);

  xml.setCodec("UTF-8"); // oddly, qxmlstreamwriter does not write encoding information ...
  // even though the doc says it should

  xml.setAutoFormatting(true);

  auto& cstruct = xmlcfgparser.getConfiguration();

  XML_DOCUMENT;

  WRITE_ELEMENT("modulesConfig");

  for (const auto& pair : cstruct) {
    writeModule(pair.second, xml);
  }

  WRITE_END;

  END_XML_DOCUMENT;

  LTELL(output.toStdString().substr(0,300));
}

/////////////////

//ConfigurationTree::ConfigurationTree(const ConfigurationStructure& cstruct) {

//  root = ConfigurationNode(cstruct);

//}

//ConfigurationTree::ConfigurationTree(const ModuleConfigurationStructure& mstruct) {

//  root = ConfigurationNode(mstruct);

//}

//ConfigurationTree::ConfigurationTree(const GroupConfigurationStructure& gstruct) {

//  root = ConfigurationNode(gstruct);

//}

} // Config
} // Gui
} // Lima
