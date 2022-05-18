// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                       *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_PROPERTYCODE_XMLPROPERTYHANDLER_H
#define LIMA_COMMON_PROPERTYCODE_XMLPROPERTYHANDLER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <QtXml/QXmlDefaultHandler>

#include <string>
#include <vector>
#include <map>

namespace Lima
{
namespace Common
{
namespace PropertyCode
{

class LIMA_PROPERTYCODE_EXPORT XMLPropertyHandler : public QXmlDefaultHandler
{
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  XMLPropertyHandler();
  virtual ~XMLPropertyHandler();


  // -----------------------------------------------------------------------
  //  Implementations of the SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  bool endDocument() override;
  
  bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName) override;
  
  bool startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & attributes) override;
  
  
  struct PropertyDescription
  {
    std::string name;
    std::vector<std::string> values;
  };

  struct SubPropertyDescription
  {
    std::string name;
    std::string parentName;
    std::vector<std::pair<std::string,std::vector<std::string> > > values;
  };

  const std::vector<PropertyDescription>& getProperties();
  const std::vector<SubPropertyDescription>& getSubProperties();

private:

  std::vector<PropertyDescription> m_properties;
  std::vector<SubPropertyDescription> m_subproperties;

  enum  PropType {NONE,PROP,SUBPROP};
  PropType m_currentProp;

};

} // closing namespace PropertyCode
} // closing namespace Common
} // closing namespace Lima

#endif
