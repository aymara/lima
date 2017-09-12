/************************************************************************
 *
 * @file       SemanticRelationsXmlLogger.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Wed Sep 12 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * Project     s2lp
 * 
 * @brief xml logger for the semantic relation annotations from the
 * annotation graph
 * 
 * 
 ***********************************************************************/

#ifndef SEMANTICRELATIONSXMLLOGGERSEMANTICRELATIONSXMLLOGGER_H
#define SEMANTICRELATIONSXMLLOGGERSEMANTICRELATIONSXMLLOGGER_H

#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{

#define SEMANTICRELATIONSXMLLOGGER_CLASSID "SemanticRelationsXmlLogger"
class SemanticRelationsXmlLogger : public AbstractTextualAnalysisDumper
{
public:
  SemanticRelationsXmlLogger();

  virtual ~SemanticRelationsXmlLogger();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  MediaId m_language;
  std::string m_graph;

  // private memeber functions
  std::string vertexStringForSemanticAnnotation(const std::string& vertexRole, 
                                                const LinguisticGraphVertex& vertex,
                                                const VertexTokenPropertyMap& tokenMap,
                                                Common::AnnotationGraphs::AnnotationData* annotationData,
                                                uint64_t offset) const;

};

} // SemanticAnalysis
} // LinguisticProcessing
} // Lima

#endif
