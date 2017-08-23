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
