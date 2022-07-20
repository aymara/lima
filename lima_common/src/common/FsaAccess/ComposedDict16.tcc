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

// For ::stat() function
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

namespace Lima {
namespace Common {
namespace FsaAccess {

template <typename contentElement, typename contentSet>
ComposedDict16<contentElement, contentSet>::ComposedDict16( const contentElement& defaultValue)
  : m_fsaDict(true), m_emptyElement(defaultValue) {
//ComposedDict16::ComposedDict16(contentSet& datas) : m_datas(datas), m_fsaDict(true) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "ComposedDict16::ComposedDict16()";
#endif
}

template <typename contentElement, typename contentSet>
ComposedDict16<contentElement, contentSet>::~ComposedDict16() {
}


template <typename contentElement, typename contentSet>
void ComposedDict16<contentElement, contentSet>::parseKeys( const std::string &keyFileName ) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "ComposedDict16::parse...";
#endif

#ifdef DEBUG_CD
  LDEBUG <<  "ComposedDict16::parse: readKeyFile...";
#endif
  // read main & default keys
  uint64_t size;
  m_fsaDict.read(keyFileName);
  size = m_fsaDict.getSize();
#ifdef DEBUG_CD
  LDEBUG <<  "ComposedDict16::parse " << size << " keys in main keyfile";
#endif

#ifdef DEBUG_CD
    LDEBUG <<  "ComposedDict16::parse end ";
#endif
}

template <typename contentElement, typename contentSet>
uint64_t ComposedDict16<contentElement, contentSet>::getSize() const {
  return( m_fsaDict.getSize() );
}

// Gets the dictionary entry correponding to the specified word.
// If word is not into dictionary,  is returned.
template <typename contentElement, typename contentSet>
const contentElement& ComposedDict16<contentElement, contentSet>::getElement(
const Lima::LimaString& word) const{
//const contentElement& ComposedDict16::getElement(const Lima::LimaString& word) const {
  uint64_t index = -1;
#ifdef DEBUG_CD
  FSAALOGINIT;
  const Lima::LimaString & basicWord = word;
  LDEBUG <<  "ComposedDict16::getElement("
            << Lima::Misc::convertString(basicWord) << ")";
#endif

  // Look in FsaDictionary
  index = m_fsaDict.getIndex(word);
#ifdef DEBUG_CD
  LDEBUG <<  "index = " << index;
#endif
  if( index > 0 )
    return m_datas[index];
  else
    return m_emptyElement;
}


} // namespace FsaAccess
} // namespace Commmon
} // namespace Lima
