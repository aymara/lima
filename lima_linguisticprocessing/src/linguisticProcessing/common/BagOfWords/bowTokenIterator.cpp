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
 * @file       bowTokenIterator.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Thu Feb  9 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "bowTokenIterator.h"
#include "AbstractBoWElement.h"
#include "bowTerm.h"
#include "bowText.h"
#include "bowComplexToken.h"

#include "common/Data/strwstrtools.h"
#include <algorithm>


using namespace std;

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWTokenIteratorPrivate
{
  friend class BoWTokenIterator;

  BoWTokenIteratorPrivate(const BoWText& bowText,
                   const uint64_t nbMaxPartialCompounds=1000,
                   BoWTokenIteratorNamedEntitiesPartsPolicy iterateThroughNamedEntitiesParts = ITERATE_THROUGH_NAMEDENTITIES_PARTS);
  BoWTokenIteratorPrivate(const BoWTokenIteratorPrivate& btip);
  ~BoWTokenIteratorPrivate();

  // a class to represent complex tokens parts that are stored in
  // queue: some are pointers to tokens in BoWText, some are
  // created => keep information for clean delete
  class BoWTokenPart : public std::pair<const AbstractBoWElement*,bool> {
  public:
    BoWTokenPart():std::pair<const AbstractBoWElement*,bool>((AbstractBoWElement*)0,false) {}
    BoWTokenPart(const AbstractBoWElement* token,bool isCreated):
      std::pair<const AbstractBoWElement*,bool>(token,isCreated) {}

    ~BoWTokenPart() {}

    const AbstractBoWElement* const& getBoWToken() const { return first; }
    const AbstractBoWElement*& getBoWToken() { return first; }
    bool& isCreated() { return second; }
    bool isCreated() const { return second; }
  };

  typedef std::deque<BoWTokenPart> BoWTokenQueue;


  // members
  BoWText::const_iterator m_iterator;
  BoWText::const_iterator m_iteratorEnd;
  BoWTokenQueue m_partQueue;
  uint64_t m_maxSizeQueue;

  // created tokens are stored in a separate vector:
  // make memory management easier (copy and destructor
  // of BoWTokenPart and BoWTokenIterator do not
  // have to care about pointers)
  // this object is cleared at end of part
  std::vector<const BoWToken*> m_createdTokens;

  /// set to DO_NOT_ITERATE_THROUGH_NAMEDENTITIES_PARTS to don't iterate over named entities parts
  /// Default is ITERATE_THROUGH_NAMEDENTITIES_PARTS
  BoWTokenIteratorNamedEntitiesPartsPolicy m_iterateThroughNamedEntitiesParts;


  // private methods

  // add in queue
  // (return false if size of queue becomes greater than max)
  bool addInPartQueue(const BoWToken* token,
                      const bool isCreated);

  void storePartsInQueue(const BoWToken* token);

  // a type to store interesting parts of complex tokens
  // (the parts used to be combined to create partial complex tokens)
  typedef std::vector<const BoWToken*> PartTokens;

  // recursive function to get all parts of complex token
  // and create partial complex tokens
  // (return false if size of queue becomes greater than max)
  bool addPartElementsInQueue(const BoWToken* token,
                              std::vector<PartTokens>& partTokens);

  /**
   * combine parts to create partial complex tokens
   *
   * @param subPartTokens a vector of tokens parts:
   * the function will create all combination of complex tokens
   * taking one token after another in each PartTokens of the
   * vector
   * @param createdTokens a vector containing all created tokens
   * (pointers that should not be deleted by caller)
   *
   * @return true of all combined parts successfully added in queue,
   * false otherwise (limited size of queue)
   */
  bool addCombinedPartsInQueue(const std::vector<PartTokens>& subPartTokens,
                               const uint64_t head,
                               std::vector<BoWToken*>& createdTokens,
                               std::vector<const BoWToken*>& currentPartialToken,
                               const uint64_t i,
                               bool& first);
  /**
   * create a partial complex token with a given combination
   * of parts
   *
   * @param iterators the iterators that define a combination
   * @return the created complex token
   */
  BoWComplexToken* createComplexToken(const std::vector<const BoWToken*>& parts);
};

BoWTokenIteratorPrivate::BoWTokenIteratorPrivate(const BoWText& bowText,
                 const uint64_t nbMaxPartialCompounds,
                 BoWTokenIteratorNamedEntitiesPartsPolicy iterateThroughNamedEntitiesParts):
m_iterator(bowText.begin()),
m_iteratorEnd(bowText.end()),
m_partQueue(),
m_maxSizeQueue(nbMaxPartialCompounds),
m_createdTokens(0),
m_iterateThroughNamedEntitiesParts(iterateThroughNamedEntitiesParts)
{
}

BoWTokenIteratorPrivate::BoWTokenIteratorPrivate(const BoWTokenIteratorPrivate& btip)
{
  m_iterator = btip.m_iterator;
  m_iteratorEnd = btip.m_iteratorEnd;
  m_partQueue = btip.m_partQueue;
  m_maxSizeQueue = btip.m_maxSizeQueue;
  m_createdTokens = btip.m_createdTokens;
  m_iterateThroughNamedEntitiesParts = btip.m_iterateThroughNamedEntitiesParts;
}

BoWTokenIteratorPrivate::~BoWTokenIteratorPrivate()
{
}


//**********************************************************************
// iterator functions
//**********************************************************************
BoWTokenIterator::BoWTokenIterator(const BoWText& bowText,
                 const uint64_t nbMaxPartialCompounds,
                 BoWTokenIteratorNamedEntitiesPartsPolicy iterateThroughNamedEntitiesParts):
    m_d(new BoWTokenIteratorPrivate(bowText, nbMaxPartialCompounds, iterateThroughNamedEntitiesParts))
{
}

BoWTokenIterator::BoWTokenIterator(const BoWTokenIterator& bti):
    m_d(new BoWTokenIteratorPrivate(*bti.m_d))
{
}

BoWTokenIterator::~BoWTokenIterator() 
{
  delete m_d;
}

void BoWTokenIterator::deletePartialComplexTokens() {
  std::vector<const BoWToken*>::iterator 
    it=m_d->m_createdTokens.begin(),
    it_end=m_d->m_createdTokens.end();
  for (; it!=it_end; it++) {
    delete *it;
    *it=0;
  }
  m_d->m_createdTokens.clear();
}

bool BoWTokenIterator::isAtEnd() const {
  return (m_d->m_iterator == m_d->m_iteratorEnd);
}

//**********************************************************************
// get current element ("dereference" iterator)
//**********************************************************************
// getting parts is done in this function (rather than in ++ function):
// which means that is a ++ is done before calling a getElement on 
// a complex token, no parts will be explored
const AbstractBoWElement* BoWTokenIterator::getElement() {
  if (m_d->m_partQueue.empty()) {
    if (m_d->m_iterator==m_d->m_iteratorEnd) { // at end
      return 0;
    }
    else {
      switch ((*m_d->m_iterator)->getType()) {
      case BOW_PREDICATE:
      case BOW_TOKEN: {
        return *m_d->m_iterator;
        break;
      }
      case BOW_TERM:
      case BOW_NAMEDENTITY: {
        // element itself will be stored in queue as part
        m_d->storePartsInQueue(static_cast<const BoWToken*>(*m_d->m_iterator));
        return m_d->m_partQueue.front().getBoWToken();
        break;
      }
      default: ;
      }
    }
  }
  else {
    return m_d->m_partQueue.front().getBoWToken();
  }
  return 0;
}

//**********************************************************************
// operator ++
//**********************************************************************
BoWTokenIterator& BoWTokenIterator::operator++() 
{
  if (m_d->m_partQueue.empty()) {
    if (m_d->m_iterator!=m_d->m_iteratorEnd) {
      m_d->m_iterator++;
    }
  }
  else {
    m_d->m_partQueue.pop_front();
    if (m_d->m_partQueue.empty()) { // finished for the parts of this token
        m_d->m_iterator++;
        // delete partial complex tokens created for this token
        deletePartialComplexTokens(); 
    }
  }
  return *this;
}

// postfix ++ operator
BoWTokenIterator BoWTokenIterator::operator++(int) {
  BoWTokenIterator it = *this;
  ++(*this);
  return it;
}

//**********************************************************************
// helper functions for iterator
//**********************************************************************
bool BoWTokenIteratorPrivate::addInPartQueue(const BoWToken* token,
               const bool isCreated) 
{
  if (m_partQueue.size() >= m_maxSizeQueue) {
    BOWLOGINIT;
    LWARN << "size of queue exceeded"; 
    return false;
  }
  // avoid call to copy constructor (ensure pointer copy)
  m_partQueue.push_back(BoWTokenPart());
  m_partQueue.back().getBoWToken()=token;
  m_partQueue.back().isCreated()=isCreated;
  if (isCreated) {
    m_createdTokens.push_back(token);
  }
  BOWLOGINIT;
  LDEBUG << "add in part queue " << *token 
         << "; isCreated=" << isCreated 
         << "size of queue=" << m_partQueue.size() 
        ;
  return true;
}

void BoWTokenIteratorPrivate::storePartsInQueue(const BoWToken* token) {
  vector<vector<const BoWToken*> > partTokens;
  if (!addPartElementsInQueue(token,partTokens)) {
    BOWLOGINIT;
    LWARN << "Token contain too many subparts (some are ignored): " 
      << token->getLemma();
  }
}

bool BoWTokenIteratorPrivate::addPartElementsInQueue(const BoWToken* token,
                       vector<PartTokens>& partTokens) {

  BOWLOGINIT;
  LDEBUG << "getPartElements on " << *token;

  // push token itself
  if (! addInPartQueue(token,false)) {
    return false;
  }
//   addInPartQueue(token,false);
  if (token->getType()==BOW_NAMEDENTITY
    && m_iterateThroughNamedEntitiesParts==DO_NOT_ITERATE_THROUGH_NAMEDENTITIES_PARTS)
  {
    partTokens.push_back(vector<const BoWToken*>());
    partTokens.back().push_back(token);
    return true;
  }
  
  switch (token->getType()) {
  case BOW_TOKEN: {
    // push simple token in parts 
    partTokens.push_back(vector<const BoWToken*>());
    partTokens.back().push_back(token);
    break;
  }
  case BOW_TERM:
  case BOW_NAMEDENTITY: {
    const BoWComplexToken* complexToken=static_cast<const BoWComplexToken*>(token);

    if (complexToken->size() == 1) { 
      // only one part, do not get into it
      // (for instance, named entity with one element)
      // push simple token in parts 
      partTokens.push_back(vector<const BoWToken*>());
      partTokens.back().push_back(token);
      return true;
    }

    // add new vector for token parts
    partTokens.push_back(vector<const BoWToken*>());

    // push token itself and head
    LDEBUG << "storing token " << *token;
    partTokens.back().push_back(token);
    const BoWComplexToken::Part& headPart=complexToken->getParts()[complexToken->getHead()];
    if (!headPart.isInList()) {
      LDEBUG << "storing head token " << *(headPart.getBoWToken());
      partTokens.back().push_back(headPart.getBoWToken());
    }
    
    // treat parts
    vector<vector<const BoWToken*> > subPartTokens;

    deque<BoWComplexToken::Part>::const_iterator 
      part=complexToken->getParts().begin(),
      part_end=complexToken->getParts().end();
    
    for (; part!=part_end; part++) {
      if (!addPartElementsInQueue((*part).getBoWToken(),subPartTokens)) {
        return false;
      }
    }

    if (subPartTokens.size()>1) {
      vector<BoWToken*> combined(0);
      vector<const BoWToken*> currentPartialToken(0);
      bool first(true);
      if (!addCombinedPartsInQueue(subPartTokens,complexToken->getHead(),
                                   combined,currentPartialToken,0,first)) {
        return false;
      }
      // add created partial tokens in parts
      partTokens.back().insert(partTokens.back().end(),combined.begin(),combined.end());
    }
    break;
  }
  default: ;
  }
  return true;
}


bool BoWTokenIteratorPrivate::addCombinedPartsInQueue(const std::vector<PartTokens>& subPartTokens,
                        const uint64_t head,
                        std::vector<BoWToken*>& createdTokens,
                        std::vector<const BoWToken*>& currentPartialToken,
                        const uint64_t i,
                        bool& first)
{
  if (i>=subPartTokens.size()) {
    if (first) {
      // do not create token for first combination: corresponds
      // the token itself (the one in BoWText, that has already
      // added in queue)
      first=false;
      return true;
    }

    if (currentPartialToken.size() == 1) {
      //just the head: is already in queue
      return true;
    }

    // at end of parts => add current currentPartialToken
    BoWComplexToken* partialComplexToken=
      createComplexToken(currentPartialToken);

    if (partialComplexToken!=0) {
      if (! addInPartQueue(partialComplexToken,true)) {
        return false;
      }
      // addInPartQueue(partialComplexToken,true);
      createdTokens.push_back(partialComplexToken);
    }
    return true;
  }

  // add possible at end of currentPartialToken and recursive call
  for (std::vector<const BoWToken*>::const_iterator 
         it=subPartTokens[i].begin(),it_end=subPartTokens[i].end();
       it!=it_end; it++) {
    currentPartialToken.push_back(*it);
    if (!addCombinedPartsInQueue(subPartTokens,head,
                                 createdTokens,currentPartialToken,
                                 i+1,first)) {
      return false;
    }
    currentPartialToken.pop_back();
  }
  // if head, stop here: current iterator is head, hence always added
  // otherwise, recursive call without current iterator (that is an
  // extension)
  if (i!=head) {
    if (!addCombinedPartsInQueue(subPartTokens,head,
                                 createdTokens,currentPartialToken,
                                 i+1,first)) {
      return false;
    }
  }
  return true;
}

//**********************************************************************
// create a partial complex token 
BoWComplexToken* BoWTokenIteratorPrivate::createComplexToken(const vector<const BoWToken*>& parts) {

//   BOWLOGINIT;
//   LDEBUG << "create complex token";

  BoWTerm* partialComplexToken=new BoWTerm;

  // do not set lemma of partial token : do not store computed lemmas
  // for compounds

  for (vector<const BoWToken*>::const_iterator 
         it=parts.begin(),
         it_end=parts.end();
       it!=it_end; it++) {
    partialComplexToken->addPart(const_cast<BoWToken*>(*it),true);
  }
  // set position and length
  Common::Misc::PositionLengthList
    poslenlist=partialComplexToken->getPositionLengthList();
  // have to sort positions
  std::sort(poslenlist.begin(),poslenlist.end());
  if (! poslenlist.empty()) {
    partialComplexToken->setPosition(poslenlist.front().first);
    partialComplexToken->setLength(poslenlist.back().first+
                                   poslenlist.back().second-
                                   poslenlist.front().first);
  }
  
  return partialComplexToken;
}

} // end namespace
} // end namespace
} // end namespace
