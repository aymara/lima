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
/**
  *
  * @file        WordSenseDisambiguator.h
  * @author      Claire Mouton (Claire.Mouton@cea.fr) 

  *              Copyright (c) 2010 by CEA
  * @date        Created on Aug, 17 2010
  *
  */


#ifndef LIMA_WORDSENSEDISAMBIGUATION_WORDSENSEDISAMBIGUATOR_H
#define LIMA_WORDSENSEDISAMBIGUATION_WORDSENSEDISAMBIGUATOR_H

#include "WordSenseAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "WordSenseAnnotation.h"


#include <map>
#include <set>
#include <string>
#include "WordUnit.h"
#include "WordSenseAnnotation.h"
#include "CommonTypedefs.h"

namespace Lima
{
namespace Common {
namespace AnnotationGraphs {
    class AnnotationData;
}
}
namespace LinguisticProcessing
{
namespace SyntacticAnalysis {
class SyntacticData;
}
namespace LinguisticAnalysisStructure {
class AnalysisGraph;
}
namespace WordSenseDisambiguation
{ 
class WordSenseAnnotation;
          
#define WORDSENSEDISAMBIGUATIONPU_CLASSID "WordSenseDisambiguation"



typedef std::map<int, std::string> SensesMapping;


/** @brief A simple process unit to test the annotation graphs */

class LIMA_WORDSENSEANALYSIS_EXPORT WordSenseDisambiguator : public MediaProcessUnit
{
public:
  WordSenseDisambiguator() {};

  ~WordSenseDisambiguator() { delete m_searcher; }

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

  // inline
  MediaId language() const;  
  const std::string& knnDir() const ;
  Mode mode() const ;
  Mapping mapping() const ;
  bool resolve() const;
  const Common::PropertyCode::PropertyAccessor* macroAccessor() const;
  const Common::PropertyCode::PropertyAccessor* microAccessor() const;
    
  Lemma2Index lemma2Index() const;
  Index2Lemma index2Lemma() const;
  uint64_t lemma2Index(std::string lemma) const;
  std::string index2Lemma(uint64_t index) const;
  const std::map<std::string, std::set<std::string> >& contextList() const ;
  std::set<std::string> contextList(std::string relation) const ;
  //end inline

private:  


protected:
  LinguisticCode m_L_NC;
  LinguisticCode m_L_NP;
  LinguisticCode m_L_V;
  LinguisticCode m_L_ADJ;   
  LinguisticCode m_L_ADV;
  
  KnnSearcher* m_searcher; 
  MediaId m_language;  
  std::string m_knnDir;
  std::map<std::string, std::set<std::string> >m_contextList;
  Mode m_mode;
  Mapping m_mappingMode;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  Index2Lemma m_index2Lemma;
  Lemma2Index m_lemma2Index;
  SensesMapping m_sensesMapping;  
  std::string m_sensesPath;
  
  void initDictionaries(const std::string& dictionaryPath);
  void loadMapping(const std::string& mappingPath);
  int getLemmas (LinguisticAnalysisStructure::MorphoSyntacticData* data,
                 const FsaStringsPool& stringspool,
                 std::set<std::string>& lemmas) const ;
  int getContext(SyntacticAnalysis::SyntacticData* syntacticData,
                 LinguisticGraphVertex& v,
                 LinguisticGraph* graph,
                 const FsaStringsPool& stringspool,
                 std::map<std::string, std::set<uint64_t> >& context) const ;
  int addPreviewWindowContext(std::vector<std::set<uint64_t> >& previewWindow,
                              std::map<std::string, std::set<uint64_t> >& context) const ;
  int addPostviewWindowContext(const std::set<uint64_t>& lemmasIds,
                               std::vector<TargetWordWithContext>& targetWordsWithContext) const ;

};


inline MediaId WordSenseDisambiguator::language() const
{
  return m_language;
}
inline const std::map<std::string, std::set<std::string> >& WordSenseDisambiguator::contextList() const
{
  return m_contextList;
}
inline const std::string& WordSenseDisambiguator::knnDir() const
{
  return m_knnDir;
}
inline Mode WordSenseDisambiguator::mode() const
{
  return m_mode;
}
inline Mapping WordSenseDisambiguator::mapping() const
{
  return m_mappingMode;
}
inline const Common::PropertyCode::PropertyAccessor* WordSenseDisambiguator::macroAccessor() const
{
  return m_macroAccessor;
}

inline 
  Lemma2Index WordSenseDisambiguator::lemma2Index() const
{
  return m_lemma2Index;
}
  Index2Lemma WordSenseDisambiguator::index2Lemma() const
{
  return m_index2Lemma;
}
inline 
  uint64_t WordSenseDisambiguator::lemma2Index(std::string lemma) const
{
  if (m_lemma2Index.find(lemma)!=m_lemma2Index.end())
  {
    return m_lemma2Index.find(lemma)->second;
  }
  return 0;
}
inline 
  std::string WordSenseDisambiguator::index2Lemma(uint64_t index) const
{
  if (m_index2Lemma.find(index)!=m_index2Lemma.end())
  {
    return m_index2Lemma.find(index)->second;
  }
  return "";
}

inline std::set<std::string> WordSenseDisambiguator::contextList(std::string relation) const 
{
  if (m_contextList.find(relation) != m_contextList.end()) 
  {
    return m_contextList.find(relation)->second;
  }
  return std::set<std::string>() ;
}

} // closing namespace WordSenseDisambiguation
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_WORDSENSEDISAMBIGUATION_WORDSENSEDISAMBIGUATOR_H
