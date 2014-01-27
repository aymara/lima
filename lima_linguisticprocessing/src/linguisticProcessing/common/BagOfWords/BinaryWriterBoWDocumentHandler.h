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
 * @file       BinaryWriterBoWDocumentHandler.h
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
#include "bowToken.h"
#include "bowText.h"
#include "bowBinaryReaderWriter.h"
#include "AbstractBoWDocumentHandler.h"
#include "common/Data/genericDocumentProperties.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

class LIMA_BOW_EXPORT BinaryWriterBoWDocumentHandler : public AbstractBoWDocumentHandler
{
 public:
  BinaryWriterBoWDocumentHandler(std::ostream& os);
  ~BinaryWriterBoWDocumentHandler();
  
  void openSBoWNode(const Misc::GenericDocumentProperties* properties,
                    const std::string& elementName);
  void openSBoWIndexingNode(const Misc::GenericDocumentProperties* properties,
                            const std::string& elementName);
  void processSBoWText(const BoWText* boWText, 
                       bool useIterators);
  void processProperties(const Misc::GenericDocumentProperties* properties, 
                         bool useIterators);
  void closeSBoWNode();

 private:
  BoWBinaryWriter m_writer;
  std::ostream& m_outputStream;
};

} // end namespace
} // end namespace
} // end namespace

#endif
