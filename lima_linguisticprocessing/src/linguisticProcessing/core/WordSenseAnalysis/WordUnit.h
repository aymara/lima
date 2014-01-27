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
#ifndef LIMA_WORDSENSEDISAMBIGUATION_WORDUNIT_H
#define LIMA_WORDSENSEDISAMBIGUATION_WORDUNIT_H

#include "WordSenseAnalysisExport.h"
#include <vector>
#include <set>
#include <map>
#include "WordSenseUnit.h"
#include "CommonTypedefs.h"
#include "KnnSearcher.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{

  
typedef std::map<uint64_t, std::string> Index2Lemma;
typedef std::map<std::string, uint64_t> Lemma2Index;



  
  
class LIMA_WORDSENSEANALYSIS_EXPORT WordUnit
{
  public:
     WordUnit();
     ~WordUnit();
     WordUnit(KnnSearcher* searcher, 
        const Lemma2Index& lemma2Index,
        const Index2Lemma& index2Lemma,
        std::string lemma,
        Mode mode, 
        std::string sensesPath);
     
     friend LIMA_WORDSENSEANALYSIS_EXPORT std::ostream& operator << (std::ostream& os, const WordUnit& wu);
     friend LIMA_WORDSENSEANALYSIS_EXPORT QDebug& operator << (QDebug& os, const WordUnit& wu);
     
     const std::string& lemma() const ;
     uint64_t lemmaId() const ;
     Mode mode() const ;
     int nbSenses() const ;
     const std::set<WordSenseUnit>& wordSensesUnits() const;
     
  protected:
    std::string m_lemma;
    uint64_t m_lemmaId;
    Mode m_mode;
    int m_nbSenses;
    std::set<WordSenseUnit> m_senses;
     
     
    int loadSenses(KnnSearcher* searcher, 
       const Lemma2Index& lemma2Index,
       const Index2Lemma& index2Lemma,
       std::string sensesPath);
};

inline const std::string& WordUnit::lemma() const
{
  return m_lemma;
}
inline uint64_t WordUnit::lemmaId() const
{
  return m_lemmaId;
}
inline Mode WordUnit::mode() const
{
  return m_mode;
}
inline int WordUnit::nbSenses() const
{
  return m_nbSenses;
}
inline const std::set<WordSenseUnit>& WordUnit::wordSensesUnits() const 
{
  return m_senses;
}

}  // namespace WordSenseDisambiguation
}  // namespace LinguisticProcessing
}  // namespace Lima


#endif // LIMA_WORDSENSEDISAMBIGUATION_WORDUNIT_H
