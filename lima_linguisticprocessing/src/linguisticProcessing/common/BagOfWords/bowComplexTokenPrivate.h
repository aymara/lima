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
 * @file     bowComplexToken.cpp
 * @author   Besancon Romaric
 * @date     Tue Oct  7 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#ifndef BOWCOMPLEXTOKENPRIVATE_H
#define BOWCOMPLEXTOKENPRIVATE_H


#include "bowTokenPrivate.h"
#include "bowComplexToken.h"
// #include "BoWRelation.h"
// #include "common/Data/readwritetools.h"
// #include "common/Data/strwstrtools.h"
// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <stdexcept>
// #include <boost/graph/graph_concepts.hpp>

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWComplexTokenPrivate : public BoWTokenPrivate
{
  friend class BoWComplexToken;
public:
  BoWComplexTokenPrivate();
  /**
   * copy constructor
   *
   * @attention the copy of the parts of a complex token is tricky :
   * if a part of a complex token points to a token that have a proper
   * existence outside of the part list, the pointer will be copied
   * (no creation of a new object), otherwise a new object is created.
   */
  BoWComplexTokenPrivate(const BoWComplexToken&);

  /**
   * specialized copy constructor that takes into account a
   * pointer-to-pointer map giving the correspondance of tokens for
   * the component list, in case of a complete BoWText copy.
   *
   */
  BoWComplexTokenPrivate(const BoWComplexToken&,
                  const std::map< QSharedPointer< BoWToken >, QSharedPointer< BoWToken > >&);

  BoWComplexTokenPrivate(const Lima::LimaString& lemma,
                  const uint64_t category,
                  const uint64_t position,
                  const uint64_t length);

  /**
   * a constructor to build a simple, independent complex token
   * (parts are simple tokens, not listed elsewhere in a BoWText)
   *
   * @param lemma the lemma of the complex token
   * @param category the category of the complex token
   * @param position the position of the complex token
   * @param length the length of the complex token
   * @param parts a deque containing the simple tokens forming
   * the parts of the complex token
   * @param int head the index of the head token in the preceding list
   *
   * @return
   */
  BoWComplexTokenPrivate(const Lima::LimaString& lemma,
                  const uint64_t category,
                  const uint64_t position,
                  const uint64_t length,
                  std::deque< QSharedPointer< BoWToken > >& parts,
                  const uint64_t head);

  virtual ~BoWComplexTokenPrivate();

  BoWComplexTokenPrivate(const BoWComplexTokenPrivate&);
  BoWComplexTokenPrivate& operator=(const BoWComplexTokenPrivate&);
  
  std::deque<BoWComplexToken::Part> m_parts;  /**< the list of the parts of the
                                complex token*/
  uint64_t m_head;       /**< the index of the head of the
                                complex token in its part list*/

  QSharedPointer< BoWToken > addPart(QSharedPointer< BoWRelation > rel,
                    QSharedPointer< BoWToken > tok,
                    const bool isInList,
                    const bool isHead=false);

  //helper functions for constructors, destructor and assignment operator
  void copy(const BoWComplexToken& a);
  void copy(const BoWComplexToken& t,
            const std::map< QSharedPointer< BoWToken >, QSharedPointer< BoWToken > >& refMap);

};

} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif
