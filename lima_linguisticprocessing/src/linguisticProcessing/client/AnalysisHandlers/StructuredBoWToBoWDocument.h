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
 * @file       StructuredBoWToBoWDocument.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Mon Mar  6 2006
 * @version    $Id$
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     LinguisticProcessing
 * 
 * @brief      handler to transform XML-structured bow document to fit in a standard bowDocument
 * 
 * 
 ***********************************************************************/

#ifndef STRUCTUREDBOWTOBOWDOCUMENT_H
#define STRUCTUREDBOWTOBOWDOCUMENT_H

#include "AnalysisHandlersExport.h"
#include "linguisticProcessing/common/BagOfWords/AbstractBoWDocumentHandler.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "common/Data/genericDocumentProperties.h"

namespace Lima {
namespace LinguisticProcessing {

class LIMA_ANALYSISHANDLERS_EXPORT StructuredBoWToBoWDocument :
  public Common::BagOfWords::AbstractBoWDocumentHandler
{
 public:
  StructuredBoWToBoWDocument(std::vector<Common::BagOfWords::BoWDocument>* bowDocs); 
  virtual ~StructuredBoWToBoWDocument();
  
  void openSBoWNode(const Common::Misc::GenericDocumentProperties* properties,
                    const std::string& elementName);
  
  void openSBoWIndexingNode(const Common::Misc::GenericDocumentProperties* properties,
                            const std::string& elementName);

  void processSBoWText(const Common::BagOfWords::BoWText* boWText, 
                       bool useIterators, bool useIndexIterator);

  void processProperties(const Common::Misc::GenericDocumentProperties* properties, 
                         bool useIterators, bool useIndexIterator);
  void closeSBoWNode();

 private:
  std::vector<Common::BagOfWords::BoWDocument>* m_bowDocs;
  std::vector<bool> m_inIndexingNode;
  Common::BagOfWords::BoWDocument* m_currentDocument;

  // private member functions
  void addProperties(Common::BagOfWords::BoWDocument& document,
                     const Common::Misc::GenericDocumentProperties* properties);
  
};
 
} // end namespace
} // end namespace

#endif
