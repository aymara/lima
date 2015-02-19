/*
    Copyright 2002-2014 CEA LIST

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

#ifndef LIMA_LINGUISTICPROCESSING_CRFSEDUMPER_H
#define LIMA_LINGUISTICPROCESSING_CRFSEDUMPER_H

#include "AnalysisDumpersExport.h"

#include "WordFeatureExtractor.h"

#include "common/MediaProcessors/MediaProcessUnit.h"

#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

#define CRFSEDUMPER_CLASSID "CrfSEDumper"


class LIMA_ANALYSISDUMPERS_EXPORT CrfSEDumper : public AbstractTextualAnalysisDumper 
{

public:
  CrfSEDumper();

  virtual ~CrfSEDumper();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const;

protected:
  std::string m_graph;
  WordFeatures m_features; //!< use dedicated class for feature storage (easy initialization functions)
  std::map<AbstractFeatureExtractor*,std::string> m_featuresMap; //!< use additional map to associate features with XML tags (only pointers)
  std::map<std::string,std::string> m_defaultFeatures;
  bool m_outputSentenceBoundaries;
  bool m_outputSpecificEntities;
  std::map<std::string, std::string> m_NEauthorized;
  
  // private member functions
  void clearFeatures();
  void initializeFeatures(const std::map<std::string,std::string>& features,
                          const std::deque<std::string>& featureOrder=std::deque<std::string>());
  
  void xmlOutput(std::ostream& out,
                 AnalysisContent& analysis,
                 LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                 LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                 const Common::AnnotationGraphs::AnnotationData* annotationData) const;
  
  void xmlOutputVertices(std::ostream& out,
                         LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                         LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                         const Common::AnnotationGraphs::AnnotationData* annotationData,
                         const LinguisticGraphVertex begin,
                         const LinguisticGraphVertex end,
                         const FsaStringsPool& sp,
                         const uint64_t offset) const;

  /**
   * @brief display Vertex Infos
   */
  void xmlOutputVertex(std::ostream& out, 
                       LinguisticGraphVertex v,
                       LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                       LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                       const Common::AnnotationGraphs::AnnotationData* annotationData,
                       const FsaStringsPool& sp,
                       uint64_t offset) const;

  /**
   * @brief Display Vertex Infos if several interpretation of the token
   */
  void xmlOutputVertexInfos(std::ostream& out, 
                            LinguisticGraphVertex v,
                            LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                            uint64_t offset) const;


  // string manipulation functions to protect XML entities
  std::string xmlString(const std::string& str) const;
  void replace(std::string& str, const std::string& toReplace, const std::string& newValue) const;


};
}
}
}


#endif
