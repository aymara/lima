/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST - LIC2M                              *
 *                                                                         *
 ***************************************************************************/

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
} // FrCeaLic2m

