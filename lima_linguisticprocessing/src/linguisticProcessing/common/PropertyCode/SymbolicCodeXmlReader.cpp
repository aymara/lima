// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "SymbolicCodeXmlReader.h"
#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h"

#include <QXmlStreamReader>

using namespace Lima;
using namespace Lima::Common::PropertyCode;

class LIMA_PROPERTYCODE_EXPORT SymbolicCodeXmlReaderPrivate
{
  friend class SymbolicCodeXmlReader;
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  SymbolicCodeXmlReaderPrivate(const Lima::Common::PropertyCode::PropertyCodeManager& pcm,
                               std::map<std::string,Lima::LinguisticCode>& outputMap);
  ~SymbolicCodeXmlReaderPrivate() = default;

  bool parse(QIODevice *device);
  void readCodes();
  void readCode();
  void readField();

  QXmlStreamReader m_reader;
  QString m_currentCode;
  std::map<std::string, std::string> m_currentPropertyValues;
  std::map<std::string,Lima::LinguisticCode>& m_outputMap;
  const Lima::Common::PropertyCode::PropertyCodeManager& m_propCodeManager;
};

SymbolicCodeXmlReaderPrivate::SymbolicCodeXmlReaderPrivate(const PropertyCodeManager& pcm,
                                                           std::map<std::string,LinguisticCode>& outputMap) :
    m_currentCode(),
    m_currentPropertyValues(),
    m_outputMap(outputMap),
    m_propCodeManager(pcm)
{}

SymbolicCodeXmlReader::SymbolicCodeXmlReader(const PropertyCodeManager& pcm,
                                             std::map<std::string,LinguisticCode>& outputMap) :
    m_d(new SymbolicCodeXmlReaderPrivate(pcm, outputMap))
{}

SymbolicCodeXmlReader::~SymbolicCodeXmlReader()
{
  delete m_d;
}

bool SymbolicCodeXmlReader::parse(QIODevice *device)
{
  return m_d->parse(device);
}

// <?xml version='1.0' encoding='UTF-8'?>
// <codes>
//   <code key="">
//     <field key="MICRO" value=""/>
//   </code>
// </codes>
bool SymbolicCodeXmlReaderPrivate::parse(QIODevice *device)
{
  PROPERTYCODELOGINIT;
  LDEBUG << "SymbolicCodeXmlReaderPrivate::parse";
  m_reader.setDevice(device);
  if (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("codes"))
      {
          readCodes();
      }
      else
      {
          m_reader.raiseError(QObject::tr("The file is not a LIMA XML symbolic codes file."));
      }
  }
  return !m_reader.error();
}

// <?xml version='1.0' encoding='UTF-8'?>
// <codes>
// …
// </codes>
void SymbolicCodeXmlReaderPrivate::readCodes()
{
  PROPERTYCODELOGINIT;
  LTRACE << "SymbolicCodeXmlReader::readCodes" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("codes"));

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("code"))
          readCode();
      else
          m_reader.raiseError(QObject::tr("Expected a code but got a %1.").arg(m_reader.name()));
  }
}

//   <code key="">
//     <field key="MICRO" value=""/>
//     …
//   </code>
void SymbolicCodeXmlReaderPrivate::readCode()
{
  PROPERTYCODELOGINIT;
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("code"));

  m_currentCode = m_reader.attributes().value("key").toString();
  m_currentPropertyValues.clear();
  LTRACE << "SymbolicCodeXmlReader::readCode" << m_reader.name() << m_currentCode;
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("field"))
          readField();
      else
          m_reader.raiseError(QObject::tr("Expected a field but got a %1.").arg(m_reader.name()));
  }
  if (m_outputMap.find(m_currentCode.toStdString()) != m_outputMap.end())
  {
    PROPERTYCODELOGINIT;
    LWARN << m_currentCode << " already exists! overwrite it";
  }
  m_outputMap[m_currentCode.toStdString()] = m_propCodeManager.encode( m_currentPropertyValues);
}

//     <field key="MICRO" value=""/>
void SymbolicCodeXmlReaderPrivate::readField()
{
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
  LTRACE << "SymbolicCodeXmlReader::readField" << m_reader.name()
          << m_reader.attributes().value("key")
          << m_reader.attributes().value("value");
#endif
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("field"));
  auto key = m_reader.attributes().value("key").toString();
  auto value = m_reader.attributes().value("value").toString();
  if (value != "")
  {
    m_currentPropertyValues[key.toStdString()] = value.toStdString();
  }
  m_reader.skipCurrentElement();
}

QString SymbolicCodeXmlReader::errorString() const
{
  PROPERTYCODELOGINIT;
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_d->m_reader.errorString())
          .arg(m_d->m_reader.lineNumber())
          .arg(m_d->m_reader.columnNumber());
  LERROR << errorStr;
  return errorStr;
}
