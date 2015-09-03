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
 * @file       BinaryWriterBoWDocumentHandler.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Mar  7 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "BinaryWriterBoWDocumentHandler.h"
#include "bowDocument.h"
#include "bowTokenIterator.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

//***********************************************************************
// constructors and destructors
BinaryWriterBoWDocumentHandler::BinaryWriterBoWDocumentHandler(std::ostream& os):
m_writer(),
m_outputStream(os)
{
}

BinaryWriterBoWDocumentHandler::~BinaryWriterBoWDocumentHandler() {
}

//***********************************************************************
void BinaryWriterBoWDocumentHandler::
openSBoWNode(const Misc::GenericDocumentProperties* /*properties*/,
             const std::string& elementName)
{
  Common::Misc::writeOneByteInt(m_outputStream,Common::BagOfWords::HIERARCHY_BLOC);
  Misc::writeStringField(m_outputStream,elementName);
}

void BinaryWriterBoWDocumentHandler::
openSBoWIndexingNode(const Misc::GenericDocumentProperties* /*properties*/,
                     const std::string& elementName)
{
  Common::Misc::writeOneByteInt(m_outputStream,Common::BagOfWords::INDEXING_BLOC);
  Misc::writeStringField(m_outputStream,elementName);
}

void BinaryWriterBoWDocumentHandler::
processSBoWText(const BoWText* boWText, 
                bool /*useIterators*/)
{
  Common::Misc::writeOneByteInt(m_outputStream,Common::BagOfWords::BOW_TEXT_BLOC);
  m_writer.writeBoWText(m_outputStream,*boWText);
}

void BinaryWriterBoWDocumentHandler::
processProperties(const Misc::GenericDocumentProperties* properties, 
                  bool /*useIterators*/)
{
  Common::Misc::writeOneByteInt(m_outputStream,Common::BagOfWords::DOCUMENT_PROPERTIES_BLOC);
  properties->write(m_outputStream);
}

void BinaryWriterBoWDocumentHandler::
closeSBoWNode()
{
  Common::Misc::writeOneByteInt(m_outputStream,Common::BagOfWords::END_BLOC);
}


} // end namespace
} // end namespace
} // end namespace
