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
  * @file        EventSegmentation.h
  * @author      Romaric Besancon (romaric.besancon@cea.fr) 

  *              Copyright (c) 2011 by CEA
  * created from EventAnalyzer
  * @date        Created on Mar 21 2011
  *
  */

#ifndef LIMA_EVENTANALYSIS_EVENTSEGMENTATION_H
#define LIMA_EVENTANALYSIS_EVENTSEGMENTATION_H

#include "EventAnalysisExport.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/MediaticData/EntityType.h"
#include "Event.h"
#include "Paragraph.h"


#include <string>
#include <map>

namespace Lima
{

namespace LinguisticProcessing
{

namespace EventAnalysis
{ 

#define EVENTSEGMENTATION_CLASSID "EventSegmentation"



class LIMA_EVENTANALISYS_EXPORT EventSegmentation : public MediaProcessUnit
{
public:
  EventSegmentation();

  virtual ~EventSegmentation() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;


  
private:  
  std::string m_graphId;
  std::string m_language;
  Common::MediaticData::EntityType m_dateEntity;
  Common::MediaticData::EntityType m_eventEntity;
  std::set<Common::MediaticData::EntityType> m_set_otherentities;
  
  std::map<Common::MediaticData::EntityType,unsigned short> m_entitiesWeights;
  std::vector<Event> m_events;
  std::map<uint64_t,Paragraph*> map_paragraphs;

protected:
  void compute_events(std::map<std::string,Event*>&, std::vector<Paragraph*>,LinguisticGraph*,Common::AnnotationGraphs::AnnotationData*, std::string) const;
  void compute_paragraphs(std::vector<Paragraph*>&, LinguisticGraph*,LinguisticAnalysisStructure::AnalysisGraph*,SegmentationData*,SegmentationData*,Common::AnnotationGraphs::AnnotationData*) const;
  bool is_a_bound(LinguisticGraphVertex, SegmentationData*) const;
  bool is_specific_entity(LinguisticGraphVertex,Common::AnnotationGraphs::AnnotationData*) const;
  bool is_specific_entity(LinguisticGraphVertex,Common::AnnotationGraphs::AnnotationData*,Common::MediaticData::EntityType) const;

  bool is_specific_entity_in(LinguisticGraphVertex,Common::AnnotationGraphs::AnnotationData*,std::set<Common::MediaticData::EntityType>) const;

  std::string getDate(LinguisticGraphVertex,Common::AnnotationGraphs::AnnotationData*,Common::MediaticData::EntityType) const;
  
  Common::MediaticData::EntityType getEntityType(LinguisticGraphVertex,Common::AnnotationGraphs::AnnotationData*, std::string graphId) const;

};



} // closing namespace EventAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_EVENTANALYSIS_EVENTANALYZER_H
