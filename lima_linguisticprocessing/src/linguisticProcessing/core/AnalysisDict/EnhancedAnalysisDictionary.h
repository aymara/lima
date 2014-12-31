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
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTENHANCEDANALYSISDICTIONARY_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTENHANCEDANALYSISDICTIONARY_H

#include "AnalysisDictExport.h"
#include "AbstractAnalysisDictionary.h"
#include "DictionaryData.h"
#include "common/misc/AbstractAccessByString.h"
#include "common/misc/fsaStringsPool.h"

namespace Lima {

namespace LinguisticProcessing {

namespace AnalysisDict {

#define ENHANCEDANALYSISDICTIONARY_CLASSID "EnhancedAnalysisDictionary"

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDICT_EXPORT EnhancedAnalysisDictionary : public AbstractAnalysisDictionary
{
  Q_OBJECT
public:
    EnhancedAnalysisDictionary();
    
    EnhancedAnalysisDictionary(
      FsaStringsPool* sp,
      Lima::Common::AbstractAccessByString* access,
      const std::string& dataFile);

    virtual ~EnhancedAnalysisDictionary();

    virtual void init(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager);
    
    virtual DictionaryEntry getEntry(const Lima::LimaString& word) const;
    virtual DictionaryEntry getEntry(const StringsPoolIndex wordId) const;
    virtual DictionaryEntry getEntry(const StringsPoolIndex wordId, const Lima::LimaString& word) const;
    /**
     * Returns subwords entries. Warning : Since all strings are stored as keys in dictionary, the subword
     * entries function may returns substrings which have empty dictionary entry. Program which use
     * this function should take care of it.
     * @param offset 
     * @param key 
     * @return subword iterators
     */
    virtual std::pair< DictionarySubWordIterator, DictionarySubWordIterator > getSubWordEntries(const int offset, const LimaString& key) const;
    virtual std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator > getSuperWordEntries(const LimaString& key) const;
    virtual uint64_t getSize() const;
    
    void loadDataFile(const std::string& file);
    
    DictionaryEntry getEntryData(const StringsPoolIndex entryId) const;

private Q_SLOTS:
  void dictionaryFileChanged ( const QString & path );

private :

  Lima::Common::AbstractAccessByString* m_access;
  DictionaryData m_dicoData;
  Lima::FsaStringsPool* m_sp;
  bool m_isMainKeys;
  
};

inline uint64_t EnhancedAnalysisDictionary::getSize() const
{
  return m_dicoData.getSize();
}

inline DictionaryEntry EnhancedAnalysisDictionary::getEntry(const StringsPoolIndex wordId, const Lima::LimaString& word) const
{
  if (m_isMainKeys) return getEntryData(wordId);
  return getEntryData(static_cast<StringsPoolIndex>(m_access->getIndex(word)));
}

inline DictionaryEntry EnhancedAnalysisDictionary::getEntry(const Lima::LimaString& word) const
{
  return getEntryData( static_cast<StringsPoolIndex>(m_access->getIndex(word)) );
}

inline DictionaryEntry EnhancedAnalysisDictionary::getEntry(const StringsPoolIndex wordId) const
{
  if (m_isMainKeys) return getEntryData(wordId);
  return getEntryData( static_cast<StringsPoolIndex>(m_access->getIndex((*m_sp)[wordId])) );
}

}

}

}

#endif
