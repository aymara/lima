/*
    Copyright 2002-2022 CEA LIST

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
 * @file     bowComplexToken.cpp
 * @author   Besancon Romaric
 * @date     Tue Oct  7 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "bowComplexToken.h"
#include "bowTokenPrivate.h"
#include "bowComplexTokenPrivate.h"

#include "BoWRelation.h"
#include "common/Data/readwritetools.h"
#include "common/Data/strwstrtools.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Lima {
namespace Common {
namespace BagOfWords {

BoWComplexTokenPrivate::BoWComplexTokenPrivate():
  BoWTokenPrivate(),
  m_parts(0),
  m_head(0)
{
}

BoWComplexTokenPrivate::BoWComplexTokenPrivate(const BoWComplexTokenPrivate& bctp):
  BoWTokenPrivate(bctp),
  m_parts(),
  m_head(bctp.m_head)
{
  for (std::deque<BoWComplexToken::Part>::const_iterator i=bctp.m_parts.begin(); i!=bctp.m_parts.end(); i++) {
    // with the second parameter set to false, ensure that the part token will be cloned
    addPart((*i).getBoWRelation(),(*i).getBoWToken());
  }
  if (m_head>m_parts.size()) {
    m_head=0;
  }
}

BoWComplexTokenPrivate::BoWComplexTokenPrivate(const LimaString& lemma,
                                const LinguisticCode category,
                                const uint64_t position,
                                const uint64_t length):
  BoWTokenPrivate(lemma, category, position, length),
  m_parts(0),
  m_head(0)
{
}

BoWComplexTokenPrivate::BoWComplexTokenPrivate(const LimaString& lemma,
                                 const LinguisticCode category,
                                 const uint64_t position,
                                 const uint64_t length,
                                 std::deque< boost::shared_ptr< BoWToken > >& parts,
                                 const uint64_t head):
BoWTokenPrivate(lemma, category, position, length),
m_parts(0),
m_head(head)
{
  for (auto i = parts.begin(); i != parts.end(); i++) {
    addPart(boost::shared_ptr< BoWRelation >(),*i);
  }
  if (m_head>m_parts.size()) {
    m_head=0;
  }
}

BoWComplexTokenPrivate::BoWComplexTokenPrivate(const BoWComplexToken& t):
BoWTokenPrivate(t),
m_parts(0),
m_head(0)
{
  copy(t);
}


BoWComplexTokenPrivate::~BoWComplexTokenPrivate()
{
}


//////////////////

BoWComplexToken::BoWComplexToken():
  BoWToken(*new BoWComplexTokenPrivate)
{
}

BoWComplexToken::BoWComplexToken(const LimaString& lemma,
                                 const LinguisticCode category,
                                 const uint64_t position,
                                 const uint64_t length):
    BoWToken(*new BoWComplexTokenPrivate())
{
  m_d->m_lemma = lemma;
  m_d->m_category = category;
  m_d->m_position = position;
  m_d->m_length = length;
}

BoWComplexToken::BoWComplexToken(const LimaString& lemma,
                                 const LinguisticCode category,
                                 const uint64_t position,
                                 const uint64_t length,
                                 std::deque< boost::shared_ptr< BoWToken > >& parts,
                                 const uint64_t head):
                                 BoWToken(*new BoWComplexTokenPrivate())
{
  m_d->m_lemma = lemma;
  m_d->m_category = category;
  m_d->m_position = position;
  m_d->m_length = length;
  static_cast<BoWComplexTokenPrivate *>(m_d)->m_head = head;
  for (auto i = parts.begin(); i != parts.end(); i++)
  {
    addPart(boost::shared_ptr< BoWRelation >(), *i);
  }
  if (static_cast<BoWComplexTokenPrivate *>(m_d)->m_head > static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.size())
  {
    static_cast<BoWComplexTokenPrivate *>(m_d)->m_head=0;
  }
}


BoWComplexToken::BoWComplexToken(BoWComplexTokenPrivate& d) : BoWToken(d)
{
}

BoWComplexToken::~BoWComplexToken()
{
  clear();
//   delete m_d;
}

BoWComplexToken& BoWComplexToken::operator= (const BoWComplexToken& t) {
  if (this != &t) {
    clear();
    BoWToken::operator=(t);
    static_cast<BoWComplexTokenPrivate *>(m_d)->copy(t);
  }
  return *this;
}

bool BoWComplexToken::operator== (const BoWComplexToken& t) {
  return BoWToken::operator==(t);
}

//**********************************************************************
// helper functions for constructors, destructor and assignment operator
//**********************************************************************
// Careful : I create new objects for tokens that exist
// elsewhere in the list, and put the isInList member to false
// (safer solution than keeping the pointer)
void BoWComplexTokenPrivate::copy(const BoWComplexToken& t) {
  m_parts.clear();
  m_head = static_cast<BoWComplexTokenPrivate *>(t.m_d)->m_head;
  m_category = t.m_d->m_category;
  m_compoundSeparator = t.m_d->m_compoundSeparator;
  m_inflectedForm = t.m_d->m_inflectedForm;
  m_lemma = t.m_d->m_lemma;
  m_length = t.m_d->m_length;
  m_position = t.m_d->m_position;
  m_separator = t.m_d->m_separator;
  m_useOnlyLemma = t.m_d->m_useOnlyLemma;
  m_vertex = t.m_d->m_vertex;
  for (std::deque<BoWComplexToken::Part>::const_iterator i(t.getParts().begin());
       i != t.getParts().end(); i++) {
    boost::shared_ptr< BoWRelation > rel = (*i).get<0>();
    m_parts.push_back(BoWComplexToken::Part(rel,(*i).getBoWToken()));
  }
}

//**************************************************************
// functions
//**************************************************************
std::deque<BoWComplexToken::Part>& BoWComplexToken::getParts(void)
{
  return static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts;
}

const std::deque<BoWComplexToken::Part>&
BoWComplexToken::getParts(void) const
{
  return static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts;
}

std::deque<BoWComplexToken::Part>::iterator
BoWComplexToken::getPartsIterator(void)
{
  return static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.begin();
}

const std::deque<BoWComplexToken::Part>::const_iterator
BoWComplexToken::getPartsIterator(void) const
{
  return static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.begin();
}

uint64_t BoWComplexToken::getHead() const
{
  return static_cast<BoWComplexTokenPrivate *>(m_d)->m_head;
}

void BoWComplexToken::setHead(const uint64_t i) {
  static_cast<BoWComplexTokenPrivate *>(m_d)->m_head=i;
}

boost::shared_ptr< BoWToken > BoWComplexTokenPrivate::addPart(boost::shared_ptr< BoWRelation > rel,
                                          boost::shared_ptr< BoWToken > tok,
                                          bool isHead)
{
  m_parts.push_back(BoWComplexToken::Part(rel, tok));
  if (isHead) {
    m_head=m_parts.size()-1;
  }
  uint64_t previousPosition = m_position;

  // added the first part
  if (m_position == 0 && m_length==0)
  {
    m_position = tok->getPosition();
    m_length = tok->getLength();
  }
  // adding a part before the previous first part
  else if (tok->getPosition() < m_position)
  {
    m_position = tok->getPosition();
    m_length = previousPosition - tok->getPosition() + m_length;
  }
  // adding a part after the current end
  else if (tok->getPosition() > (previousPosition + m_length))
  {
    m_length = tok->getPosition() - previousPosition + tok->getLength();
  }
  return tok;
}


boost::shared_ptr< BoWToken > BoWComplexToken::addPart(boost::shared_ptr< BoWToken > tok,
                                          bool isHead)
{
  return addPart(boost::shared_ptr< BoWRelation >(), tok, isHead);
}


boost::shared_ptr< BoWToken > BoWComplexToken::addPart(boost::shared_ptr< BoWRelation > rel,
                                          boost::shared_ptr< BoWToken > tok,
                                          bool isHead)
{
    return static_cast<BoWComplexTokenPrivate *>(m_d)->addPart(rel,tok,isHead);
}

// BoWToken* BoWComplexToken::addPart(const BoWToken* tok,
//                                    bool isHead)
// {
//   return addPart(0, const_cast<BoWToken*>(tok), false, isHead);
// }
//
//
// BoWToken* BoWComplexToken::addPart(const BoWRelation* rel,
//                                    const BoWToken* tok,
//                                    bool isHead)
// {
//   return static_cast<BoWComplexTokenPrivate *>(m_d)->addPart(const_cast<BoWRelation*>(rel),const_cast<BoWToken*>(tok),false,isHead);
// }



void BoWComplexToken::clear()
{
//   for (std::deque<Part>::iterator i(static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.begin());
//        i != static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.end(); i++)
//   {
//     if (! (*i).isInList())
//     {
//       if ((*i).getBoWToken() != 0)
//       {
//         delete ((*i).getBoWRelation());
//         delete ((*i).getBoWToken());
//         (*i).get<0>()=0;
//         (*i).get<1>()=0;
//       }
//     }
//   }
  static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.clear();
}

LimaString BoWComplexToken::getLemma(void) const
{
  if (static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.size() ==0) {
    return m_d->m_lemma;
  }
  LimaString lemma;
  lemma = static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[static_cast<BoWComplexTokenPrivate *>(m_d)->m_head].get<1>()->getLemma();

  for (uint64_t i=0; i < static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.size(); i++)
  {
    if (i != static_cast<BoWComplexTokenPrivate *>(m_d)->m_head)
    {
      lemma += BoWToken::getCompoundSeparator() + static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].get<1>()->getLemma();
    }
  }
  return lemma;
}

LimaString BoWComplexToken::getInflectedForm(void) const
{
  LimaString infl;
  if (static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.size() ==0) {
    return infl;
  }
  infl = static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[static_cast<BoWComplexTokenPrivate *>(m_d)->m_head].get<1>()->getInflectedForm();

  for (uint64_t i=0; i < static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.size(); i++)
  {
    if (i != static_cast<BoWComplexTokenPrivate *>(m_d)->m_head)
    {
      infl += BoWToken::getCompoundSeparator() + static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].get<1>()->getInflectedForm();
    }
  }
  return infl;
}


  //**********************************************************************
uint64_t BoWComplexToken::size(void) const {
  if (static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.size() ==0) {
    return 1;
  }
  uint64_t nbParts=0;
  for (std::deque<BoWComplexToken::Part>::const_iterator
    part=static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.begin(); part!=static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.end(); part++) {
    nbParts+=(*part).getBoWToken()->size();
  }
  return nbParts;
}

//**********************************************************************
// get the list of (position,length) of the parts
//**********************************************************************
Common::Misc::PositionLengthList BoWComplexToken::getPositionLengthList() const {
  Common::Misc::PositionLengthList poslenlist(0);
  for (std::deque<Part>::const_iterator i(static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.begin());
       i != static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.end(); i++) {
    Common::Misc::PositionLengthList partposlen=(*i).getBoWToken()->getPositionLengthList();
    poslenlist.insert(poslenlist.end(),partposlen.begin(),partposlen.end());
  }
  return poslenlist;
}


//**********************************************************************
// add the offset to the position of the token and recursively to each
// of its parts
//**********************************************************************
void BoWComplexToken::addToPosition(const uint64_t offset) {
  m_d->m_position+=offset;
  for (std::deque<Part>::const_iterator part(static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.begin());
       part != static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.end(); part++) {
    (*part).getBoWToken()->addToPosition(offset);
  }
}

//**********************************************************************
// output (for debug)
// useful function for output of derived classes
//**********************************************************************
std::string BoWComplexToken::getUTF8StringParts(const Common::PropertyCode::PropertyManager* macroManager) const

{
  std::ostringstream oss;
  oss << "[";
  for (uint64_t i(0); i<static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.size(); i++) {
    if (static_cast<BoWComplexTokenPrivate *>(m_d)->m_head == i) {
      oss << "*";
    }
    //oss << m_parts[i].getBoWToken() << ":" << *(m_parts[i].getBoWToken());
    if (static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].getBoWRelation()!=0) {
      oss << static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].getBoWRelation()->getOutputUTF8String();
    }
    oss << static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].getBoWToken()->getOutputUTF8String(macroManager);
  }
  oss << "]";
  return oss.str();
}

//**********************************************************************
// output (for debug)
// useful function for output of derived classes
//**********************************************************************
std::string BoWComplexToken::getstdstringParts() const
{
  std::ostringstream oss;
  oss << "[";
  for (uint64_t i(0); i<static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.size(); i++) {
    if (static_cast<BoWComplexTokenPrivate *>(m_d)->m_head == i) {
      oss << "*";
    }
    //oss << m_parts[i].getBoWToken() << ":" << *(m_parts[i].getBoWToken());
    if (static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].getBoWRelation()!=0) {
      oss << *(static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].getBoWRelation());
    }
    oss << *(static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].getBoWToken());
  }
  oss << "]";
  return oss.str();
}

std::set< uint64_t > BoWComplexToken::getVertices() const
{
  std::set< uint64_t > result;
  for (uint64_t i(0); i<static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts.size(); i++)
  {
    if (boost::dynamic_pointer_cast< BoWComplexToken >(static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].get<1>()) != 0)
    {
      std::set< uint64_t > partResult = boost::dynamic_pointer_cast< BoWComplexToken >(static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].get<1>())->getVertices();
      result.insert(partResult.begin(), partResult.end());
    }
    else
    {
      result.insert(static_cast<BoWComplexTokenPrivate *>(m_d)->m_parts[i].get<1>()->getVertex());
    }
  }
  return result;
}

} // namespace BagOfWords
} // namespace Common
} // namespace Lima
