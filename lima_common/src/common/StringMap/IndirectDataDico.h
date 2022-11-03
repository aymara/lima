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
#ifndef COMMON_STRINGMAP_INDIRECTEDDATADICO_HPP
#define COMMON_STRINGMAP_INDIRECTEDDATADICO_HPP


#include "common/LimaCommon.h"

#include <common/StringMap/StringMap.h>

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
 * typename storedSet: class with following method:
 *   contentElement operator [] (int);
 *
 */

// Example : analysis dictionary
//typedef FsaAccessSpare16 accessMethod;
//typedef Lima::LinguisticProcessing::Dictionary::DictionarEntry contentElement;
//typedef std::vector<uint8_t *> storedSet;

template <typename accessMethod, typename contentElement>
//class AnalysisDico : public StringMap<accessMethod, contentElement, storedSet> {
class IndirectDataDico : public StringMap<accessMethod, contentElement> {
public:
  IndirectDataDico(const contentElement& defaultValue );
  virtual ~IndirectDataDico();
  void parseData( const std::string& dataFileName );
  virtual contentElement getEntry(const Lima::LimaString& word) const = 0;
/*
  std::pair<ComposedDict16_subword_iterator, ComposedDict16_subword_iterator>
    getSubWordEntries(
    const Lima::uint64_t offset,
    const Lima::LimaString& key ) const;
*/
protected:
  // set of data associated to entries
  uint64_t *m_index2Data;
  uint8_t* m_data;
  virtual void fillIndex2Data();
};

} // namespace StringMap
} // namespace Commmon
} // namespace Lima

#include "common/StringMap/IndirectDataDico.tcc"

#endif   //COMMON_STRINGMAP_INDIRECTEDDATADICO_HPP
