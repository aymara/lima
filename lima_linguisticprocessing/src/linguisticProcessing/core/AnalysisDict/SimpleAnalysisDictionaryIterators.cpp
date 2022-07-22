// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "SimpleAnalysisDictionaryIterators.h"
#include "SimpleAnalysisDictionary.h"

#include "common/misc/strwstrtools.h"

using namespace std;
using namespace Lima::Common;

namespace Lima
{
namespace LinguisticProcessing
{
namespace AnalysisDict
{

SimpleAnalysisDictionarySubWordIterator::SimpleAnalysisDictionarySubWordIterator(
  const AccessSubWordIterator& it,
  const SimpleAnalysisDictionary& dico,
  const LimaString& key,
  const int offset) :
    ClonableDictionarySubWordIterator(),
    m_accessItr(it),
    m_dico(dico),
    m_key(key),
    m_offset(offset)
{}

SimpleAnalysisDictionarySubWordIterator::SimpleAnalysisDictionarySubWordIterator(const SimpleAnalysisDictionarySubWordIterator& source) :
ClonableDictionarySubWordIterator(),
    m_accessItr(source.m_accessItr),
    m_dico(source.m_dico),
    m_key(source.m_key),
    m_offset(source.m_offset)
{}

SimpleAnalysisDictionarySubWordIterator::~SimpleAnalysisDictionarySubWordIterator() {}

const std::pair<int,Dictionary::DictionaryEntry>
SimpleAnalysisDictionarySubWordIterator::operator*() const
{
#ifdef DEBUG_CD
  ANALYSISDICTLOGINIT;
  LDEBUG <<  "SimpleAnalysisDictionarySubWordIterator::operator*()";
#endif
  const std::pair<int,int64_t> pos =
    *m_accessItr;

#ifdef DEBUG_CD
  LDEBUG <<  "SimpleAnalysisDictionarySubWordIterator::operator*(): m_access=("
  << pos.first << "," << pos.second << ")"
 ;
#endif

  int offset = pos.first;

  // TODO: optimize it: access iterator knows it!
  // reformation du mot
  LimaString word(m_key, m_offset, offset - m_offset);
  string word8 = Common::Misc::limastring2utf8stdstring(word);

#ifdef DEBUG_CD
  int dataOffset = m_dico.m_index2Data[pos.second-1];
LDEBUG <<  "SimpleAnalysisDictionarySubWordIterator::operator*(): word=" << word8
  << ", index =" << pos.second
  << ", dataOffset =" << dataOffset;
#endif

  return std::pair<int, Dictionary::DictionaryEntry>
         (offset, Dictionary::DictionaryEntry(
            word, m_dico.m_stringStartAddr, m_dico.m_lingPropertiesStartAddr, m_dico.m_data+m_dico.m_index2Data[pos.second]));
}


SimpleAnalysisDictionarySuperWordIterator::SimpleAnalysisDictionarySuperWordIterator(
  const Common::AccessSuperWordIterator& it) :
  m_accessItr(it)
{}


SimpleAnalysisDictionarySuperWordIterator::SimpleAnalysisDictionarySuperWordIterator(const SimpleAnalysisDictionarySuperWordIterator& source) :
  ClonableDictionarySuperWordIterator(),
  m_accessItr(source.m_accessItr)
{}

SimpleAnalysisDictionarySuperWordIterator::~SimpleAnalysisDictionarySuperWordIterator()
{}

} // namespace AnalysisDict
} // namespace LinguisticProcessing
} // namespace Lima
