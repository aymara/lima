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
 *  Compact dictionnary based on finite state automata implemented with    *
 *  Boost Graph library.                                                   *
 *  Algorithm is described in article from Daciuk, Mihov, Watson & Watson: *
 *  "Incremental Construction of Minimal Acyclic Finite State Automata"    *
 ***************************************************************************/


namespace Lima {
namespace Common {
namespace StringMap {

//template <typename accessMethod, typename contentElement, typename storedSet>
//StringMap<accessMethod, contentElement, storedSet>::StringMap( const contentElement& defaultValue )
template <typename accessMethod, typename contentElement>
StringMap<accessMethod, contentElement>::StringMap( const contentElement& defaultValue )
  : m_accessMethod(true), m_emptyElement(defaultValue) {
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG <<  "StringMap::StringMap()" << LENDL;
#endif
}

//template <typename accessMethod, typename contentElement, typename storedSet>
//StringMap<accessMethod, contentElement, storedSet>::~StringMap() {
template <typename accessMethod, typename contentElement>
StringMap<accessMethod, contentElement>::~StringMap() {
}


//template <typename accessMethod, typename contentElement, typename storedSet>
//void StringMap<accessMethod, contentElement, storedSet>::parseAccessMethod( const std::string &keyFileName )
template <typename accessMethod, typename contentElement>
void StringMap<accessMethod, contentElement>::parseAccessMethod( const std::string &keyFileName ) {
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG <<  "StringMap::parseKeys(" << keyFileName << ")" << LENDL;
#endif

  // read main & default keys
  m_accessMethod.read(keyFileName);
#ifdef DEBUG_CD
  uint64_t size = m_accessMethod.getSize();
  LDEBUG <<  "StringMap::parseKeys: " << size << " keys read from main keyfile" << LENDL;
#endif
}


//template <typename accessMethod, typename contentElement, typename storedSet>
//uint64_t StringMap<accessMethod, contentElement, storedSet>::getSize() const {
template <typename accessMethod, typename contentElement>
uint64_t StringMap<accessMethod, contentElement>::getSize() const {
  return( m_accessMethod.getSize() );
}


} // namespace StringMap
} // namespace Commmon
} // namespace Lima
