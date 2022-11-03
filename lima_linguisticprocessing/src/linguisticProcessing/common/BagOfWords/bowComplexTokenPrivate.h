// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

  BoWComplexTokenPrivate(const Lima::LimaString& lemma,
                  const LinguisticCode category,
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
                  const LinguisticCode category,
                  const uint64_t position,
                  const uint64_t length,
                  std::deque< boost::shared_ptr< BoWToken > >& parts,
                  const uint64_t head);

  virtual ~BoWComplexTokenPrivate();

  BoWComplexTokenPrivate(const BoWComplexTokenPrivate&);
  BoWComplexTokenPrivate& operator=(const BoWComplexTokenPrivate&);

  std::deque<BoWComplexToken::Part> m_parts;  /**< the list of the parts of the
                                complex token*/
  uint64_t m_head;       /**< the index of the head of the
                                complex token in its part list*/

  boost::shared_ptr< BoWToken > addPart(boost::shared_ptr< BoWRelation > rel,
                    boost::shared_ptr< BoWToken > tok,
                    const bool isHead=false);

  //helper functions for constructors, destructor and assignment operator
  void copy(const BoWComplexToken& a);

};

} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif
