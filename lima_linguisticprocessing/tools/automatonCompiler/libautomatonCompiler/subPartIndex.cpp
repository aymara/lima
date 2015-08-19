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
 * @file       subPartIndex.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Jan 19 2005
 * @version    $Id: subPartIndex.cpp 8083 2007-04-13 17:07:59Z mesnardo $
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 *
 ***********************************************************************/

#include "compilerExceptions.h"
#include "subPartIndex.h"
#include "subAutomaton.h" // need subautomatons for named indexes
#include "common/Data/strwstrtools.h"
#include "ruleFormat.h"

using namespace Lima::Common;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

#define AUCLOGINIT LOGINIT("Automaton::Compiler")

//***********************************************************************
// constructors
//***********************************************************************
SubPartIndex::SubPartIndex():
m_partIndex(SUB_NONE,0),
m_subPartIndex(0)
{
}

SubPartIndex::SubPartIndex(const LimaString& str,
                           const std::vector<SubAutomaton>& subAutomatons):
m_partIndex(SUB_NONE,0),
m_subPartIndex(0)
{
  init(str,subAutomatons);
}

SubPartIndex::SubPartIndex(const SubPartIndex& i):
m_partIndex(i.m_partIndex),
m_subPartIndex(0) {
  if (i.hasSubPart()) {
    m_subPartIndex=new SubPartIndex(*(i.getSubPartIndex()));
  }
}

void SubPartIndex::init(const LimaString& str,
                        const std::vector<SubAutomaton>& subAutomatons) {
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "initialization SubPartIndex from " << str;
#endif

  m_partIndex=make_pair(SUB_NONE,0);
  m_subPartIndex=0;

  if (str.isEmpty()) {
    return;
  }

  LimaChar sep('.');
  //int i=str.find(sep);
  int i=str.indexOf(sep);
  LimaString index = str.left(i);
  LimaString subIndex;
  if (i!=-1) {
    subIndex=str.mid(i+1);
  }

  if (index==STRING_CONSTRAINT_GROUP_FIRST) {
    m_partIndex.first=SUB_FIRST;
  }
  else if (index==STRING_CONSTRAINT_GROUP_CURRENT) {
    m_partIndex.first=SUB_CURRENT;
  }
  else if (index==STRING_CONSTRAINT_GROUP_NEXT) {
    m_partIndex.first=SUB_NEXT;
  }
  else if (index==STRING_CONSTRAINT_GROUP_LAST) {
    m_partIndex.first=SUB_LAST;
  }
  else {
    m_partIndex.second=atoi(Misc::limastring2utf8stdstring(index).c_str());
    if (m_partIndex.second==0) {
      // try to find string as named index in one subAutomaton
      std::vector<SubAutomaton>::const_iterator
        sub=subAutomatons.begin(),
        sub_end=subAutomatons.end();
      LimaString value;
      for (; sub != sub_end; sub++) {
        if ((*sub).findAttribute(index,value)) {
          init(value+subIndex,subAutomatons);
          break;
        }
      }
      if (sub == sub_end) {
        AUCLOGINIT;
      LERROR << "Error: cannot parse subindex " << Common::Misc::limastring2utf8stdstring(index)
          << " in " << Common::Misc::limastring2utf8stdstring(str);
      }
    }
  }

  if (! subIndex.isEmpty()) {
    m_subPartIndex=new SubPartIndex(subIndex,subAutomatons);
  }

#ifdef DEBUG_LP
  LDEBUG << "=>" << *this;
#endif
}


//***********************************************************************
// destructor
//***********************************************************************
SubPartIndex::~SubPartIndex() {
  if (hasSubPart()) {
    delete m_subPartIndex;
    m_subPartIndex=0;
  }
}

//***********************************************************************
// assignment operator
//***********************************************************************
SubPartIndex& SubPartIndex::operator = (const SubPartIndex& i) {
  if (this != &i) {
    m_partIndex=i.m_partIndex;
    if (hasSubPart()) {
      delete m_subPartIndex;
      m_subPartIndex=0;
    }
    if (i.hasSubPart()) {
      m_subPartIndex=new SubPartIndex(*(i.getSubPartIndex()));
    }
  }
  return *this;
}

//***********************************************************************
// comparison function
//***********************************************************************
bool SubPartIndex::isBefore(const SubPartIndex& i) {

#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "comparing part indexes "
         << *this << " and " << i;
#endif

  bool isBefore(false);
  bool isEqual(false);
  bool error(false);

  switch (m_partIndex.first) {
  case SUB_NONE: {
    switch(i.getPartIndex().first) {
    case SUB_NONE:
      isEqual=(m_partIndex.second==i.getPartIndex().second);
      isBefore=(m_partIndex.second<i.getPartIndex().second);
      break;
    default: error=true; break;
    }
    break;
  }
  case SUB_FIRST: {
    switch(i.getPartIndex().first) {
    case SUB_NONE: error=true; break;
    case SUB_FIRST: isEqual=true; break;
    default: isBefore=true; break;
    }
    break;
  }
  case SUB_CURRENT: {
    switch(i.getPartIndex().first) {
    case SUB_NONE: error=true; break;
    case SUB_FIRST: isBefore=false; break;
    case SUB_CURRENT: isEqual=true; break;
    default: isBefore=true; break;
    }
    break;
  }
  case SUB_NEXT: {
    switch(i.getPartIndex().first) {
    case SUB_NONE: error=true; break;
    case SUB_FIRST:
    case SUB_CURRENT: isBefore=false; break;
    case SUB_NEXT: isEqual=true; break;
    default: isBefore=true; break;
    }
    break;
  }
  case SUB_LAST: {
    switch(i.getPartIndex().first) {
    case SUB_NONE: error=true; break;
    case SUB_LAST: isEqual=true; break;
    default: isBefore=false; break;
    }
    break;
  }
  }

  if (isEqual) {
    if (hasSubPart() && i.hasSubPart()) {
      isBefore=m_subPartIndex->isBefore(*(i.getSubPartIndex()));
    }
    else {
      error=true;
    }
  }

  if (error) {
    ostringstream oss;
    if (isEqual) {
      oss << "both arguments of constraint are identical (part indexes"
          << *this << " and " << i << " are equal)";
    }
    else {
      oss << "cannot compare part indexes "
          << *this << " and " << i;
    }
    throw AutomatonCompilerException(oss.str());
  }

  return isBefore;
}

//***********************************************************************
// output
//***********************************************************************
std::ostream& operator << (std::ostream& os, const SubPartIndex& i) {
  switch (i.getPartIndex().first) {
    case SUB_NONE:    os << i.getPartIndex().second; break;
    case SUB_FIRST:   os << "FIRST" ; break;
    case SUB_CURRENT: os << "CURRENT" ; break;
    case SUB_NEXT:    os << "NEXT" ; break;
    case SUB_LAST:    os << "LAST" ; break;
  }
  if (i.hasSubPart()) {
    os << "/" << *(i.getSubPartIndex());
  }
  return os;
}
QDebug& operator << (QDebug& os, const SubPartIndex& i) {
  switch (i.getPartIndex().first) {
    case SUB_NONE:    os << i.getPartIndex().second; break;
    case SUB_FIRST:   os << "FIRST" ; break;
    case SUB_CURRENT: os << "CURRENT" ; break;
    case SUB_NEXT:    os << "NEXT" ; break;
    case SUB_LAST:    os << "LAST" ; break;
  }
  if (i.hasSubPart()) {
    os << "/" << *(i.getSubPartIndex());
  }
  return os;
}

} // end namespace
} // end namespace
} // end namespace
