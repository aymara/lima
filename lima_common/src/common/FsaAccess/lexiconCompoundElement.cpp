// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
