// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file     bowText.cpp
 * @author   Besancon Romaric
 * @date     Tue Oct  7 17:13:26 2003
 * copyright Copyright (C) 2003 by CEA LIST
 * 
 ***********************************************************************/

#include "bowText.h"
#include "bowToken.h"
#include "bowTerm.h"
#include "bowNamedEntity.h"
#include "common/Data/strwstrtools.h"

#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"

using namespace std;

namespace Lima {
namespace Common {
namespace BagOfWords {


//**********************************************************************
// constructors,destructor,copy assignment
//**********************************************************************
BoWText::BoWText():
std::vector< boost::shared_ptr< AbstractBoWElement > >()
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWText::BoWText()" << this;
#endif
}


BoWText::BoWText(const BoWText& t):
std::vector< boost::shared_ptr< AbstractBoWElement > >()
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWText::BoWText(BoWText)" << this;
#endif
  (*this) = t;
}

BoWText& BoWText::operator = (const BoWText& t)
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWText::operator=" << this;
#endif
  if (this != &t)
  {
    clear();
    // have to store a pointer map to handle references in
    // the component lists of complex tokens
//     std::map<AbstractBoWElement*,AbstractBoWElement*> pointerMap;
    for (BoWText::const_iterator i(t.begin());
        i != t.end(); i++) {
      boost::shared_ptr< AbstractBoWElement > tok( (*i)->clone(/*pointerMap*/) );
      push_back(tok);
//       pointerMap[(*i)]=tok;
    }
  }
  return *this;
}

BoWText::~BoWText()
{
  clear();
}
void BoWText::writeBoWText(ostream& stream)
{
  BoWBinaryWriter writer;
  writer.writeBoWText(stream, *this);
}


void BoWText::clear() {
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWText::clear()" << this;
#endif
  for (auto it = begin(); it != end(); it++)
  {
#ifdef DEBUG_LP
    LDEBUG << "BoWText::clear clearing" << &(**it);
#endif
    (*it)->clear();
  }
  std::vector< boost::shared_ptr<  AbstractBoWElement > >::clear();
}

//**********************************************************************
// input/output functions
//**********************************************************************
std::ostream& operator << (std::ostream& os, const BoWText& text) {
  for (BoWText::const_iterator i(text.begin());
       i != text.end(); i++) {
    os << (*i)->getOutputUTF8String() << std::endl;
       }
       return os;
}
QDebug& operator << (QDebug& os, const BoWText& text) {
  for (BoWText::const_iterator i(text.begin());
       i != text.end(); i++) {
    os << (*i)->getString();
       }
       return os;
}

} // namespace BagOfWords
} // namespace Common
} // namespace Lima
