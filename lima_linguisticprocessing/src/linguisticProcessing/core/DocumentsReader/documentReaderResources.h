/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST- LIC2M                               *
 *                                                                         *
 ***************************************************************************/


#ifndef FRCEALIC2M_LINGUISTICPROCESSING_XMLREADER_DOCUMENTSREADER_DOCUMENTSREADERRESOURCES_H
#define FRCEALIC2M_LINGUISTICPROCESSING_XMLREADER_DOCUMENTSREADER_DOCUMENTSREADERRESOURCES_H

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
} // FrCeaLic2m

#endif


