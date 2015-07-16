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
 * @file       bowTokenIterator.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Thu Feb  9 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     BagOfWords
 * 
 * @brief      an iterator on BoWText that returns BoWTokens (including partial complex tokens)
 * 
 * 
 ***********************************************************************/

#ifndef BOWTOKENITERATOR_H
#define BOWTOKENITERATOR_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <deque>

#include <boost/shared_ptr.hpp>

namespace Lima {
namespace Common {
namespace BagOfWords {

enum BoWTokenIteratorNamedEntitiesPartsPolicy {
  ITERATE_THROUGH_NAMEDENTITIES_PARTS,
  DO_NOT_ITERATE_THROUGH_NAMEDENTITIES_PARTS
};

class BoWText;
class AbstractBoWElement;

/** 
 * an iterator (const) to go through the BoWText and get all BoWTokens
 * that are to be indexed : go into recursive complex tokens, 
 * and retrieve also partial complex tokens :
 * (*aeroport (*capitale bosniaque)) will also give aeroport_capitale
 * as a BoWToken
 *
 * Note about memory management of partial complex tokens:
 * pointers on partial complex tokens that are returned by the
 * getElement functions are pointers on temporary BoWTokens 
 * that are deleted when all parts of complex token have been 
 * explored
 * => if their usage is not immediate, pointers returned should
 * be cloned.
 *
 * Note also that this iterator should be mainly used in "standard"
 * search: while (!it.isAtEnd) { <...> it.getElement(); <...> it++ };
 * the behaviour for other exotic manipulations is not guaranteed.
 */
class BoWTokenIteratorPrivate;
class LIMA_BOW_EXPORT BoWTokenIterator {
 public:
  /** 
   * constructor
   * 
   * @param bowText the BoWText on which the iterator iterates
   * @param nbMaxPartialCompounds a parameter allowing to limit
   * the number of partial complex tokens (avoid combinatorial
   * explosion for too large compounds)
   * @param iterateThroughNamedEntitiesParts set to
   * DO_NOT_ITERATE_THROUGH_NAMEDENTITIES_PARTS to don't iterate over named
   * entities parts. Default is ITERATE_THROUGH_NAMEDENTITIES_PARTS
   * 
   */
  BoWTokenIterator(const BoWText& bowText,
                   const uint64_t nbMaxPartialCompounds=1000,
                   BoWTokenIteratorNamedEntitiesPartsPolicy iterateThroughNamedEntitiesParts = ITERATE_THROUGH_NAMEDENTITIES_PARTS);
  ~BoWTokenIterator();
      
  bool isAtEnd() const;
  boost::shared_ptr< Lima::Common::BagOfWords::AbstractBoWElement > getElement();
  BoWTokenIterator& operator++();   //prefix ++
  BoWTokenIterator operator++(int); //postfix++

  void deletePartialComplexTokens();

private:
  BoWTokenIterator(const BoWTokenIterator&);
  BoWTokenIterator& operator=(const BoWTokenIterator&);
  BoWTokenIteratorPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
