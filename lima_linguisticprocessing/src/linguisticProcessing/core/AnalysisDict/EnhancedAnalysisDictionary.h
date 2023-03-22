// Copyright 2002-2015 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

class EnhancedAnalysisDictionaryPrivate;
/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDICT_EXPORT EnhancedAnalysisDictionary : public AbstractAnalysisDictionary
{
  Q_OBJECT
public:
    EnhancedAnalysisDictionary(const QString& dataFilePath = QString());
    
    EnhancedAnalysisDictionary(
      FsaStringsPool* sp,
      std::shared_ptr<Lima::Common::AbstractAccessByString> access,
      const std::string& dataFile);

    virtual ~EnhancedAnalysisDictionary();

    virtual void init(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager) override;
    
    virtual DictionaryEntry getEntry(const Lima::LimaString& word) const override;
    virtual DictionaryEntry getEntry(const StringsPoolIndex wordId) const override;
    virtual DictionaryEntry getEntry(const StringsPoolIndex wordId, 
                                     const Lima::LimaString& word) const override;
    /**
     * Returns subwords entries. Warning : Since all strings are stored as keys in dictionary, the subword
     * entries function may returns substrings which have empty dictionary entry. Program which use
     * this function should take care of it.
     * @param offset 
     * @param key 
     * @return subword iterators
     */
    virtual std::pair< DictionarySubWordIterator, DictionarySubWordIterator > 
    getSubWordEntries(const int offset, 
                      const LimaString& key) const override;

    virtual std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator > 
    getSuperWordEntries(const LimaString& key) const override;

    virtual uint64_t getSize() const override;
    
    DictionaryEntry getEntryData(const StringsPoolIndex entryId) const;

private Q_SLOTS:
  void dictionaryFileChanged ( const QString & path );
  void slotAccessFileReloaded(std::shared_ptr<Common::AbstractAccessByString> access);

private :
  EnhancedAnalysisDictionary(const EnhancedAnalysisDictionary& ead);
  EnhancedAnalysisDictionary& operator=(const EnhancedAnalysisDictionary& ead);

  EnhancedAnalysisDictionaryPrivate* m_d;
};

}

}

}

#endif
