// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
                    const std::string& elementName) override;
  
  void openSBoWIndexingNode(const Common::Misc::GenericDocumentProperties* properties,
                            const std::string& elementName) override;

  void processSBoWText(const Common::BagOfWords::BoWText* boWText, 
                       bool useIterators, bool useIndexIterator) override;

  void processProperties(const Common::Misc::GenericDocumentProperties* properties, 
                         bool useIterators, bool useIndexIterator) override;
  void closeSBoWNode() override;

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
