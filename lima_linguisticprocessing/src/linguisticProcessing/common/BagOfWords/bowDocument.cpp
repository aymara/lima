// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       bowDocument.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed Oct  8 2003
 * copyright   Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "bowDocument.h"
#include "common/Data/strwstrtools.h"
#include <fstream>
#include <deque>

using namespace std;

namespace Lima
{
namespace Common
{
namespace BagOfWords
{

//***********************************************************************
// stream operators
//***********************************************************************

std::ostream& operator<<(std::ostream& out,const BoWBlocType& blocType)
{
  unsigned char blocTypeCode=static_cast<unsigned char>(blocType);
  out.write((char*) &(blocTypeCode), sizeof(unsigned char));
  return out;
}

QDebug& operator<<(QDebug& out,const BoWBlocType& blocType)
{
  unsigned char blocTypeCode=static_cast<unsigned char>(blocType);
  out << blocTypeCode;
  return out;
}

std::istream& operator>>(std::istream& in,BoWBlocType& blocType)
{
  unsigned char typeCode;
  in.read((char*) &(typeCode), sizeof(unsigned char));
  blocType=static_cast<BoWBlocType>(typeCode);
  return in;
}

std::string getBlocTypeString(BoWBlocType blocType)
{
    switch (blocType) {
    case BoWBlocType::DOCUMENT_PROPERTIES_BLOC:
        return "DOCUMENT_PROPERTIES_BLOC";
    case BoWBlocType::BOW_TEXT_BLOC:
        return "BOW_TEXT_BLOC";
    case BoWBlocType::BIN_NUM_BLOC:
        return "BIN_NUM_BLOC";
    case BoWBlocType::ST_BLOC:
        return "ST_BLOC";
    case BoWBlocType::END_BLOC:
        return "END_BLOC";
    case BoWBlocType::NODE_PROPERTIES_BLOC:
        return "NODE_PROPERTIES_BLOC";
    case BoWBlocType::HIERARCHY_BLOC:
        return "HIERARCHY_BLOC";
    case BoWBlocType::INDEXING_BLOC:
        return "INDEXING_BLOC";
    }
    return "";
}

//***********************************************************************
// constructors
//***********************************************************************
BoWDocument::BoWDocument():
    GenericDocumentProperties(),
    BoWText()
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWDocument::BoWDocument()" << this;
#endif
}

BoWDocument::BoWDocument(const BoWDocument& d):
    GenericDocumentProperties(d),
    BoWText(d)
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWDocument::BoWDocument(BoWDocument)" << this;
#endif
}

//***********************************************************************
// destructor
//***********************************************************************
BoWDocument::~BoWDocument()
{
  clear();
}

//***********************************************************************
// assignment operator
//***********************************************************************
BoWDocument& BoWDocument::operator = (const BoWDocument& d)
{
  if (&d != this)
  {
    BoWText::operator=(d);
    GenericDocumentProperties::operator=(d);
  }
  return *this;
}

void BoWDocument::clear()
{
  BoWText::clear();
  Lima::Common::Misc::GenericDocumentProperties::reinit();
}

//***********************************************************************
// binary read/write
//***********************************************************************
void BoWDocument::setProperties(const GenericDocumentProperties& properties)
{
  GenericDocumentProperties::operator=(properties);
}

//***********************************************************************
// binary read/write
//***********************************************************************
void BoWDocument::reinit()
{
  GenericDocumentProperties::reinit();
  BoWText::clear();
}

//***********************************************************************
// binary read/write
//***********************************************************************
/*
void BoWDocument::read(std::istream& file)
{
  // enum type with small number of values coded on one byte;
  BoWBlocType blocType = readOneByteInt(file);
  // old format
  if (blocType==DOCUMENT_PROPERTIES_BLOC)
  {
    Misc::GenericDocumentProperties::read(file);
    file >> blocType;
  }
  while (blocType==BOW_TEXT_BLOC)
  {
    BoWText::read(file);
    if (file.eof()) break;
    file >> blocType;
  }
}
*/

/*
void BoWDocument::readPart(std::istream& file, AbstractBoWXMLWriter& writer, bool useIterator, std::ostream& os)
{
  BoWBlocType blocType = readOneByteInt(file);

  BOWLOGINIT;
  LDEBUG << "BoWDocument::readPart: read "
             << (int)blocType;

  // new format
  switch( blocType )
  {
    case HIERARCHY_BLOC:
    {
      std::string elementName;
      Misc::readStringField(file,elementName);
      writer.openSBoWNode(os, this, elementName);
      break;
    }
    case INDEXING_BLOC:
    {
      std::string elementName;
      Misc::readStringField(file,elementName);
      writer.openSBoWIndexingNode(os, this, elementName);
      break;
    }
    case BOW_TEXT_BLOC:
    {
      BoWText::clear();
      BoWText::read(file);
      writer.processSBoWText(os, this, useIterator);
      break;
    }
    case NODE_PROPERTIES_BLOC:
    {
      Misc::GenericDocumentProperties::read(file);
      writer.processProperties(os, this, useIterator);
      break;
    }
    case END_BLOC:
    {
      writer.closeSBoWNode(os);
      break;
    }
  }
}
*/

/*
void BoWDocument::write(std::ostream& file) const
{
  // enum type with small number of values coded on one byte;
  file << DOCUMENT_PROPERTIES_BLOC;
  Misc::GenericDocumentProperties::write(file);
  writeOneByteInt(file, BOW_TEXT_BLOC)
  BoWText::write(file);
  writeOneByteInt(file, END_BLOC)
}
*/

//***********************************************************************
// output
//***********************************************************************
std::ostream& operator << (std::ostream& os, const BoWDocument& d)
{
  os << static_cast<Misc::GenericDocumentProperties>(d) << std::endl;
  os << static_cast<BoWText>(d);
  return os;
}

QDebug& operator << (QDebug& os, const BoWDocument& d)
{
  os << static_cast<Misc::GenericDocumentProperties>(d);
  os << static_cast<BoWText>(d);
  return os;
}

} // end namespace
} // end namespace Common
} // end namespace
