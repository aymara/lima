// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        SimpleEventBuilder.cpp
  * @author      Romaric Besancon (romaric.besancon@cea.fr) 

  *              Copyright (c) 2011 by CEA
  * @date        Created on Mar 2011
  *
  */
 
#include "SimpleEventBuilder.h"
#include "EventData.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include <iostream>
#include <queue>

using namespace std;
using namespace boost;
using namespace boost::tuples;

using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::Automaton;


namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

SimpleFactory<MediaProcessUnit,SimpleEventBuilder> SimpleEventBuilderFactory(SIMPLEEVENTBUILDER_CLASSID);

SimpleEventBuilder::SimpleEventBuilder() : 
m_segmData(),
m_segmentType(),
m_entities()
{}

void SimpleEventBuilder::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                              Manager* manager)

{

  EVENTANALYZERLOGINIT;
  MediaId language=manager->getInitializationParameters().media;
  try
  {
    m_segmData=unitConfiguration.getParamsValueAtKey("segmentationData");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
    LERROR << "No segmentationData specified in "<<unitConfiguration.getName()
           <<" configuration group for language " << language;
  }
  
  try
  {
    m_segmentType=unitConfiguration.getParamsValueAtKey("segmentType");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
    LERROR << "No segmentType specified in "<<unitConfiguration.getName()
           <<" configuration group for language " << language;
  }

  try
  {
    deque<string> entities = unitConfiguration.getListsValueAtKey("entities");
    for(deque<string>::iterator it=entities.begin(),it_end=entities.end();it!=it_end;it++)
    {
      try {
        m_entities.push_back(Common::MediaticData::MediaticData::single().getEntityType(Common::Misc::utf8stdstring2limastring(*it)));
      }
      catch (LimaException& ) {
        LERROR << "Error: unknown entity type '"<< *it << "' : ignored";
      }
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
    LERROR << "No entities specified in "<<unitConfiguration.getName()
           <<" configuration group for language " << language;
  }
  
}

LimaStatusCode SimpleEventBuilder::process(AnalysisContent& analysis) const
{
  EVENTANALYZERLOGINIT;
  TimeUtils::updateCurrentTime();
  LDEBUG << "start SimpleEventBuilder";

  // get annotation data (for entities)
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "no annotation graph available !";
    return MISSING_DATA;
  }
    
  // get segmentation data
  AnalysisData* data=analysis.getData(m_segmData);
  if (data==0) {
    LERROR << "Missing data '" << m_segmData << "'";
    return MISSING_DATA;
  }
  SegmentationData* segmData=static_cast<SegmentationData*>(data);
  if (segmData==0)
  {
    LERROR << "Failed to interpret data '" << m_segmData << "' as SegmentationData";
    return MISSING_DATA;
  }

  // get graph on which the segmentation data relies
  string graphId=segmData->getGraphId();
  AnalysisGraph* graph=static_cast<AnalysisGraph*>(analysis.getData(graphId));
  if (graph==0) {
    LERROR << "Cannot get graph '" << graphId << "' (from segmentation data)";
    return MISSING_DATA;
  }
  
  EventData* eventData=new EventData;
  LDEBUG << "set new data EventData of type EventData";
  analysis.setData("EventData", eventData);

  // get entities
  map<Common::MediaticData::EntityType,vector<Entity> >& entities=eventData->getEntities();
  // ??OME2 for (SegmentationData::const_iterator it=segmData->begin(),it_end=segmData->end();it!=it_end;it++) {
  for (std::vector<Segment>::const_iterator it=(segmData->getSegments()).begin(),it_end=(segmData->getSegments()).end();it!=it_end;it++) {
    if ((*it).getType()==m_segmentType) {
      LDEBUG << "in segment " << m_segmentType << " [" << (*it).getPosBegin() << "," << (*it).getLength() << "]";
      // get entities in this segment
      getEntitiesFromSegment(entities,graph,(*it).getFirstVertex(),(*it).getLastVertex(),annotationData);
#ifdef DEBUG_LP
      LDEBUG << "found " << entities.size() << " entities";
#endif
    }
    else {
#ifdef DEBUG_LP
      LDEBUG << "ignored segment " << (*it).getType();
#endif
    }
  }

  // choose main entities : take first
  for (map<Common::MediaticData::EntityType,vector<Entity> >::iterator it=entities.begin(),
    it_end=entities.end();it!=it_end;it++) {
    if ((*it).second.size()!=0) {
#ifdef DEBUG_LP
     LDEBUG << "set main for entity of type " << (*it).first << " at pos " << (*it).second[0].getPosition();
#endif
      (*it).second[0].setMain(true);
    }
  }


  TimeUtils::logElapsedTime("SimpleEventBuilder");
  return SUCCESS_ID;
}

void SimpleEventBuilder::getEntitiesFromSegment(map<Common::MediaticData::EntityType,vector<Entity> >& entities,
                                                AnalysisGraph* anagraph,
                                                LinguisticGraphVertex firstVertex,
                                                LinguisticGraphVertex lastVertex,
                                                const Common::AnnotationGraphs::AnnotationData* annotationData) const
{
  LinguisticGraph* graph=anagraph->getGraph();
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  
  // get entities by going through the graph
  toVisit.push(firstVertex);
  
  bool first=true;
  bool last=false;
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (last || v == anagraph->lastVertex()) {
      continue;
    }
    if (v == lastVertex) {
      last=true;
    }
    
    for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph); outItr!=outItrEnd; outItr++) 
    {
      LinguisticGraphVertex next=target(*outItr,*graph);
      if (visited.find(next)==visited.end())
      {
        visited.insert(next);
        toVisit.push(next);
      }
    }
    
    if (first) {
      first=false;
    }
    else {
      // check if is entity
      const SpecificEntityAnnotation* se=getSpecificEntity(anagraph,annotationData,v);
      if (se!=0) {
        entities[se->getType()].push_back(Entity(se->getPosition(),se->getLength(),se->getFeatures()));
        EVENTANALYZERLOGINIT;
        LDEBUG << "inserted entity of type " << se->getType() << " at pos " << se->getPosition() << " with main=" << entities[se->getType()].back().getMain();
      }
    }
  }
}

const SpecificEntityAnnotation* SimpleEventBuilder::
getSpecificEntity(AnalysisGraph* graph,
                  const Common::AnnotationGraphs::AnnotationData* annotationData,
                  LinguisticGraphVertex v) const
{
  EVENTANALYZERLOGINIT;
  // then check if vertex corresponds to a specific entity found after POS tagging
  std::set< AnnotationGraphVertex > matches = annotationData->matches(graph->getGraphId(),v,"annot");
  LDEBUG << "vertex " << v << " has " << matches.size() << " annotations";
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      //BoWToken* se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin);
      const SpecificEntityAnnotation* se =
        annotationData->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
        pointerValue<SpecificEntityAnnotation>();
      return se;
    }
  }
  LDEBUG << "vertex " << v << ": no entity in graph " << graph->getGraphId();
  return 0;
}

} // end namespace
} // end namespace
} // end namespace
