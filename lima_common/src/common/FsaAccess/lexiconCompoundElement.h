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
 *  lexiconCompoundElement.h  -  description                                 *
 *  common definition to implement lexiconIdGenerator and lexiconIdAccessor  *
 *****************************************************************************/
#include <iostream>
#include "common/LimaCommon.h"

#ifndef FSA_LEXICON_COMPOUND_ELEMENT_H
#define FSA_LEXICON_COMPOUND_ELEMENT_H

namespace Lima {
namespace Common {
namespace FsaAccess {

class LIMA_FSAACCESS_EXPORT DepCompound : public std::pair<uint64_t, uint64_t> {
  friend LIMA_FSAACCESS_EXPORT std::ostream& operator << ( std::ostream& os, const DepCompound& cmp);
public:
  DepCompound()
    : std::pair<uint64_t, uint64_t> (0,0) {}
  DepCompound(uint64_t head , uint64_t ext)
    : std::pair<uint64_t, uint64_t> (head, ext) {}
  uint64_t getHead() const { return first; }
  uint64_t getExt() const { return second ;}
  void print (std::ostream& os) const;
};

struct LIMA_FSAACCESS_EXPORT DepCompoundLtOp
{
  bool operator()(const DepCompound& cpp1, const DepCompound& cpp2) const
  {
    if( cpp1.getHead() < cpp2.getHead() )
      return true;
    else if( cpp1.getHead() > cpp2.getHead() )
      return false;
    else
      return ( cpp1.getExt() < cpp2.getExt() );
  }
};


class LIMA_FSAACCESS_EXPORT ExtCompound : public std::pair<uint64_t, uint64_t> {
  friend LIMA_FSAACCESS_EXPORT std::ostream& operator << ( std::ostream& os, const ExtCompound& cmp);
public:
  ExtCompound()
   : std::pair<uint64_t, uint64_t> (0, 0) {}
  ExtCompound(uint64_t first , uint64_t next)
   : std::pair<uint64_t, uint64_t> (first, next) {}
  uint64_t getFirst() const { return first; }
  uint64_t getNext() const { return second; }
  void print ( std::ostream& os) const;
};

struct LIMA_FSAACCESS_EXPORT ExtCompoundLtOp
{
  bool operator()(const ExtCompound& cpp1, const ExtCompound& cpp2) const
  {
    if( cpp1.getFirst() < cpp2.getFirst() )
      return true;
    else if( cpp1.getFirst() > cpp2.getFirst() )
      return false;
    else
      return ( cpp1.getNext() < cpp2.getNext() );
  }
};

class LIMA_FSAACCESS_EXPORT DepCompoundLink : public std::pair<Lima::Common::FsaAccess::DepCompound, uint64_t> {
public:
  DepCompoundLink() : std::pair<Lima::Common::FsaAccess::DepCompound, uint64_t>() {}
  DepCompoundLink(Lima::Common::FsaAccess::DepCompound depCompound, uint64_t id)
    : std::pair<Lima::Common::FsaAccess::DepCompound, uint64_t>() {std::make_pair(depCompound,id);}
  Lima::Common::FsaAccess::DepCompound getDepCompound() const { return first; }
  uint64_t getId() const { return second ;}
};

struct LIMA_FSAACCESS_EXPORT DepCompoundLinkLtOp
{
  bool operator()(const DepCompoundLink& link1, const DepCompoundLink& link2) const
  {
    return m_op(link1.getDepCompound(), link2.getDepCompound() );
  }
  DepCompoundLtOp m_op;
};

class LIMA_FSAACCESS_EXPORT ExtCompoundLink : public std::pair<Lima::Common::FsaAccess::ExtCompound, uint64_t> {
public:
  ExtCompoundLink(Lima::Common::FsaAccess::ExtCompound extCompound, uint64_t id)
    : std::pair<Lima::Common::FsaAccess::ExtCompound, uint64_t> (extCompound, id) {}
  Lima::Common::FsaAccess::ExtCompound getExtCompound() const { return first; }
  uint64_t getId() const { return second; }
};

struct LIMA_FSAACCESS_EXPORT ExtCompoundLinkLtOp
{
  bool operator()(const ExtCompoundLink& link1, const ExtCompoundLink& link2) const
  {
    return m_op(link1.getExtCompound(), link2.getExtCompound() );
  }
  ExtCompoundLtOp m_op;
};


} // namespace FsaAccess
} // namespace Common
} // namespace Lima

#endif   //FSA_LEXICON_COMPOUND_ELEMENT_H
