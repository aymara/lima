// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       abstractBowDocumentHandler.h
 * @author     Mesnard Olivier (olivier.mesnard@.cea.fr)
 * @date       Wed Oct  12 2005
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     BagOfWords
 * 
 * @brief      handler to deal with parts of structured BoW documents
 * 
 * 
 ***********************************************************************/

#ifndef ABSTRACTBOWDOCUMENTHANDLER_H
#define ABSTRACTBOWDOCUMENTHANDLER_H

#include <string>
#include <vector>

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/Handler/AbstractDocumentHandler.h"

namespace Lima {
namespace Common {
namespace Misc {
  class GenericDocumentProperties;
}
namespace BagOfWords {

class BoWText;
  
class LIMA_BOW_EXPORT AbstractBoWDocumentHandler : public AbstractDocumentHandler
{
 public:
   virtual ~AbstractBoWDocumentHandler() {}

   virtual void openSBoWNode(const Lima::Common::Misc::GenericDocumentProperties* properties,
                             const std::string& elementName) = 0;

   virtual void openSBoWIndexingNode(const Lima::Common::Misc::GenericDocumentProperties* properties,
                                     const std::string& elementName) = 0;

   virtual void processSBoWText(const BoWText* boWText, 
                                bool useIterators,
                                bool useIndexIterator) = 0;

   virtual void processProperties(const Misc::GenericDocumentProperties* properties, 
                                  bool useIterators,
                                  bool useIndexIterator) override = 0;

   virtual void closeSBoWNode() = 0;
   
//   virtual void writeDocumentsHeader() = 0;
//   virtual void writeDocumentsFooter() = 0;
  
  void openSNode(const Lima::Common::Misc::GenericDocumentProperties* properties,
                 const std::string& elementName) override 
  {
    openSBoWNode(properties,elementName);
  }

  void openSIndexingNode(const Lima::Common::Misc::GenericDocumentProperties* properties,
                         const std::string& elementName) override 
  {
    openSBoWIndexingNode(properties,elementName);
  }

  void closeSNode() override 
  {
    closeSBoWNode();
  }

  void processSContent( const Lima::Common::Misc::GenericDocumentProperties* /*properties*/) override 
  {
  }
  
};

} // end namespace
} // end namespace
} // end namespace

#endif
