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
 *   Copyright (C) 2010 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_SIMPLEXMLBOWDUMPER_H
#define LIMA_LINGUISTICPROCESSING_SIMPLEXMLBOWDUMPER_H

#include "AnalysisDumpersExport.h"

#include "WordFeatureExtractor.h"
#include "BoWFeatureExtractor.h" // for compounds (stored in BoWTerms)

#include "common/MediaProcessors/MediaProcessUnit.h"

#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "BowGeneration.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

#define GENERICXMLDUMPER_CLASSID "GenericXmlDumper"

/**
@author Romaric Besancon
*/
class LIMA_ANALYSISDUMPERS_EXPORT GenericXmlDumper : public AbstractTextualAnalysisDumper 
{
public:
  GenericXmlDumper();

  virtual ~GenericXmlDumper();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const;

protected:
  std::string m_graph;
  WordFeatures m_features; //!< use dedicated class for feature storage (easy initialization functions)
  BoWFeatures m_bowFeatures; //!< use dedicated class for feature storage (easy initialization functions)
  std::deque<std::string> m_featureNames; //!< use additional vector (aligned) to store feature names
  std::vector<std::string> m_featureTags; //!< use additional vector (aligned) to store associated XML tags 
  std::map<std::string,std::string> m_defaultFeatures;
  bool m_outputWords;              //!< output simple words
  bool m_outputSentenceBoundaries; //!< output sentence boundaries (enclosing sentence tags)
  bool m_outputSpecificEntities;   //!< output specific entities
  bool m_outputSpecificEntityParts; //!< output parts of specific entities
  bool m_outputCompounds;     //!< output compounds
  bool m_outputCompoundParts; //!< output also compound parts
  bool m_outputAllCompounds;  //!< output all partial compounds (created using BoWToken iterator)
  std::string m_wordTag;
  std::string m_sentenceBoundaryTag;
  std::string m_specificEntityTag;
  std::string m_compoundTag;
  Compounds::BowGenerator* m_bowGenerator;

  //   std::string m_property;
//   const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
//   const Common::PropertyCode::PropertyManager* m_propertyManager;
// 
//   // output of some specific properties (temporary: should be inserted in WordFeatures with XML output)
//   bool m_outputVerbTense;
//   bool m_outputTStatus;
//   const Common::PropertyCode::PropertyAccessor* m_tenseAccessor;
//   const Common::PropertyCode::PropertyManager* m_tenseManager;
  
  // private member functions
  void clearFeatures();
  void initializeFeatures(const std::map<std::string,std::string>& features,
                          const std::deque<std::string>& featureOrder=std::deque<std::string>());
  
  void xmlOutput(std::ostream& out,
                 AnalysisContent& analysis,
                 LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                 LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                 const Common::AnnotationGraphs::AnnotationData* annotationData,
                 const SyntacticAnalysis::SyntacticData* syntacticData) const;
  
  void xmlOutputVertices(std::ostream& out,
                         LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                         LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                         const Common::AnnotationGraphs::AnnotationData* annotationData,
                         const SyntacticAnalysis::SyntacticData* syntacticData,
                         const LinguisticGraphVertex begin,
                         const LinguisticGraphVertex end,
                         const FsaStringsPool& sp,
                         const uint64_t offset) const;

  void xmlOutputVertex(std::ostream& out, 
                       LinguisticGraphVertex v,
                       LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                       LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                       const Common::AnnotationGraphs::AnnotationData* annotationData,
                       const SyntacticAnalysis::SyntacticData* syntacticData,
                       const FsaStringsPool& sp,
                       uint64_t offset,
                       std::set<LinguisticGraphVertex>& visited,
                       std::set<LinguisticGraphVertex>& alreadyStoredVertices) const;

  void xmlOutputVertexInfos(std::ostream& out, 
                            LinguisticGraphVertex v,
                            LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                            uint64_t offset) const;

  void xmlOutputBoWInfos(std::ostream& out, 
                         Common::BagOfWords::AbstractBoWElement* token,
                         uint64_t offset) const;
                          
  /**
   * check if a vertex is a specific entity: returns the specific entity annotation 
   * if it is the case, along with the graph in which this entity has been found 
   * (can be analysis graph or pos graph). 
   * Returns (0,0) if not a specific entity
   */
  std::pair<const SpecificEntities::SpecificEntityAnnotation*,LinguisticAnalysisStructure::AnalysisGraph*>
  checkSpecificEntity(LinguisticGraphVertex v,
                      LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                      LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                      const Common::AnnotationGraphs::AnnotationData* annotationData) const;
                            
  bool xmlOutputSpecificEntity(std::ostream& out, 
                               const SpecificEntities::SpecificEntityAnnotation* se,
                               LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                               const FsaStringsPool& sp,
                               uint64_t offset) const;
                               
  // hack to get compatible features between specific entities and words
  // without having to define abstract Feature Extractors for SpecificEntityAnnotation
  std::string specificEntityFeature(const SpecificEntities::SpecificEntityAnnotation* se,
                                    const std::string& featureName,
                                    const FsaStringsPool& sp,
                                    uint64_t offset) const;
                               
  std::vector< QSharedPointer< Common::BagOfWords::BoWToken > >
  checkCompound(LinguisticGraphVertex v,
                LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                const Common::AnnotationGraphs::AnnotationData* annotationData,
                const SyntacticAnalysis::SyntacticData* syntacticData,
                uint64_t offset,
                std::set<LinguisticGraphVertex>& visited) const;

  void xmlOutputCompound(std::ostream& out, QSharedPointer<Lima::Common::BagOfWords::AbstractBoWElement> token, Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph* anagraph, Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph* posgraph, const Lima::Common::AnnotationGraphs::AnnotationData* annotationData, const Lima::FsaStringsPool& sp, uint64_t offset) const;
                               
  /*void xmlOutputVertexInfos(std::ostream& out, 
                            const LinguisticAnalysisStructure::Token* ft,
                            const std::vector<LinguisticAnalysisStructure::MorphoSyntacticData*>& data,
                            const FsaStringsPool& sp,
                            uint64_t offset,
                            LinguisticCode category=LinguisticCode(0)) const;

  bool outputSpecificEntity(std::ostream& out, 
                            const SpecificEntities::SpecificEntityAnnotation* se,
                            LinguisticAnalysisStructure::MorphoSyntacticData* data,
                            const LinguisticGraph* graph,
                            const FsaStringsPool& sp,
                            const uint64_t offset) const;
  */
  // string manipulation functions to protect XML entities
  std::string xmlString(const std::string& str) const;
  void replace(std::string& str, const std::string& toReplace, const std::string& newValue) const;
                            
};

} // AnalysisDumpers
} // LinguisticProcessing
} // Lima

#endif
