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

#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICO_ANALYSISDICO_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICO_ANALYSISDICO_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

// accessMethod
#include "AbstractDictionaryEntry.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/AnalysisDict/AnalysisDictionaryIterators.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace AnalysisDict
{

/**
* @brief interface for analysis dictionary
*/
class LIMA_ANALYSISDICT_EXPORT AbstractAnalysisDictionary : public AbstractResource
{
  Q_OBJECT
public:

  AbstractAnalysisDictionary(QObject* parent = nullptr);

  virtual ~AbstractAnalysisDictionary();

/**
  * @brief get dictionary entry for a word
  * @param word word to search
  * @return AbstractDictionaryEntry* corresponding to word
  * @retval 0 if no available entry
  * @retval pointer to AbstractDictionaryEntry*. WARNING : must be deleted
  *         when no more used.
  */
  virtual DictionaryEntry getEntry(
    const Lima::LimaString& word) const = 0;

  /**
  * @brief get dictionary entry for a word
  * @param wordId string pool Id of word to search
  * @return AbstractDictionaryEntry* corresponding to word
  * @retval 0 if no available entry
  * @retval pointer to AbstractDictionaryEntry*. WARNING : must be deleted
  *         when no more used.
  */
  virtual DictionaryEntry getEntry(
    const StringsPoolIndex wordId) const = 0;
  
  virtual DictionaryEntry getEntry(
    const StringsPoolIndex wordId,
    const Lima::LimaString& word) const = 0;
  
  /**
  * @brief return subword entries
  * A subword entry is a word in the key string that begin at the given offset.
  * @param offset offset from which to start searching entry
  * @param key string in which searching for word.
  * @return subword iterator pair
  */
  virtual std::pair<DictionarySubWordIterator, DictionarySubWordIterator>
  getSubWordEntries(
    const int offset,
    const LimaString& key ) const = 0;

  /**
  * @brief return superword entries
  * A superword entry is an entry that begins with the given key
  */
  virtual std::pair<DictionarySuperWordIterator, DictionarySuperWordIterator>
  getSuperWordEntries(
    const LimaString& key ) const = 0;

  /**
  * @brief return number of keys in dictionary
  * @return number of keys.
  */
  virtual uint64_t getSize() const = 0;
    
};

} // namespace AnalysisDict
} // namespace LinguisticProcessing
} // namespace Lima

#endif   //COMMON_ANALYSISDICO_ANALYSISDICO_HPP
