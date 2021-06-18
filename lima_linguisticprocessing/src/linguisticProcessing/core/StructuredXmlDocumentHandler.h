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
