// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                       *
 *                                                                         *
 ***************************************************************************/

#ifndef SYMBOLICCODEXMLHANDLER_H
#define SYMBOLICCODEXMLHANDLER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include<QtXml/QXmlDefaultHandler>

#include <string>
#include <vector>
#include <map>

#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"

class LIMA_PROPERTYCODE_EXPORT SymbolicCodeXMLHandler : public QXmlDefaultHandler
{
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  SymbolicCodeXMLHandler(const Lima::Common::PropertyCode::PropertyCodeManager& pcm,std::map<std::string,Lima::LinguisticCode>& outputMap);
  virtual ~SymbolicCodeXMLHandler();


  // -----------------------------------------------------------------------
  //  Implementations of the SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  bool endElement(const QString & namespaceURI, const QString & name, const QString & qName) override;

  bool startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & attributes) override;

private:

  std::string m_currentCode;
  std::map<std::string,std::string> m_currentPropertyValues;
  std::map<std::string,Lima::LinguisticCode>& m_outputMap;
  const Lima::Common::PropertyCode::PropertyCodeManager& m_propCodeManager;
};

#endif
