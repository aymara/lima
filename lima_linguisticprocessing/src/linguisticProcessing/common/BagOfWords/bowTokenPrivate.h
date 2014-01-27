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
 * @file     BoWToken.cpp
 * @author   Besancon Romaric
 * @date     Tue Oct  7 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#ifndef BOWTOKENPRIVATE_H
#define BOWTOKENPRIVATE_H

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"

#include <iterator>  // for debug output
#include <algorithm> // function (on refMap)
#include <string>
#include <wchar.h>

namespace Lima
{
namespace Common
{
namespace BagOfWords
{
class BoWToken;


class BoWTokenPrivate
{
public:
  friend class BoWToken;
  friend std::ostream& operator << (std::ostream&, const BoWToken&);
  /** only for debug */
  friend std::ostream& operator << (std::ostream& os,
                                    const std::map<BoWToken*,uint64_t>& refMap);
  friend QDebug& operator << (QDebug&, const BoWToken&);
  /** only for debug */
  friend QDebug& operator << (QDebug& os,
                                    const std::map<BoWToken*,uint64_t>& refMap);
  
  BoWTokenPrivate(const Lima::LimaString& lemma,
           const uint64_t category,
           const uint64_t position,
           const uint64_t length);
  BoWTokenPrivate(const Lima::LimaString& str,
           const uint64_t position=0,
           const uint64_t length=0);
  BoWTokenPrivate(const BoWToken& tok);
  BoWTokenPrivate();

  BoWTokenPrivate(const BoWTokenPrivate& tok);
  BoWTokenPrivate& operator=(const BoWTokenPrivate& tok);
  bool operator==(const BoWTokenPrivate& tok);
  
  
  virtual ~BoWTokenPrivate();

  Lima::LimaString m_lemma;
  Lima::LimaString m_inflectedForm;
  LinguisticCode m_category;
  uint64_t m_position;
  uint64_t m_length;
  uint64_t m_vertex;

  static LimaChar m_separator;
  static LimaChar m_compoundSeparator;

  static bool m_useOnlyLemma;

  /**
   * convert the spaces in the lemma string into a non-space character
   * (defined by the m_compoundSeparator member)
   */
  void convertSpaces(void);

};



} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif
