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
 * @file       TextWriterBoWDocumentHandler.h
 * @author     Mesnard Olivier (olivier.mesnard@.cea.fr)
 * @date       Wed Oct  12 2005
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     BagOfWords
 * 
 * @brief      handler for structured bow document that produces a text output
 * 
 * 
 ***********************************************************************/

#ifndef TEXTWRITERBOWDOCUMENTHANDLER_H
#define TEXTWRITERBOWDOCUMENTHANDLER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "AbstractBoWDocumentHandler.h"
#include <vector>

namespace Lima {
namespace Common {
  namespace Misc {
    class GenericDocumentProperties;
  }
namespace BagOfWords {

class BoWText;

class TextWriterBoWDocumentHandlerPrivate;
class LIMA_BOW_EXPORT TextWriterBoWDocumentHandler : public AbstractBoWDocumentHandler
{
 public:
  TextWriterBoWDocumentHandler(std::ostream& os);
  ~TextWriterBoWDocumentHandler();
  
  void openSBoWNode(const Misc::GenericDocumentProperties* properties,
                    const std::string& elementName);
  void openSBoWIndexingNode(const Misc::GenericDocumentProperties* properties,
                            const std::string& elementName);
  void processSBoWText(const BoWText* boWText, 
                       bool useIterators, bool useIndexIterator);
  void processProperties(const Misc::GenericDocumentProperties* properties, 
                         bool useIterators, bool useIndexIterator);
  void closeSBoWNode();

 private:
   TextWriterBoWDocumentHandler(const TextWriterBoWDocumentHandler&);
   TextWriterBoWDocumentHandler& operator=(const TextWriterBoWDocumentHandler&);
   TextWriterBoWDocumentHandlerPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
