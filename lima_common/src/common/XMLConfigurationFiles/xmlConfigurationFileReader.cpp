/*
    Copyright 2022 CEA LIST

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
  * @date       begin Wed May, 25 2022
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  */

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "common/LimaCommon.h"
#include <common/Data/strwstrtools.h>
#include "xmlConfigurationFileReader.h"
#include "xmlConfigurationFileExceptions.h"

#include <QXmlStreamReader>

namespace Lima
{
namespace Common
{
namespace XMLConfigurationFiles
{

class LIMA_XMLCONFIGURATIONFILES_EXPORT XmlConfigurationFileReaderPrivate
{
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  XmlConfigurationFileReaderPrivate(ConfigurationStructure& theConfiguration);
  ~XmlConfigurationFileReaderPrivate() = default;

  bool parse(QIODevice *device);
  void readConfiguration();
  void readModule();
  void readGroup();
  void readParam();
  void readList();
  void readListItem();
  void readMap();
  void readMapEntry();

  static inline QString moduleNameAttribute() { return QStringLiteral("name"); }
  static inline QString groupNameAttribute() { return QStringLiteral("name"); }
  static inline QString listNameAttribute() { return QStringLiteral("name"); }
  static inline QString mapNameAttribute() { return QStringLiteral("name"); }
  static inline QString paramKeyAttribute() { return QStringLiteral("key"); }
  static inline QString paramValueAttribute() { return QStringLiteral("value"); }
  static inline QString listItemValueAttribute() { return QStringLiteral("value"); }
  static inline QString mapEntryKeyAttribute() { return QStringLiteral("key"); }
  static inline QString mapEntryValueAttribute() { return QStringLiteral("value"); }

  QXmlStreamReader m_reader;
  /*
    * The current module group and, possibly, list
    */
  QString m_moduleName;
  QString m_groupName;
  QString m_listName;
  QString m_mapName;

  // bools to handle items with attributes
  bool m_firstItem;
  bool m_itemWithAttributes;

  /*
    * The structure where data will be stored
    */
  ConfigurationStructure& m_configuration;
};

XmlConfigurationFileReaderPrivate::XmlConfigurationFileReaderPrivate(ConfigurationStructure& theConfiguration) :
    m_reader(),
    m_moduleName(""), m_groupName(""), m_listName(""), m_configuration(theConfiguration)
{}

XmlConfigurationFileReader::XmlConfigurationFileReader(ConfigurationStructure& theConfiguration) :
    m_d(new XmlConfigurationFileReaderPrivate(theConfiguration))
{}

XmlConfigurationFileReader::~XmlConfigurationFileReader()
{
  delete m_d;
}

bool XmlConfigurationFileReader::parse(QIODevice *device)
{
  return m_d->parse(device);
}

// <?xml version='1.0' encoding='UTF-8'?>
// <modulesConfig>
// …
// </modulesConfig>
bool XmlConfigurationFileReaderPrivate::parse(QIODevice *device)
{
  XMLCFGLOGINIT;
  LTRACE << "parse";
  m_reader.setDevice(device);
  if (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("modulesConfig"))
      {
          readConfiguration();
      }
      else
      {
          m_reader.raiseError(QObject::tr("The file is not a LIMA XML configuration file."));
      }
  }
  return !m_reader.error();
}

// <modulesConfig>
//   <module name="common">
//   …
//   </module>
//   …
// </modulesConfig>
void XmlConfigurationFileReaderPrivate::readConfiguration()
{
    XMLCFGLOGINIT;
    LTRACE << "readConfiguration";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("modulesConfig"));

    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == QLatin1String("module"))
            readModule();
        else
            m_reader.raiseError(QObject::tr("Expected a module but got a %1.").arg(m_reader.name()));
    }
}

//   <module name="common">
//     <group name="mediaDeclaration">
//     …
//     </group>
//     …
//   </module>
void XmlConfigurationFileReaderPrivate::readModule()
{
    m_moduleName = m_reader.attributes().value(moduleNameAttribute()).toString();
    XMLCFGLOGINIT;
    LTRACE << "XmlConfigurationFileReader::readModule module name is " << m_moduleName;
    if ((m_configuration.find(m_moduleName.toStdString())) == (m_configuration. end()))
    {
      m_configuration.insert(std::make_pair(m_moduleName.toStdString(), ModuleConfigurationStructure(m_moduleName)));
    }
    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == QLatin1String("group"))
            readGroup();
        else
            m_reader.raiseError(QObject::tr("Expected a group in module %1 but got a %2.").arg(m_moduleName).arg(m_reader.name()));
    }
    m_moduleName = "";
}

//     <group name="mediaDeclaration">
//       <list name="available">
//       …
//       </list>
//       <param key="eng" value="6"/>
//       <map name="conceptTypes">
//       …
//       </map>
//     </group>
void XmlConfigurationFileReaderPrivate::readGroup()
{
    m_groupName = m_reader.attributes().value(groupNameAttribute()).toString();
    XMLCFGLOGINIT;
    LTRACE << "XmlConfigurationFileReader::readGroup group name is " << m_groupName;

    m_configuration.addGroupNamedForModuleNamed(m_groupName, m_moduleName);
    for (const auto& attribute: m_reader.attributes())
    {
      if (attribute.name() == groupNameAttribute()) {
        continue;
      }
      auto key = attribute.name();
      auto value = attribute.value();
      m_configuration.addAttributeForGroupInModule(key.toString(), value.toString(), m_groupName, m_moduleName);
    }

    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == QLatin1String("param"))
            readParam();
        else if (m_reader.name() == QLatin1String("list"))
            readParam();
        else if (m_reader.name() == QLatin1String("map"))
            readParam();
        else
            m_reader.raiseError(QObject::tr("Expected a param, a list or a map in group %1, in module %2 but got a %3.").arg(m_groupName).arg(m_moduleName).arg(m_reader.name()));
    }
    m_groupName = "";

}

//       <param key="eng" value="6"/>
void XmlConfigurationFileReaderPrivate::readParam()
{
  auto key = m_reader.attributes().value(paramKeyAttribute());
  auto value = m_reader.attributes().value(paramValueAttribute());
  XMLCFGLOGINIT;
  LTRACE << "XmlConfigurationFileReader::readParam" << key << value;
  m_configuration.addParamValuePairForModuleAndGroup(key.toString(), value.toString(), m_moduleName, m_groupName);
}

//       <list name="available">
//         <item value="eng" />
//         …
//       </list>
void XmlConfigurationFileReaderPrivate::readList()
{
  m_listName = m_reader.attributes().value(listNameAttribute()).toString();
  m_firstItem=true;
  m_itemWithAttributes=false;
  XMLCFGLOGINIT;
  LTRACE << "XmlConfigurationFileReader::readList" << m_listName;
  while (m_reader.readNextStartElement()) {
    if (m_reader.name() == QLatin1String("item"))
      readListItem();
    else
      m_reader.raiseError(QObject::tr("Expected a list item in list %1 in group %2, in module %3 but got a %4.")
        .arg(m_listName).arg(m_groupName).arg(m_moduleName).arg(m_reader.name()));
  }
  m_listName = "";
}

//         <item value="eng" />
void XmlConfigurationFileReaderPrivate::readListItem()
{
  auto value = m_reader.attributes().value(listItemValueAttribute());
  XMLCFGLOGINIT;
  LTRACE << "XmlConfigurationFileReader::readListItem" << value;

  auto nbAtt = m_reader.attributes().length();
  if (m_firstItem)
  {
    // decide if list is simple list or list of items with attributes
    if (nbAtt==1)
    {
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
    auto itemName = m_reader.attributes().value(listItemValueAttribute());
    ItemWithAttributes item(itemName.toString());
    for (const auto& attribute: m_reader.attributes()) {
      item.addAttribute(attribute.name().toString(), attribute.value().toString());
    }
    m_configuration.addItemInListOfItemsForModuleAndGroup(item, m_listName, m_moduleName, m_groupName);
  }
  else {
    auto value = m_reader.attributes().value(listItemValueAttribute());
    m_configuration.addItemInListNamedForModuleAndGroup(value.toString(), m_listName, m_moduleName, m_groupName);
  }

}

//       <map name="conceptTypes">
//         <entry key="LatticeDown" value="0"/>
//         …
//       </map>
void XmlConfigurationFileReaderPrivate::readMap()
{
  m_mapName = m_reader.attributes().value(mapNameAttribute()).toString();
  XMLCFGLOGINIT;
  LTRACE << "map name is " << m_mapName;
  m_firstItem=true;
  m_itemWithAttributes=false;
  while (m_reader.readNextStartElement()) {
    if (m_reader.name() == QLatin1String("entry"))
      readMapEntry();
    else
      m_reader.raiseError(QObject::tr("Expected a map entry in map %1 in group %2, in module %3 but got a %4.")
        .arg(m_mapName).arg(m_groupName).arg(m_moduleName).arg(m_reader.name()));
  }
  m_mapName = "";
}

//         <entry key="LatticeDown" value="0"/>
void XmlConfigurationFileReaderPrivate::readMapEntry()
{
  XMLCFGLOGINIT;
  LTRACE << "XmlConfigurationFileReaderPrivate::readMapEntry";

  auto nbAtt = m_reader.attributes().length();
  if (m_firstItem)
  {
    // decide if map is simple map or map of entries with attributes
    if (nbAtt==2)
    { // name+value => simple map
      LTRACE << "add map list "<< m_mapName;
      m_configuration.addMapNamedForModuleAndGroup(m_mapName, m_moduleName, m_groupName);
    }
    else
    {
      LTRACE << "add map of items with attributes "<< m_mapName;
      m_configuration.addMapOfItemsForModuleAndGroup(m_mapName, m_moduleName, m_groupName);
      m_itemWithAttributes = true;
    }
    m_firstItem = false;
  }
  else if (nbAtt>2 && !m_itemWithAttributes)
  {
    // was indeed in list of item with attributes => has to change
    m_configuration.changeMapToMapOfItems(m_mapName, m_moduleName, m_groupName);
    m_itemWithAttributes = true;
  }

  if (m_itemWithAttributes)
  {
    auto key = m_reader.attributes().value(mapEntryKeyAttribute());
    auto value = m_reader.attributes().value(mapEntryValueAttribute());
    ItemWithAttributes item(value.toString());
    for (const auto& attribute: m_reader.attributes())
    {
      auto attName = attribute.name();
      auto value = attribute.value();
      if (attName != "key" && attName != "value")
      {
        item.addAttribute(attName.toString(), value.toString());
      }
    }
    m_configuration.addEntryInMapOfItemsForModuleAndGroup(key.toString(), item, m_mapName, m_moduleName, m_groupName);
  }
  else
  {
    auto key = m_reader.attributes().value(mapEntryKeyAttribute());
    auto value = m_reader.attributes().value(mapEntryValueAttribute());
    m_configuration.addEntryInMapNamedForModuleAndGroup(key.toString(), value.toString(), m_mapName, m_moduleName, m_groupName);
  }
}


QString XmlConfigurationFileReader::errorString() const
{
  XMLCFGLOGINIT;
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_d->m_reader.errorString())
          .arg(m_d->m_reader.lineNumber())
          .arg(m_d->m_reader.columnNumber());
  LERROR << errorStr;
  return errorStr;
}

} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima
