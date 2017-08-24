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

#ifdef __GNUC__
#include <bits/allocator.h>
#endif
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
