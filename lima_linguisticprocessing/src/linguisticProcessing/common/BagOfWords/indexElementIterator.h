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
 * @file       indexElementIterator.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Feb  7 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     BagOfWords
 * 
 * @brief      an iterator on the bowText that returns IndexElements
 * 
 * 
 ***********************************************************************/

#ifndef INDEXELEMENTITERATOR_H
#define INDEXELEMENTITERATOR_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

namespace Lima {
namespace Common {
  class AbstractLexiconIdGenerator;
namespace BagOfWords {

class BoWText;
class IndexElement;

class IndexElementIteratorPrivate;

class LIMA_BOW_EXPORT IndexElementIterator
{
 public:
  /** 
   * constructor
   * 
   * @param bowText the BoWText on which the iterator iterates
   * @param idAccessor object which convert a string or an compound word in term id
   * @param maxCompoundSize maximum size authorized for a compound: if
   * size is exceeded the compound will not be accessed by the
   * iterator (however, its partial compounds will be built and
   * accessed if their size is less than maxCompoundSize). If set to
   * zero, no limit to compound size.
   * @param nbMaxPartialCompounds a parameter allowing to limit
   * the number of partial complex tokens (avoid combinatorial
   * explosion for too large compounds)
   * 
   */
  IndexElementIterator(const BoWText& bowText,
                       AbstractLexiconIdGenerator* idAccessor=0,
                       const uint64_t maxCompoundSize=0,
                       const uint64_t nbMaxPartialCompounds=1000);
  ~IndexElementIterator();
      
  bool isAtEnd() const;
  const IndexElement& getElement() const;
  IndexElement getElement();
  IndexElementIterator& operator++();   //prefix ++
  IndexElementIterator operator++(int); //postfix++

private:
  IndexElementIterator(const IndexElementIterator&);
  IndexElementIterator& operator=(const IndexElementIterator&);
  IndexElementIteratorPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
