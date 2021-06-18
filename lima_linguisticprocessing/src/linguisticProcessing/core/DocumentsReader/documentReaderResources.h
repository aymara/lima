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

  StructuredDocumentXMLParser* getXmlParser( const std::string& id) const;
  DocumentPropertyType* getProperty( const std::string& id) const;
  void init( Lima::Common::XMLConfigurationFiles::ModuleConfigurationStructure& confModule);
private:

  DocumentsReaderResources();

  AbstractReaderResource::Manager* m_objectManager;

};


} // DocumentsReader
} // Lima

#endif


