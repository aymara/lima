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

#include "linguisticProcessing/core/LinguisticProcessors/AbstractLinguisticLogger.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{

#define SEMANTICRELATIONSXMLLOGGER_CLASSID "SemanticRelationsXmlLogger"
class SemanticRelationsXmlLogger : public AbstractLinguisticLogger
{
public:
  SemanticRelationsXmlLogger();

  virtual ~SemanticRelationsXmlLogger();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const;

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

