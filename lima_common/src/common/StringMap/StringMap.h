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
#ifndef COMMON_STRINGMAP_STRINGMAP_HPP
#define COMMON_STRINGMAP_STRINGMAP_HPP


#include "common/LimaCommon.h"

#include "common/FsaAccess/FsaExceptions.h"

namespace Lima {
namespace Common {
namespace StringMap {

/*
 *  typename accessMethod: class with following method:
 *   accessMethod(bool keyOrderingIsForward)
 *   int getIndex(const basic_string<Lima::LimaChar>& key) const;
 *   uint64_t getSize() const;
 *   void read(keyFileName);
 *
 * typename contentElement: class or predefined type with following features
 *   copy constructor
 *
// * typename storedSet: class with following method:
// *   contentElement operator [] (int);
 *
 */

// Example : frequency dictionary
//typedef FsaAccessSpare16 accessMethod;
//typedef int contentElement;
//typedef std::vector<int> storedSet;

//template <typename accessMethod, typename contentElement, typename storedSet>
template <typename accessMethod, typename contentElement>
class StringMap {
public:
  StringMap(const contentElement& defaultValue );
  virtual ~StringMap();
  void parseAccessMethod( const std::string &keyFileName );
  uint64_t getSize() const;
  const contentElement& getDefaultValue() const { return m_emptyElement; }
/*
  std::pair<ComposedDict16_subword_iterator, ComposedDict16_subword_iterator>
    getSubWordEntries(
    const Lima::uint64_t offset,
    const Lima::LimaString& key ) const;
*/
protected:
  // automaton, tree or other acces method
  accessMethod m_accessMethod;
  // set of data associated to entries
//  storedSet m_data;
  // default return value for getElement() when there is no entry in dictionary
  contentElement m_emptyElement;
};

} // namespace StringMap
} // namespace Commmon
} // namespace Lima

#include "common/StringMap/StringMap.tcc"

#endif   //COMMON_STRINGMAP_STRINGMAP_HPP
