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

#define SIMPLEXMLDUMPER_CLASSID "SimpleXmlDumper"

/**
@author Romaric Besancon
*/
class LIMA_ANALYSISDUMPERS_EXPORT SimpleXmlDumper : public AbstractTextualAnalysisDumper 
{
public:
  SimpleXmlDumper();

  virtual ~SimpleXmlDumper();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

protected:
  std::string m_graph;
  std::string m_property;
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
  const Common::PropertyCode::PropertyManager* m_propertyManager;

  // output of some specific properties (temporary: should be inserted in WordFeatures with XML output)
  bool m_outputVerbTense;
  bool m_outputTStatus;
  const Common::PropertyCode::PropertyAccessor* m_tenseAccessor;
  const Common::PropertyCode::PropertyManager* m_tenseManager;
  
  // private member functions
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
                         const FsaStringsPool& sp,const uint64_t offset) const;

  void xmlOutputVertex(std::ostream& out, 
                       LinguisticGraphVertex v,
                       const LinguisticAnalysisStructure::Token* ft,
                       LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                       LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                       const Common::AnnotationGraphs::AnnotationData* annotationData,
                       const FsaStringsPool& sp,
                       uint64_t offset) const;

  void xmlOutputVertexInfos(std::ostream& out, 
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

  // string manipulation functions to protect XML entities
  std::string xmlString(const std::string& str) const;
  void replace(std::string& str, const std::string& toReplace, const std::string& newValue) const;
                            
};

} // AnalysisDumpers
} // LinguisticProcessing
} // Lima

#endif
