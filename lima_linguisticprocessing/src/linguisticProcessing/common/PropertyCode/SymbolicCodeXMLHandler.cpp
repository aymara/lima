// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                                        *
 *                                                                         *
 ***************************************************************************/

#include "SymbolicCodeXMLHandler.h"
#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h"


using namespace std;
using namespace Lima;
using namespace Lima::Common::PropertyCode;

SymbolicCodeXMLHandler::SymbolicCodeXMLHandler(const PropertyCodeManager& pcm,std::map<std::string,LinguisticCode>& outputMap) :
    m_currentCode(),
    m_currentPropertyValues(),
    m_outputMap(outputMap),
    m_propCodeManager(pcm)
{}

SymbolicCodeXMLHandler::~SymbolicCodeXMLHandler() {}

// -----------------------------------------------------------------------
//  Implementations of the SAX DocumentHandler interface
// -----------------------------------------------------------------------
bool SymbolicCodeXMLHandler::endElement(const QString & namespaceURI, const QString & name, const QString & qName)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  string stringName=name.toUtf8().data();
  if (stringName == "code")
  {
#ifdef DEBUG_LP
    PROPERTYCODELOGINIT;
    LDEBUG << "write code " << m_currentCode;
#endif
    if (m_outputMap.find(m_currentCode)!=m_outputMap.end()) {
      PROPERTYCODELOGINIT;
      LWARN << m_currentCode << " already exists! overwrite it";
    }
    m_outputMap[m_currentCode]=m_propCodeManager.encode(m_currentPropertyValues);
  }
  return true;
}

bool SymbolicCodeXMLHandler::startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & attributes)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
#endif
  string stringName=name.toUtf8().data();
  if (stringName == "code")
  {
    m_currentCode=attributes.value("key").toUtf8().data();
    m_currentPropertyValues.clear();
#ifdef DEBUG_LP
    LDEBUG << "read code " << m_currentCode;
#endif
  }
  else if (stringName == "field")
  {
    string key=attributes.value("key").toUtf8().data();
    string value=attributes.value("value").toUtf8().data();
#ifdef DEBUG_LP
    LDEBUG << "read field key=" << key << " value=" << value;
#endif
    if (value!="")
    {
      m_currentPropertyValues[key]=value;
    }
  }
  return true;
}
