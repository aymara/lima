/*
    Copyright 2002-2022 CEA LIST

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

#include "XmlPropertyReader.h"
#include "common/LimaCommon.h"

#include <QXmlStreamReader>

namespace Lima
{
namespace Common
{
namespace PropertyCode
{

class XmlPropertyReaderPrivate
{
  friend class XmlPropertyReader;

public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  XmlPropertyReaderPrivate();
  ~XmlPropertyReaderPrivate() = default;

  bool parse(QIODevice *device);
  void readLinguisticCodes();
  void readProperty();
  void readValue();
  void readSubProperty();
  void readSubValues();
  void readSubValue();

  QXmlStreamReader m_reader;
  std::vector<XmlPropertyReader::PropertyDescription> m_properties;
  std::vector<XmlPropertyReader::SubPropertyDescription> m_subproperties;
};

XmlPropertyReaderPrivate::XmlPropertyReaderPrivate()
{
}

XmlPropertyReader::XmlPropertyReader() : m_d(new XmlPropertyReaderPrivate())
{
}

XmlPropertyReader::~XmlPropertyReader()
{
  delete m_d;
}

bool XmlPropertyReader::parse(QIODevice *device)
{
  return m_d->parse(device);
}

// <?xml version='1.0' encoding='UTF-8'?>
// <Linguistic_codes>
//   <property name="GENDER">
//     <value name="MASC"/>
//     …
//   </property>
//   <subproperty name="MICRO" parent="MACRO">
//     <subvalues value="ADJ">
//       <value name="ADJ"/>
//     </subvalues>
//    …
//   </subproperty>
//   …
// </Linguistic_codes>
bool XmlPropertyReaderPrivate::parse(QIODevice *device)
{
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
  LDEBUG << "XmlPropertyReaderPrivate::parse";
#endif
  m_reader.setDevice(device);
  if (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("Linguistic_codes"))
      {
          readLinguisticCodes();
      }
      else
      {
          m_reader.raiseError(QObject::tr("The file is not a LIMA XML linguistic properties codes file."));
      }
  }
  return !m_reader.error();
}

// <Linguistic_codes>
//   <property name="GENDER">
//     …
//   </property>
//   <subproperty name="MICRO" parent="MACRO">
//    …
//   </subproperty>
//   …
// </Linguistic_codes>
void XmlPropertyReaderPrivate::readLinguisticCodes()
{
  PROPERTYCODELOGINIT;
  LTRACE << "XmlPropertyReaderPrivate::readLinguisticCodes" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("Linguistic_codes"));

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("property"))
          readProperty();
      if (m_reader.name() == QLatin1String("subproperty"))
          readSubProperty();
      else
          m_reader.raiseError(QObject::tr("Expected a property or subproperty but got a %1.").arg(m_reader.name()));
  }
}

//   <property name="GENDER">
//     <value name="MASC"/>
//     …
//   </property>
void XmlPropertyReaderPrivate::readProperty()
{
  m_properties.push_back(XmlPropertyReader::PropertyDescription());
  m_properties.back().name = m_reader.attributes().value("name").toUtf8().data();
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
  LDEBUG << "read property " << m_properties.back().name;
#endif
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("value"))
          readValue();
      else
          m_reader.raiseError(QObject::tr("Expected a value but got a %1.").arg(m_reader.name()));
  }
}

//     <value name="MASC"/>
void XmlPropertyReaderPrivate::readValue()
{
  auto value = m_reader.attributes().value("name").toString();
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
  LDEBUG << "XmlPropertyReaderPrivate::readValue" << value;
#endif
  m_properties.back().values.push_back(value.toStdString());
}

//   <subproperty name="MICRO" parent="MACRO">
//     <subvalues value="ADJ">
//     …
//     </subvalues>
//    …
//   </subproperty>
void XmlPropertyReaderPrivate::readSubProperty()
{
  m_subproperties.push_back(XmlPropertyReader::SubPropertyDescription());
  m_subproperties.back().name = m_reader.attributes().value("name").toUtf8().data();
  m_subproperties.back().parentName = m_reader.attributes().value("parent").toUtf8().data();
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
  LDEBUG << "read subproperty " << m_subproperties.back().name << " of parent property " << m_subproperties.back().parentName;
#endif
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("subvalues"))
          readSubValues();
      else
          m_reader.raiseError(QObject::tr("Expected a subvalues but got a %1.").arg(m_reader.name()));
  }
}

//     <subvalues value="ADJ">
//       <value name="ADJ"/>
//       …
//     </subvalues>
void XmlPropertyReaderPrivate::readSubValues()
{
  auto value = m_reader.attributes().value("value").toUtf8().data();
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
  LDEBUG << "XmlPropertyReaderPrivate::readSubValues" << value;
#endif
  m_subproperties.back().values.push_back(make_pair(value, std::vector<std::string>()));
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("value"))
          readSubValue();
      else
          m_reader.raiseError(QObject::tr("Expected a subvalues but got a %1.").arg(m_reader.name()));
  }
}

//       <value name="ADJ"/>
void XmlPropertyReaderPrivate::readSubValue()
{
  auto value = m_reader.attributes().value("name").toString();
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
  LDEBUG << "XmlPropertyReaderPrivate::readSubValue" << value;
#endif
  m_subproperties.back().values.back().second.push_back(value.toStdString());
}


QString XmlPropertyReader::errorString() const
{
  PROPERTYCODELOGINIT;
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_d->m_reader.errorString())
          .arg(m_d->m_reader.lineNumber())
          .arg(m_d->m_reader.columnNumber());
  LERROR << errorStr;
  return errorStr;
}


const std::vector<XmlPropertyReader::PropertyDescription>& XmlPropertyReader::getProperties()
{
  return m_d->m_properties;
}

const std::vector<XmlPropertyReader::SubPropertyDescription>& XmlPropertyReader::getSubProperties()
{
  return m_d->m_subproperties;
}

} // closing namespace PropertyCode
} // closing namespace Common
} // closing namespace Lima
