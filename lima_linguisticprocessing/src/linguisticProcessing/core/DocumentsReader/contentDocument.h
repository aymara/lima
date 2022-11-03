// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file   contentDocument.h
 * @author Besancon Romaric
 * @date   Wed Oct 29 13:22:02 2003
 *
 * @brief  the representation of the content parts of a document,
 * and its meta-data
 *
 *
 ***********************************************************************/

#include "documentsreader_export.h"
#include "documentProperty.h"
#include "linguisticProcessing/core/StructuredXmlDocumentHandler.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/Data/LimaString.h"
#include "common/Data/genericDocumentProperties.h"

#include <deque>
#include <string>

#ifndef CONTENTDOCUMENT_H
#define CONTENTDOCUMENT_H

namespace Lima {
namespace DocumentsReader {

class AbstractStructuredDocumentElement;
class DiscardableDocumentElement;
class IndexingDocumentElement;
class HierarchyDocumentElement;
class DocumentPropertyElement;
class PresentationDocumentElement;
class IgnoredDocumentElement;

/**
 * Partie d'un document structure decodee avant analyse
 */
class ContentStructuredDocument : public std::deque<AbstractStructuredDocumentElement*>,
    public Lima::Common::Misc::GenericDocumentProperties
{
public:
  ContentStructuredDocument() :
      std::deque<AbstractStructuredDocumentElement*>(),
      Lima::Common::Misc::GenericDocumentProperties()
  {
  }

  ~ContentStructuredDocument();

  /// retourne la position courante byte du dernier element, pour initialiser le suivant
  unsigned int getOffset();

  /// Ajoute un couple (texte,offset) au dernier element
  void addPart(const Lima::LimaString& text, const unsigned int offset);
  /// Ajoute un element a la pile de type noeud discardable
  DiscardableDocumentElement* pushDiscardableChild( const QString& elementName, unsigned int parserOffset );
  /// Ajoute un element a la pile de type propriete du noeud fils
  DocumentPropertyElement* addPropertyChild( const QString& elementName, unsigned int parserOffset,
    const DocumentPropertyType& type );
  /// Ajoute un element a la pile de type hierarchy
  HierarchyDocumentElement* pushHierarchyChild( const QString& elementName, unsigned int parserOffset, const DocumentPropertyType& propType );
  IndexingDocumentElement* pushIndexingChild( const QString& elementName, unsigned int parserOffset, const DocumentPropertyType& propType );
  IgnoredDocumentElement* pushIgnoredChild( const QString& elementName, unsigned int parserOffset, const DocumentPropertyType& propType  );
  /// Ajoute un element a la pile de type presentation
  PresentationDocumentElement* pushPresentationChild( const QString& elementName, unsigned int parserOffset );

  void popDiscardableElement(unsigned int parserOffset);
  void popIndexingElement( unsigned int parserOffset );
  void popPresentationElement( unsigned int parserOffset );
  void popPropertyElement(unsigned int parserOffset);
  void popHierarchyElement(unsigned int parserOffset);
  void popIgnoredElement(unsigned int parserOffset);

//   void reportTextOfLastElement(  );
  void setDataToPreviousElement( StructuredXmlDocumentHandler* processor );
  void setDataToLastElement( const DocumentPropertyType& property,
    const std::string& data, StructuredXmlDocumentHandler* processor );
  void setDataToElement( AbstractStructuredDocumentElement* absElement,
    const DocumentPropertyType& property, const std::string& data,
    StructuredXmlDocumentHandler* processor );
  static void parseDate(const std::string& dateStr,
                        QDate& dateBegin,
                        QDate& dateEnd);

  static void parseWeightedValue(const std::string& weightedValueStr,
                                    std::string& value,
                                    float& score);
};

} // namespace DocumentsReader
} // namespace Lima

#endif
