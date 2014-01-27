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

#include "XMLPropertyHandler.h"
#include "common/LimaCommon.h"

using namespace std;

namespace Lima
{
namespace Common
{
namespace PropertyCode
{

XMLPropertyHandler::XMLPropertyHandler() : m_currentProp(NONE) {}

XMLPropertyHandler::~XMLPropertyHandler() {}

// -----------------------------------------------------------------------
//  Implementations of the SAX DocumentHandler interface
// -----------------------------------------------------------------------
bool XMLPropertyHandler::endDocument()
{  return true;
}

bool XMLPropertyHandler::endElement(const QString & namespaceURI, const QString & name, const QString & qName)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  const QString& stringName = name;
  if (m_currentProp==PROP && stringName == "property")
  {
    m_currentProp=NONE;
  }
  else if (m_currentProp==SUBPROP && stringName == "subproperty")
  {
    m_currentProp=NONE;
  }
  return true;
}

bool XMLPropertyHandler::startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & attributes)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  PROPERTYCODELOGINIT;
  const QString& stringName = name;
  if (stringName == "property")
  {
      m_currentProp=PROP;
      m_properties.push_back(PropertyDescription());
      m_properties.back().name = attributes.value("name").toUtf8().data();
      LDEBUG << "read property " << m_properties.back().name << LENDL;
  }
  else if (stringName == "subproperty")
  {
    m_currentProp=SUBPROP;
    m_subproperties.push_back(SubPropertyDescription());
    m_subproperties.back().name = attributes.value("name").toUtf8().data();
    m_subproperties.back().parentName = attributes.value("parent").toUtf8().data();
    LDEBUG << "read subproperty " << m_subproperties.back().name << " of parent property " << m_subproperties.back().parentName << LENDL;
  }
  else if (stringName == "value")
  {
    string value=attributes.value("name").toUtf8().data();
    LDEBUG << "read value " << value << LENDL;
    if (m_currentProp == PROP)
    {
      m_properties.back().values.push_back(value);
    }
    else if (m_currentProp == SUBPROP)
    {
      m_subproperties.back().values.back().second.push_back(value);
    }
    else
    {
      LERROR << "Don't know what to do with value " << value << " !" << LENDL;
    }
  }
  else if (stringName == "subvalues")
  {
    string value=attributes.value("value").toUtf8().data();
    LDEBUG << "read subvalues " << value << LENDL;
    if (m_currentProp == SUBPROP)
    {
      m_subproperties.back().values.push_back(make_pair(value,vector<string>()));
    }
    else
    {
      LERROR << "Don't know what to do with subvalues " << value << " !" << LENDL;
    }
  }
  return true;
}


const std::vector<XMLPropertyHandler::PropertyDescription>& XMLPropertyHandler::getProperties()
{
  return m_properties;
}

const std::vector<XMLPropertyHandler::SubPropertyDescription>& XMLPropertyHandler::getSubProperties()
{
  return m_subproperties;
}

} // closing namespace PropertyCode
} // closing namespace Common
} // closing namespace Lima
