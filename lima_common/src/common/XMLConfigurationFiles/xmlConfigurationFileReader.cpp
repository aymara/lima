// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT
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
  void readListItem(const QString& listName);
  void readMap();
  void readMapEntry(const QString& mapName);

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


  /*
    * The structure where data will be stored
    */
  ConfigurationStructure& m_configuration;
};

XmlConfigurationFileReaderPrivate::XmlConfigurationFileReaderPrivate(ConfigurationStructure& theConfiguration) :
    m_reader(),
    m_moduleName(""), m_groupName(""), m_configuration(theConfiguration)
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
  if (m_reader.readNextStartElement())
  {
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

    while (m_reader.readNextStartElement())
    {
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
  while (m_reader.readNextStartElement())
  {
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
      if (attribute.name() == groupNameAttribute())
      {
        continue;
      }
      auto key = attribute.name();
      auto value = attribute.value();
      m_configuration.addAttributeForGroupInModule(key.toString(), value.toString(), m_groupName, m_moduleName);
    }

    while (m_reader.readNextStartElement())
    {
        if (m_reader.name() == QLatin1String("param"))
            readParam();
        else if (m_reader.name() == QLatin1String("list"))
            readList();
        else if (m_reader.name() == QLatin1String("map"))
            readMap();
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
  m_reader.skipCurrentElement();
}

//       <list name="available">
//         <item value="eng" />
//         …
//       </list>
void XmlConfigurationFileReaderPrivate::readList()
{
  auto listName = m_reader.attributes().value(listNameAttribute()).toString();
  XMLCFGLOGINIT;
  LTRACE << "XmlConfigurationFileReader::readList" << listName;
  m_configuration.addListNamedForModuleAndGroup(listName, m_moduleName, m_groupName);
  while (m_reader.readNextStartElement())
  {
    if (m_reader.name() == QLatin1String("item"))
      readListItem(listName);
    else
      m_reader.raiseError(QObject::tr("Expected a list item in list %1 in group %2, in module %3 but got a %4.")
        .arg(listName).arg(m_groupName).arg(m_moduleName).arg(m_reader.name()));
  }
}

//         <item value="eng" />
void XmlConfigurationFileReaderPrivate::readListItem(const QString& listName)
{
  auto value = m_reader.attributes().value(listItemValueAttribute());
  XMLCFGLOGINIT;
  LTRACE << "XmlConfigurationFileReader::readListItem" << value;
  m_configuration.addItemInListNamedForModuleAndGroup(value.toString(), listName, m_moduleName, m_groupName);
  m_reader.skipCurrentElement();
}

//       <map name="conceptTypes">
//         <entry key="LatticeDown" value="0"/>
//         …
//       </map>
void XmlConfigurationFileReaderPrivate::readMap()
{
  auto mapName = m_reader.attributes().value(mapNameAttribute()).toString();
  XMLCFGLOGINIT;
  LTRACE << " XmlConfigurationFileReaderPrivate::readMap" << mapName;
  while (m_reader.readNextStartElement())
  {
    if (m_reader.name() == QLatin1String("entry"))
      readMapEntry(mapName);
    else
      m_reader.raiseError(QObject::tr("Expected a map entry in map %1 in group %2, in module %3 but got a %4.")
        .arg(mapName).arg(m_groupName).arg(m_moduleName).arg(m_reader.name()));
  }
  LTRACE << "XmlConfigurationFileReaderPrivate::readMap add empty map"<< mapName;
  m_configuration.addMapNamedForModuleAndGroup(mapName, m_moduleName, m_groupName);
}

//         <entry key="LatticeDown" value="0"/>
void XmlConfigurationFileReaderPrivate::readMapEntry(const QString& mapName)
{
  XMLCFGLOGINIT;
  LTRACE << "XmlConfigurationFileReaderPrivate::readMapEntry";

  auto key = m_reader.attributes().value(mapEntryKeyAttribute());
  auto value = m_reader.attributes().value(mapEntryValueAttribute());
  m_configuration.addEntryInMapNamedForModuleAndGroup(
    key.toString(), value.toString(), mapName, m_moduleName, m_groupName);
  m_reader.skipCurrentElement();
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
