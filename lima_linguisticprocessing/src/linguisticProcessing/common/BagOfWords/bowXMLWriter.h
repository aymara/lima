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
/************************************************************************
 *
 * @file       bowXMLWriter.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed May  5 2004
 * copyright   Copyright (C) 2004 by CEA LIST
 * Project     BagOfWords
 * 
 * @brief      XML input/output for BoW elements
 * 
 * 
 ***********************************************************************/

#ifndef BOWXMLWRITER_H
#define BOWXMLWRITER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "AbstractBoWDocumentHandler.h"

#include <string>
#include <iostream>

namespace Lima {
namespace Common {
namespace BagOfWords {


class BoWXMLWriterPrivate;
class BoWToken;
class BoWRelation;
class BoWText;
class BoWDocument;
class IndexElement;

class LIMA_BOW_EXPORT BoWXMLWriter :  public AbstractBoWDocumentHandler
{
 public:
// FWI 08/09/2015 : ajout de la langue en paramètre optionnel
#ifdef ANTINNO_SPECIFIC
   BoWXMLWriter(std::ostream& os, Lima::MediaId const& language);
#else
#endif
  BoWXMLWriter(std::ostream& os); 
  virtual ~BoWXMLWriter();

  void writeBoWText(const BoWText* document,
                    const bool useIterator,
                    const bool useIndexIterator);
  void writeBoWToken(const BoWToken* token);
  void writeBoWDocument(const BoWDocument* document,
                        const bool useIterator,
                        const bool useIndexIterator);

  // root tags for valid XML if several documents
  void writeBoWDocumentsHeader();
  void writeBoWDocumentsFooter();
  void writeDocumentsHeader(){writeBoWDocumentsHeader();};
  void writeDocumentsFooter(){writeBoWDocumentsFooter();};
   
  // Implementation of AbstractBoWXMLWriter functions
  void openSBoWNode(const Lima::Common::Misc::GenericDocumentProperties* properties, const std::string& elementName);
  void openSBoWIndexingNode(const Lima::Common::Misc::GenericDocumentProperties* properties, const std::string& elementName);
  void processSBoWText(const BoWText* boWText, bool useIterators,
                         bool useIndexIterator);
  void processProperties(const Misc::GenericDocumentProperties* properties, bool useIterators,
                         bool useIndexIterator);
  void closeSBoWNode();
  void writeIndexElement(const IndexElement& element);
  void setSpaces(const std::string& s);
  void incIndent();
  void decIndent();
  
  
  void setLanguage(const Lima::MediaId lang);
  void setLanguage(const std::string& lang);
private:
  BoWXMLWriter(const BoWXMLWriter&);
  BoWXMLWriter& operator=(const BoWXMLWriter&);
  BoWXMLWriterPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
