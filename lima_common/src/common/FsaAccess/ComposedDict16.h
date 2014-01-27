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
/***************************************************************************
 *   Copyright (C) 2003 by  CEA                                            *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  composed dictionnary                                                   *
 ***************************************************************************/
#ifndef COMPACT_DICT_COMPOSED_DICT16_HPP
#define COMPACT_DICT_COMPOSED_DICT16_HPP


#include "common/LimaCommon.h"

#include "common/FsaAccess/FsaAccessSpare16.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

//typedef int contentElement;
//typedef std::vector<int> contentSet;

template <typename contentElement, typename contentSet>
class ComposedDict16 {
public:
  ComposedDict16(const contentElement& defaultValue );
  virtual ~ComposedDict16( );
  void parseKeys( const std::string &keyFileName );
  uint64_t getSize() const;
  const contentElement& getDefaultValue() const { return m_emptyElement; }
  const contentElement& getElement(const Lima::LimaString& word) const;
/*
  std::pair<ComposedDict16_subword_iterator, ComposedDict16_subword_iterator>
    getSubWordEntries(
    const Lima::uint64_t offset,
    const Lima::LimaString& key ) const;
*/
protected:
  // automaton
  FsaAccessSpare16 m_fsaDict;
  // set of data associated to entries
  contentSet m_datas;
  // default return value for getElement() when there is no entry in dictionary
  contentElement m_emptyElement;
};

} // namespace FsaAccess
} // namespace Commmon
} // namespace Lima

#include "common/FsaAccess/ComposedDict16.tcc"

#endif   //COMPACT_DICT_COMPOSED_DICT16_HPP
