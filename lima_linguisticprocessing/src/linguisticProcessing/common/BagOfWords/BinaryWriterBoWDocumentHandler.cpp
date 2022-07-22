// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
                bool /*useIterators*/, bool /*useIndexIterator*/)
{
  Common::Misc::writeOneByteInt(m_outputStream,Common::BagOfWords::BOW_TEXT_BLOC);
  m_writer.writeBoWText(m_outputStream,*boWText);
}

void BinaryWriterBoWDocumentHandler::
processProperties(const Misc::GenericDocumentProperties* properties,
                  bool /*useIterators*/, bool /*useIndexIterator*/)
{
  Common::Misc::writeOneByteInt(m_outputStream,Common::BagOfWords::DOCUMENT_PROPERTIES_BLOC);
  properties->write(m_outputStream);
}

void BinaryWriterBoWDocumentHandler::
closeSBoWNode()
{
  Common::Misc::writeOneByteInt(m_outputStream,Common::BagOfWords::END_BLOC);
  m_outputStream.flush();
}


} // end namespace
} // end namespace
} // end namespace
