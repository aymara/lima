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

//#include "linguisticProcessing/core/Dictionary/DictionaryEntry.h"
//#include "linguisticProcessing/core/Dictionary/DictionaryCode.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {

template <typename accessMethod>
AnalysisDico<accessMethod>::AnalysisDico( bool trie_direction_fwd )
    : Lima::Common::StringMap::IndirectDataDico<accessMethod, analysisDicoEntry>( analysisDicoEntry() ),
      m_trie_direction_fwd(trie_direction_fwd) {
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG <<  "AnalysisDico::AnalysisDico()";
#endif
}

template <typename accessMethod>
analysisDicoEntry AnalysisDico<accessMethod>::getEntry(const Lima::LimaString& word) const{
  int64_t index = -1;
  std::cerr <<  "AnalysisDico::getEntry().." << std::endl;
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  const LimaString & basicWord = word;
  LDEBUG <<  "AnalysisDico::getEntry("
            << Lima::Common::Misc::convertString(basicWord) << ")";
#endif

  // Look in FsaDictionary (or tree or..)
  index = m_accessMethod.getIndex(word);
#ifdef DEBUG_CD
  LDEBUG <<  "index = " << index;
#endif

  index = m_accessMethod.getIndex(word);
//  if( index > 0 ) ???
  if( index >= 0 )
  {
#ifdef DEBUG_CD
    LDEBUG <<  "index = " << index;
#endif
    analysisDicoEntry entry(word, m_data+m_index2Data[index], m_dicoCode );
    return entry;
//    return analysisDataElement(word, m_index2Data[index], m_dicoCode );
  }
  else
    return m_emptyElement;
}

template <typename accessMethod>
void AnalysisDico<accessMethod>::fillIndex2Data() {
  // fill index2Data
  uint64_t keyCount = getSize();
  m_index2Data = new uint64_t[keyCount];
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG <<  "IndirectDataDico::parseData: fill index_2data... ";
#endif
  size_t ptrOffset = 0;
  unsigned char *datasAddr = m_data;
  Lima::LinguisticProcessing::Dictionary::BinaryEntry binaryEntry;
  for( uint64_t entry=0 ; entry < keyCount ; entry++ )
  {
#ifdef DEBUG_CD
//    LDEBUG <<  "IndirectDataDico::parseData: offset m_index2Data[" << entry << "]=" << ptrOffset;
#endif
    ptrOffset = datasAddr - m_data;
    m_index2Data[entry] = ptrOffset;
    binaryEntry.nextField(datasAddr);
  }
#ifdef DEBUG_CD
  LDEBUG <<  "IndirectDataDico::parseData: end fill index_2data";
#endif
}

} // namespace AnalysisDict
} // namespace LinguisticProcessing
} // namespace Lima
