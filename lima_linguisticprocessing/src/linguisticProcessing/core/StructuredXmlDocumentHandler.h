// Copyright 2004-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSINGSTRUCTURED_XMLDOCUMENTHANDLER_H
#define LIMA_LINGUISTICPROCESSINGSTRUCTURED_XMLDOCUMENTHANDLER_H

#include "common/Data/LimaString.h"

#include <string>

namespace Lima
{

// forward declaration of ContentStructuredDocument
namespace DocumentsReader {
  class ContentStructuredDocument;
  class DocumentPropertyType;
}

//! @brief Handler pour les elements de parsing de document structure
class StructuredXmlDocumentHandler {
public:
  virtual ~StructuredXmlDocumentHandler() {}
  virtual void startHierarchy( const DocumentsReader::ContentStructuredDocument& contentDocument) = 0;
  virtual void endHierarchy( const DocumentsReader::ContentStructuredDocument& contentDocument) = 0;
  virtual void startIndexing( const DocumentsReader::ContentStructuredDocument& contentDocument) = 0;
  virtual void endIndexing( const DocumentsReader::ContentStructuredDocument& contentDocument) = 0;
  virtual void handle( const DocumentsReader::ContentStructuredDocument& contentDocument,
    const Lima::LimaString& text, unsigned long int offset, std::string langOfAnalysis) = 0;
  virtual void handleProperty( const DocumentsReader::DocumentPropertyType& property,
    const std::string& data) = 0;
};

}

#endif
