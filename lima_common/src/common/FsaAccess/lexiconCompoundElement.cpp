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
/*****************************************************************************
 *   Copyright (C) 2006 by  CEA                                              *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                           *
 *                                                                           *
 *  lexiconCompoundElement.cpp  -  description                               *
 *  common definition to implement lexiconIdGenerator and lexiconIdAccessor  *
 *****************************************************************************/

#include "common/FsaAccess/lexiconCompoundElement.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

std::ostream& operator << (std::ostream& os, 
                           const DepCompound& cmp) {
  cmp.print(os);
  return os;
}

std::ostream& operator << (std::ostream& os, 
                           const ExtCompound& ext) {
  ext.print(os);
  return os;
}

DepCompound::DepCompound(const DepCompound& dc) :
  std::pair<uint64_t, uint64_t>(dc)
{
}

DepCompound& DepCompound::operator=(const DepCompound& dc)
{
  first=dc.first;
  second=dc.second;
  return *this;
}

void DepCompound::print ( std::ostream& os) const {
  os << "(" << getHead() << "," << getExt() << ")";
}

ExtCompound::ExtCompound(const ExtCompound& dc) :
  std::pair<uint64_t, uint64_t>(dc)
{
}

ExtCompound& ExtCompound::operator=(const ExtCompound& dc)
{
  first=dc.first;
  second=dc.second;
  return *this;
}

void ExtCompound::print ( std::ostream& os) const {
  os << "(" << getFirst() << "," << getNext() << ")";
}

} // namespace FsaAccess
} // namespace Common
} // namespace Lima
