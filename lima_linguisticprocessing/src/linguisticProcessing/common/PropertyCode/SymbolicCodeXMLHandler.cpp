/*
    Copyright 2002-2013 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                       *
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
    PROPERTYCODELOGINIT;
    LDEBUG << "write code " << m_currentCode << LENDL;
    if (m_outputMap.find(m_currentCode)!=m_outputMap.end()) {
      LWARN << m_currentCode << " already exists ! overwrite it" << LENDL;
    }
    m_outputMap[m_currentCode]=m_propCodeManager.encode(m_currentPropertyValues);
  }
  return true;
}

bool SymbolicCodeXMLHandler::startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & attributes)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  PROPERTYCODELOGINIT;
  string stringName=name.toUtf8().data();
  if (stringName == "code")
  {
    m_currentCode=attributes.value("key").toUtf8().data();
    m_currentPropertyValues.clear();
    LDEBUG << "read code " << m_currentCode << LENDL;
  }
  else if (stringName == "field")
  {
    string key=attributes.value("key").toUtf8().data();
    string value=attributes.value("value").toUtf8().data();
    LDEBUG << "read field key=" << key << " value=" << value << LENDL;
    if (value!="")
    {
      m_currentPropertyValues[key]=value;
    }
  }
  return true;
}
