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
 * @file       EventTemplateData.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Sep  2 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "EventTemplateData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "Events.h"
#include "Entity.h"

using namespace std;
using namespace Lima::Common;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;
//using namespace boost;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

// create default template data with one empty event template to store 
EventTemplateData::EventTemplateData():
AnalysisData(),
std::vector<EventTemplate>(1,EventTemplate())
{
}

EventTemplateData::~EventTemplateData() {
}

void EventTemplateData::addTemplate()
{
  push_back(EventTemplate());
}

void EventTemplateData::addElementInCurrentTemplate(const std::string& role, 
                                                    const EventTemplateElement& elt)
{
  back().addElement(role,elt);
}

void EventTemplateData::clearCurrentTemplate()
{
  back().clear();
}

void EventTemplateData::setTypeInCurrentTemplate(const std::string& type)
{
    LOGINIT("LP::EventAnalysis");
        LDEBUG << "set Current Template Type " << type << LENDL;
    back().setType(type);
    LDEBUG << "bak.getType " << back().getType() << LENDL; 
}

//-------------------------------------------------------------------------------
// conversion to Events (for compatibility with EventExtraction web service)
Events* EventTemplateData::
convertToEvents(const AnnotationData* annotationData) const
{
  Events* events=new Events();
  
  int eventNum=1;
  for (std::vector<EventTemplate>::const_iterator it= begin(); it!= end();it++)
  {
    const map<string,EventTemplateElement>& templateElements=(*it).getTemplateElements();
    if (! templateElements.empty()) {
      // one event per template
      Event* event=new Event();
      // only one paragraph per event
      EventParagraph* eventPar=new EventParagraph();
      event->push_back(eventPar);
      /*if ((*it).isMainEvent()) {
        event->setMain();
      }
      event->setWeight((*it).getWeight());*/
      eventPar->setId(eventNum);
      // set position and length of paragraph using position and length of template elements
      uint64_t posBeginPar(ULONG_MAX);
      uint64_t posEndPar(0);
      for(map<string,EventTemplateElement>::const_iterator it1= templateElements.begin(); it1!= templateElements.end();it1++)
      {
        const LinguisticAnalysisStructure::AnalysisGraph* graph=(*it1).second.getGraph();
        LinguisticGraphVertex v=(*it1).second.getVertex();
        MediaticData::EntityType type=(*it1).second.getType();
        Entity* e=createEntity(graph, v, annotationData, (*it1).first, eventNum);
        if (e->getPosition() < posBeginPar) {
          posBeginPar=e->getPosition();
        }
        uint64_t posEnd=e->getPosition()+e->getLength();
        if (posEnd > posEndPar) {
          posEndPar=posEnd;
        }
        if (e!=0) {
          eventPar->addEntity(type,e);
        }
      }
      eventPar->setPosition(posBeginPar);
      eventPar->setLength(posEndPar-posBeginPar);
      events->push_back(event);
      eventNum++;
    }
  }
  return events;
}

Entity* EventTemplateData::
createEntity(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             LinguisticGraphVertex v,
             const AnnotationData* annotationData,
             const std::string& role,
             uint64_t eventId)  const
{
  LinguisticAnalysisStructure::Token* token=get(vertex_token, *(graph->getGraph()), v);
  if (token==0) {
    LOGINIT("LP::EventAnalysis");    LWARN << "EventTemplateDataXmlLogger: no token for vertex " << v;
    return 0;
  }

  // store eventId feature as string (for output in WebLab)
  ostringstream eid;
  eid << eventId;

  // check if vertex corresponds to a specific entity found
  std::set< AnnotationGraphVertex > matches = annotationData->matches(graph->getGraphId(),v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
  it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      const SpecificEntityAnnotation* se =
      annotationData->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
      pointerValue<SpecificEntityAnnotation>();
      
      Automaton::EntityFeatures features=se->getFeatures();
      features.setFeature("role",role);
      features.setFeature("eventId",eid.str());
      Entity* e=new Entity(token->position(),token->length(),features);
      e->setMain(true);
      return e;
    }
  }
  
  // otherwise, have to create entity features
  Automaton::EntityFeatures features;
  features.setFeature("value",token->stringForm());
  features.setFeature("role",role);
  features.setFeature("eventId",eid.str());
  Entity* e=new Entity(token->position(),token->length(),features);
  e->setMain(true);
  return e;
}

} // end namespace
} // end namespace
} // end namespace
