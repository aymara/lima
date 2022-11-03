// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        SimpleEventBuilder.h
  * @author      Romaric Besancon (romaric.besanconr@cea.fr) 

  *              Copyright (c) 2011 by CEA
  * @date        Created on Mar 25 2011
  *
  */

#ifndef LIMA_EVENTANALYSIS_SIMPLEEVENTBUILDER_H
#define LIMA_EVENTANALYSIS_SIMPLEEVENTBUILDER_H

#include "EventAnalysisExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/MediaticData/EntityType.h"
#include "Entity.h"

#include <string>
#include <map>

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis { 

#define SIMPLEEVENTBUILDER_CLASSID "SimpleEventBuilder"

class LIMA_EVENTANALISYS_EXPORT SimpleEventBuilder : public MediaProcessUnit
{
public:
  SimpleEventBuilder();

  virtual ~SimpleEventBuilder() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;
  
private:  
  std::string m_segmData;
  std::string m_segmentType;
  std::vector<Common::MediaticData::EntityType> m_entities;

  void getEntitiesFromSegment(std::map<Common::MediaticData::EntityType,std::vector<Entity> >& entities,
                              LinguisticAnalysisStructure::AnalysisGraph* graph,
                              LinguisticGraphVertex firstVertex,
                              LinguisticGraphVertex lastVertex,
                              const Common::AnnotationGraphs::AnnotationData* annotationData) const;
  const SpecificEntities::SpecificEntityAnnotation* 
  getSpecificEntity(LinguisticAnalysisStructure::AnalysisGraph* graph,
                    const Common::AnnotationGraphs::AnnotationData* annotationData,
                    LinguisticGraphVertex v) const;
                    
};

} // end namespace
} // end namespace
} // end namespace

#endif
