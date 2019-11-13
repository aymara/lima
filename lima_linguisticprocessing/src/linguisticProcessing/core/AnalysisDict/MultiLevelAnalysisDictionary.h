/*
    Copyright 2002-2019 CEA LIST

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
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTMULTILEVELANALYSISDICTIONARY_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTMULTILEVELANALYSISDICTIONARY_H

#include "AnalysisDictExport.h"
#include "AbstractAnalysisDictionary.h"
#include "DictionaryData.h"
#include "common/misc/AbstractAccessByString.h"
#include "common/misc/fsaStringsPool.h"

namespace Lima {

namespace LinguisticProcessing {

namespace AnalysisDict {

#define MULTILEVELANALYSISDICTIONARY_CLASSID "MultiLevelAnalysisDictionary"

class MultiLevelAnalysisDictionaryPrivate;
/**
@author Benoit Mathieu
@author Gaël de Chalendar
*/
class LIMA_ANALYSISDICT_EXPORT MultiLevelAnalysisDictionary : public AbstractAnalysisDictionary
{
  friend class MultiLevelAnalysisDictionaryPrivate;
  Q_OBJECT
public:
    MultiLevelAnalysisDictionary();

    virtual ~MultiLevelAnalysisDictionary();

    MultiLevelAnalysisDictionary(const MultiLevelAnalysisDictionary&) = delete;
    MultiLevelAnalysisDictionary& operator=(const MultiLevelAnalysisDictionary&) = delete;

    virtual void init(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager) override;

    virtual DictionaryEntry getEntry(const Lima::LimaString& word) const override;
    virtual DictionaryEntry getEntry(const StringsPoolIndex wordId) const override;
    virtual DictionaryEntry getEntry(const StringsPoolIndex wordId,
                                     const Lima::LimaString& word) const override;
    DictionaryEntry getEntry(const std::vector<uint64_t>& indexes) const;
    DictionaryEntry getEntry(const StringsPoolIndex wordId,
                             const std::vector<uint64_t>& indexes) const;

    virtual std::pair< DictionarySubWordIterator, DictionarySubWordIterator >
    getSubWordEntries(const int offset, const LimaString& key) const override;

    virtual std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator >
    getSuperWordEntries(const LimaString& key) const override;

    virtual uint64_t getSize() const override;

    uint64_t getDictionaryCount() const;


private:
  MultiLevelAnalysisDictionaryPrivate* m_d;
};

}

}

}

#endif
