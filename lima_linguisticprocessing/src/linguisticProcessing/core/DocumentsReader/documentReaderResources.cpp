// Copyright 2004-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "documentReaderResources.h"

#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;


namespace Lima
{
namespace DocumentsReader {


DocumentsReaderResources::DocumentsReaderResources()
    : m_objectManager() {}

DocumentsReaderResources::~DocumentsReaderResources()
{
  // if( m_objectManager )
  //   delete m_objectManager;
}

std::shared_ptr<StructuredDocumentXMLParser> DocumentsReaderResources::getXmlParser(const std::string& id)
  const
{
  auto res = m_objectManager->getObject(id);
  auto xmlParserPtr = std::dynamic_pointer_cast<StructuredDocumentXMLParser>(res);
  return xmlParserPtr;
}

std::shared_ptr<DocumentPropertyType> DocumentsReaderResources::getProperty(const std::string& id)
  const
{
  auto res = m_objectManager->getObject(id);
  auto attributePtr = std::dynamic_pointer_cast<DocumentPropertyType>(res);
  return attributePtr;
}

void DocumentsReaderResources::init(
  Lima::Common::XMLConfigurationFiles::ModuleConfigurationStructure& confModule)
{
  ReaderResourceInitializationParameters params;
  m_objectManager=std::make_unique<AbstractReaderResource::Manager>(confModule,params);
}

} // DocumentsReader
} // Lima

