// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
#endif
  const QString& stringName = name;
  if (stringName == "property")
  {
      m_currentProp=PROP;
      m_properties.push_back(PropertyDescription());
      m_properties.back().name = attributes.value("name").toUtf8().data();
#ifdef DEBUG_LP
      LDEBUG << "read property " << m_properties.back().name;
#endif
  }
  else if (stringName == "subproperty")
  {
    m_currentProp=SUBPROP;
    m_subproperties.push_back(SubPropertyDescription());
    m_subproperties.back().name = attributes.value("name").toUtf8().data();
    m_subproperties.back().parentName = attributes.value("parent").toUtf8().data();
#ifdef DEBUG_LP
    LDEBUG << "read subproperty " << m_subproperties.back().name << " of parent property " << m_subproperties.back().parentName;
#endif
  }
  else if (stringName == "value")
  {
    string value=attributes.value("name").toUtf8().data();
#ifdef DEBUG_LP
    LDEBUG << "read value " << value;
#endif
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
      PROPERTYCODELOGINIT;
      LERROR << "Don't know what to do with value " << value << " !";
    }
  }
  else if (stringName == "subvalues")
  {
    string value=attributes.value("value").toUtf8().data();
#ifdef DEBUG_LP
    LDEBUG << "read subvalues " << value;
#endif
    if (m_currentProp == SUBPROP)
    {
      m_subproperties.back().values.push_back(make_pair(value,vector<string>()));
    }
    else
    {
      PROPERTYCODELOGINIT;
      LERROR << "Don't know what to do with subvalues " << value << " !";
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
