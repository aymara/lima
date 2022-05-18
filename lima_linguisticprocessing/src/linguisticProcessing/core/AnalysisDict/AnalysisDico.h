// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
