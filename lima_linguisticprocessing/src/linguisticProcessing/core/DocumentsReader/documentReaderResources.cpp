/*
    Copyright 2004-2021 CEA LIST

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

#include "documentReaderResources.h"

#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;


namespace Lima
{
namespace DocumentsReader {


DocumentsReaderResources::DocumentsReaderResources()
    : m_objectManager(0) {}

DocumentsReaderResources::~DocumentsReaderResources()
{
  if( m_objectManager )
    delete m_objectManager;
}

StructuredDocumentXMLParser* DocumentsReaderResources::getXmlParser(const std::string& id)
  const
{
  AbstractReaderResource* res = m_objectManager->getObject(id);
  StructuredDocumentXMLParser* xmlParserPtr = static_cast<StructuredDocumentXMLParser*>(res);
  return xmlParserPtr;
}

DocumentPropertyType* DocumentsReaderResources::getProperty(const std::string& id)
  const
{
  AbstractReaderResource* res = m_objectManager->getObject(id);
  DocumentPropertyType* attributePtr = dynamic_cast<DocumentPropertyType*>(res);
  return attributePtr;
}

void DocumentsReaderResources::init(
  Lima::Common::XMLConfigurationFiles::ModuleConfigurationStructure& confModule)
{
  ReaderResourceInitializationParameters params;
  m_objectManager=new AbstractReaderResource::Manager(confModule,params);
}

} // DocumentsReader
} // Lima

