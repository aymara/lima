// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  void writeDocumentsHeader() override {writeBoWDocumentsHeader();};
  void writeDocumentsFooter() override {writeBoWDocumentsFooter();};
   
  // Implementation of AbstractBoWXMLWriter functions
  void openSBoWNode(const Lima::Common::Misc::GenericDocumentProperties* properties, 
                    const std::string& elementName) override;
  void openSBoWIndexingNode(const Lima::Common::Misc::GenericDocumentProperties* properties, 
                            const std::string& elementName) override;
  void processSBoWText(const BoWText* boWText, 
                       bool useIterators,
                       bool useIndexIterator) override;
  void processProperties(const Misc::GenericDocumentProperties* properties, 
                         bool useIterators,
                         bool useIndexIterator) override;
  void closeSBoWNode() override;
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
