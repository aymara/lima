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
#ifndef COMMON_ANALYSISDICO_ANALYSISDICO_HPP
#define COMMON_ANALYSISDICO_ANALYSISDICO_HPP


#include "common/LimaCommon.h"

// accessMethod
#include "common/StringMap/IndirectDataDico.h"

#include "linguisticProcessing/core/Dictionary/DictionaryEntry.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {

/*
 *  typename accessMethod: class with following method:
 *   accessMethod(bool keyOrderingIsForward)
 *   int getIndex(const basic_string<Lima::LimaChar>& key) const;
 *   uint64_t getSize() const;
 *   void read(keyFileName);
 *
 */

// Example : analysis dictionary
// typedef Lima::Common::FsaAccess::FsaAccessSpare16 accessMethod;
// AnalysisDico<accessMethod>

typedef Lima::LinguisticProcessing::Dictionary::DictionaryEntry analysisDicoEntry;

template <typename accessMethod>
class AnalysisDico : public Lima::Common::StringMap::IndirectDataDico<accessMethod, analysisDicoEntry> {
public:
  AnalysisDico(bool trie_direction_fwd=true);
  analysisDicoEntry getEntry(const Lima::LimaString& word) const;
/*
  std::pair<ComposedDict16_subword_iterator, ComposedDict16_subword_iterator>
    getSubWordEntries(
    const int offset,
    const LimaString& key ) const;
*/
  void setDicoCode(Lima::LinguisticProcessing::Dictionary::DictionaryCode* dicoCode) {
    m_dicoCode=dicoCode;
  }
protected:
  virtual void fillIndex2Data();
  Lima::LinguisticProcessing::Dictionary::DictionaryCode *m_dicoCode;
  bool m_trie_direction_fwd;
};

} // namespace AnalysisDict
} // namespace LinguisticProcessing
} // namespace Lima

#include "linguisticProcessing/core/AnalysisDict/AnalysisDico.tcc"

#endif   //COMMON_ANALYSISDICO_ANALYSISDICO_HPP
