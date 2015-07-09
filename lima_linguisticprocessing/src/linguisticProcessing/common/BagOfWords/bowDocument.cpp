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

//***********************************************************************
// constructors
//***********************************************************************
BoWDocument::BoWDocument():
    BoWText(),
    GenericDocumentProperties()
{}

BoWDocument::BoWDocument(const BoWDocument& d):
    BoWText(d),
    GenericDocumentProperties(d)
{
}

//***********************************************************************
// destructor
//***********************************************************************
BoWDocument::~BoWDocument()
{
  
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
  BoWBlocType blocType;
  file >> blocType;
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
  BoWBlocType blocType;
  file >> blocType;
  
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
  file << BOW_TEXT_BLOC;
  BoWText::write(file);
  file << END_BLOC;
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
