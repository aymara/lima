// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

/**
 * @brief      An iterator on the bowText that returns IndexElements
 * @author     Besancon Romaric <romaric.besancon@cea.fr>
 * @date       Tue Feb  7 2006
 */
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
