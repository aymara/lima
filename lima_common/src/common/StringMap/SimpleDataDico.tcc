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

template <typename accessMethod, typename contentElement, typename storedSet>
SimpleDataDico<accessMethod, contentElement, storedSet>::SimpleDataDico( const contentElement& defaultValue )
//    : StringMap<accessMethod, contentElement, storedSet>( defaultValue ) {
    : StringMap<accessMethod, contentElement>( defaultValue ) {
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG <<  "SimpleDataDico::SimpleDataDico()" << LENDL;
#endif
}

template <typename accessMethod, typename contentElement, typename storedSet>
void SimpleDataDico<accessMethod, contentElement, storedSet>::parseData( const std::string& dataFileName )
{
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG << "SimpleDataDico::parseData(" << dataFileName << LENDL;
#endif

  std::ifstream is(dataFileName.c_str(), std::ios::binary );
  if( is.bad() ) {
    std::string mess = "SimpleDataDico::parseData: Can't open file " + dataFileName;
#ifdef DEBUG_CD
    LERROR << mess << LENDL;
#endif
    throw( Lima::IncompleteResources(mess) );
  }
  copy(std::istream_iterator<contentElement>(is), std::istream_iterator<contentElement>(),
    back_inserter(m_data));
  uint64_t dataSize = m_data.size();
#ifdef DEBUG_CD
  LDEBUG << "SimpleDataDico::parseData: read " << dataSize
            << " pieces of data from " << dataFileName << LENDL;
#endif
  if( StringMap<accessMethod, contentElement>::m_accessMethod.getSize() != dataSize ) {
    std::ostringstream oss;
    oss << "SimpleDataDico::parseData dataSize = " << dataSize
        << " != accessSize = " << StringMap<accessMethod, contentElement>::m_accessMethod.getSize();
#ifdef DEBUG_CD
    LERROR << oss.str() << LENDL;
#endif
    throw( Lima::IncompleteResources(oss.str()) );
  }
}


// Gets the dictionary entry correponding to the specified word.
// If word is not into dictionary, m_emptyElement is returned.
template <typename accessMethod, typename contentElement, typename storedSet>
const contentElement& SimpleDataDico<accessMethod, contentElement, storedSet>::getElement(
const Lima::LimaString& word) const{
  uint64_t index = -1;
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  const Lima::LimaString & basicWord = word;
  LDEBUG <<  "SimpleDataDico::getElement("
            << Lima::Common::Misc::convertString(basicWord) << ")" << LENDL;
#endif

  // Look in FsaDictionary (or tree or..)
  index = StringMap<accessMethod, contentElement>::m_accessMethod.getIndex(word);
#ifdef DEBUG_CD
  LDEBUG <<  "index = " << index << LENDL;
#endif
  if( index > 0 )
    return m_data[index];
  else
    return StringMap<accessMethod, contentElement>::m_emptyElement;
}


} // namespace StringMap
} // namespace Commmon
} // namespace Lima
