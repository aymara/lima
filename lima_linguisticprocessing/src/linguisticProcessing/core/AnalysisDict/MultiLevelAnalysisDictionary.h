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

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDICT_EXPORT MultiLevelAnalysisDictionary : public AbstractAnalysisDictionary
{
public:
    MultiLevelAnalysisDictionary();

    virtual ~MultiLevelAnalysisDictionary();
    
    virtual void init(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager);

    virtual DictionaryEntry getEntry(const Lima::LimaString& word) const;
    virtual DictionaryEntry getEntry(const StringsPoolIndex wordId) const;
    virtual DictionaryEntry getEntry(const StringsPoolIndex wordId, const Lima::LimaString& word) const;
    DictionaryEntry getEntry(const std::vector<uint64_t>& indexes) const;
    DictionaryEntry getEntry(const StringsPoolIndex wordId,const std::vector<uint64_t>& indexes) const;
    
    virtual std::pair< DictionarySubWordIterator, DictionarySubWordIterator > getSubWordEntries(const int offset, const LimaString& key) const;
    virtual std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator > getSuperWordEntries(const LimaString& key) const;
    virtual uint64_t getSize() const;
    
    inline uint64_t getDictionaryCount() const;


private:

  struct LevelDico {
    LevelDico() : id(),keys(0),data(),mainKeys(false) {};
    std::string id;
    Lima::Common::AbstractAccessByString* keys;
    DictionaryData* data;
    bool mainKeys;
  };
  
  /**
  * Les dicos sont ordonn�es du plus sp�cifique au plus g�n�rique.
  * L'ordre dans lequel ils sont d�clar�s dans le fichier de conf est l'ordre dans
  * lequel ils sont lus.
  */
  std::vector<LevelDico> m_dicos;
  Lima::FsaStringsPool* m_sp;
  uint64_t m_mainKeySize;
  
};

inline uint64_t MultiLevelAnalysisDictionary::getDictionaryCount() const {
  return m_dicos.size();
}

}

}

}

#endif
