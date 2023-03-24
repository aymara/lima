// Copyright 2004-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_XMLREADER_DOCUMENTSREADER_DOCUMENTSREADERRESOURCES_H
#define LIMA_LINGUISTICPROCESSING_XMLREADER_DOCUMENTSREADER_DOCUMENTSREADERRESOURCES_H

#include "documentsreader_export.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/AbstractFactoryPattern/Singleton.h"
#include "linguisticProcessing/core/abstractReaderResource.h"
#include "structuredDocumentXMLParser.h"
#include "documentProperty.h"

namespace Lima
{
namespace DocumentsReader {

/**
  * @brief singleton that holds all resources about documents reader
  */
class DOCUMENTSREADER_EXPORT DocumentsReaderResources : public Lima::Singleton<DocumentsReaderResources>
{
friend class Lima::Singleton<DocumentsReaderResources>;

public:

  virtual ~DocumentsReaderResources();

  std::shared_ptr<StructuredDocumentXMLParser> getXmlParser( const std::string& id) const;
  std::shared_ptr<DocumentPropertyType> getProperty( const std::string& id) const;
  void init( Lima::Common::XMLConfigurationFiles::ModuleConfigurationStructure& confModule);
private:

  DocumentsReaderResources();

  std::unique_ptr<AbstractReaderResource::Manager> m_objectManager;

};


} // DocumentsReader
} // Lima

#endif


