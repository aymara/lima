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
 * @file       TextWriterBoWDocumentHandler.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Mar  7 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "TextWriterBoWDocumentHandler.h"
#include "bowTokenIterator.h"
#include "bowToken.h"
#include "bowText.h"

#include "common/Data/genericDocumentProperties.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

class TextWriterBoWDocumentHandlerPrivate
{
  friend class TextWriterBoWDocumentHandler;
  
  TextWriterBoWDocumentHandlerPrivate(std::ostream& os);
  ~TextWriterBoWDocumentHandlerPrivate();

  std::ostream& m_outputStream;

};

TextWriterBoWDocumentHandlerPrivate::TextWriterBoWDocumentHandlerPrivate(std::ostream& os):
m_outputStream(os)
{
}

TextWriterBoWDocumentHandlerPrivate::~TextWriterBoWDocumentHandlerPrivate() {
}

//***********************************************************************
// constructors and destructors
TextWriterBoWDocumentHandler::TextWriterBoWDocumentHandler(std::ostream& os):
m_d(new TextWriterBoWDocumentHandlerPrivate(os))
{
}

TextWriterBoWDocumentHandler::~TextWriterBoWDocumentHandler()
{
  delete m_d;
}

//***********************************************************************
void TextWriterBoWDocumentHandler::
openSBoWNode(const Misc::GenericDocumentProperties* /*properties*/,
             const std::string& /*elementName*/)
{
  //os << *properties;
}

void TextWriterBoWDocumentHandler::
openSBoWIndexingNode(const Misc::GenericDocumentProperties* properties,
                     const std::string& /*elementName*/)
{
  m_d->m_outputStream << *properties << std::endl;
}

void TextWriterBoWDocumentHandler::
processSBoWText(const BoWText* boWText, 
                bool useIterators)
{
  if (useIterators) {
    BoWTokenIterator it(*boWText);
    while (! it.isAtEnd()) {
      m_d->m_outputStream << it.getElement();
      it++;
    }
  }
  else {
    m_d->m_outputStream << *boWText;
  }
}

void TextWriterBoWDocumentHandler::
processProperties(const Misc::GenericDocumentProperties* /*properties*/, 
                  bool /*useIterators*/)
{
  //os << *properties;
}

void TextWriterBoWDocumentHandler::
closeSBoWNode()
{
}


} // end namespace
} // end namespace
} // end namespace
