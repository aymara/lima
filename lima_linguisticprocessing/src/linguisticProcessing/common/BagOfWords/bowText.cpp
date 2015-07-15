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

using namespace std;

namespace Lima {
namespace Common {
namespace BagOfWords {


//**********************************************************************
// constructors,destructor,copy assignment
//**********************************************************************
BoWText::BoWText():
std::vector< QSharedPointer< AbstractBoWElement > >()
{
  BOWLOGINIT;
  LDEBUG << "BoWText::BoWText()" << this;
}


BoWText::BoWText(const BoWText& t):
std::vector< QSharedPointer< AbstractBoWElement > >()
{
  BOWLOGINIT;
  LDEBUG << "BoWText::BoWText(BoWText)" << this;
  (*this) = t;
}

BoWText& BoWText::operator = (const BoWText& t)
{
  BOWLOGINIT;
  LDEBUG << "BoWText::operator=" << this;
  if (this != &t)
  {
    clear();
    // have to store a pointer map to handle references in
    // the component lists of complex tokens
//     std::map<AbstractBoWElement*,AbstractBoWElement*> pointerMap;
    for (BoWText::const_iterator i(t.begin());
        i != t.end(); i++) {
      QSharedPointer< AbstractBoWElement > tok( (*i)->clone(/*pointerMap*/) );
      push_back(tok);
//       pointerMap[(*i)]=tok;
    }
  }
  return *this;
}

BoWText::~BoWText()
{
  BOWLOGINIT;
  LDEBUG << "BoWText::~BoWText()" << this;
  clear();
}

void BoWText::clear() {
  BOWLOGINIT;
  LDEBUG << "BoWText::clear()" << this;
  for (auto it = begin(); it != end(); it++)
  {
    LDEBUG << "BoWText::clear clearing" << (*it);
    (*it).clear();
  }
  std::vector< QSharedPointer<  AbstractBoWElement > >::clear();
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
