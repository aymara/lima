// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  LDEBUG <<  "SimpleDataDico::SimpleDataDico()";
#endif
}

template <typename accessMethod, typename contentElement, typename storedSet>
void SimpleDataDico<accessMethod, contentElement, storedSet>::parseData( const std::string& dataFileName )
{
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG << "SimpleDataDico::parseData(" << dataFileName;
#endif

  std::ifstream is(dataFileName.c_str(), std::ios::binary );
  if( is.bad() ) {
    LIMA_EXCEPTION_LOGINIT(
      STRINGMAPLOGINIT,
      "SimpleDataDico::parseData: Can't open file " << dataFileName.c_str());
  }
  copy(std::istream_iterator<contentElement>(is), std::istream_iterator<contentElement>(),
    back_inserter(m_data));
  uint64_t dataSize = m_data.size();
#ifdef DEBUG_CD
  LDEBUG << "SimpleDataDico::parseData: read " << dataSize
            << " pieces of data from " << dataFileName;
#endif
  auto accessSize = StringMap<accessMethod, contentElement>::m_accessMethod.getSize();
  if( accessSize != dataSize )
  {
    LIMA_EXCEPTION_LOGINIT(
      STRINGMAPLOGINIT,
      "SimpleDataDico::parseData dataSize = " << dataSize
        << " != accessSize = " << accessSize);
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
  LDEBUG <<  "SimpleDataDico::getElement(" << basicWord << ")";
#endif

  // Look in FsaDictionary (or tree or..)
  index = StringMap<accessMethod, contentElement>::m_accessMethod.getIndex(word);
#ifdef DEBUG_CD
  LDEBUG <<  "index = " << index;
#endif
  if( index > 0 )
    return m_data[index];
  else
    return StringMap<accessMethod, contentElement>::m_emptyElement;
}


} // namespace StringMap
} // namespace Commmon
} // namespace Lima
