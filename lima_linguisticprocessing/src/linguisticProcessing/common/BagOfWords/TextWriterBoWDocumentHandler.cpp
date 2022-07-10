// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       TextWriterBoWDocumentHandler.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Mar  7 2006
 * copyright   Copyright (C) 2006-2020 by CEA LIST
 *
 ***********************************************************************/

#include "TextWriterBoWDocumentHandler.h"
#include "bowTokenIterator.h"
#include "bowToken.h"
#include "bowText.h"
#include "indexElementIterator.h"
#include "indexElement.h"

#include "common/Data/genericDocumentProperties.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

class TextWriterBoWDocumentHandlerPrivate
{
  friend class TextWriterBoWDocumentHandler;

  TextWriterBoWDocumentHandlerPrivate(std::ostream& os);
  ~TextWriterBoWDocumentHandlerPrivate();

  void writeIndexElement(const IndexElement& element);

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
                bool useIterators, bool useIndexIterator)
{
  if (useIterators) {
    BoWTokenIterator it(*boWText);
    while (! it.isAtEnd()) {
      m_d->m_outputStream << it.getElement();
      it++;
    }
  }
  else if (useIndexIterator) {
    IndexElementIterator it(*boWText);
    while (! it.isAtEnd())
    {
      m_d->writeIndexElement(it.getElement());
      it++;
    }
  }
  else {
    m_d->m_outputStream << *boWText;
  }
}

void TextWriterBoWDocumentHandler::
processProperties(const Misc::GenericDocumentProperties* /*properties*/,
                  bool /*useIterators*/, bool /*useIndexIterator*/)
{
  //os << *properties;
}

void TextWriterBoWDocumentHandler::
closeSBoWNode()
{
}

void TextWriterBoWDocumentHandlerPrivate::writeIndexElement(
                  const IndexElement& element) {
  LIMA_UNUSED(element)
//   m_outputStream << "<term "
//      << "id=\"" << element.getId() << "\"";
//   if (element.empty()) {
//     m_outputStream << "/>" << endl;
//     return;
//   }
//   if (element.isSimpleTerm()) {
//     std::string cat = static_cast<const MediaticData::LanguageData&>(MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertySymbolicValue(element.getCategory());
//
//     m_outputStream << " lemma=\"" << xmlString(Common::Misc::limastring2utf8stdstring(element.getSimpleTerm()))
//        << "\" category=\"" << cat
//        << "\" position=\"" << element.getPosition()
//        << "\" length=\"" << element.getLength() << "\"";
//     if (element.isNamedEntity()) {
//       m_outputStream << " neType=\"" << element.getNamedEntityType() << "\"";
//       m_outputStream << " type=\"" << BOW_NAMEDENTITY << "\"";
//     }
//     else {
//       m_outputStream << " type=\"" << BOW_TOKEN << "\"";
//     }
//     m_outputStream << "/>" << endl;
//     return;
//   }
//
//   // compound
//   if (element.isNamedEntity()) {
//     m_outputStream << " neType=\"" << element.getNamedEntityType() << "\"";
//     m_outputStream << " type=\"" << BOW_NAMEDENTITY << "\"";
//   }
//   else {
//     m_outputStream << " type=\"" << BOW_TERM << "\"";
//   }
//   m_outputStream << ">" << endl
//      << "  <structure>" << endl;
//
//   for (uint64_t i(0),size=element.getStructure().size(); i<size; i++) {
//     m_outputStream << "    <termRef id=\""
//        << element.getStructure()[i]
//        << "\" rel=\"" << element.getRelations()[i]
//        << "\"/>" << endl;
//
//   }
//   m_outputStream << "  </structure>" << endl
//      << "</term>" << endl;
}


} // end namespace
} // end namespace
} // end namespace
