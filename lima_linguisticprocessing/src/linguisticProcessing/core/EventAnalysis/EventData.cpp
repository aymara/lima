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
 * @file       EventData.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Mar 25 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "EventData.h"

#include "common/MediaticData/EntityType.h"
#include "common/MediaticData/mediaticData.h"
#include "EventTemplateData.h"
//#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
//#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "Events.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"


using namespace std;
using namespace Lima::Common;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

//***********************************************************************
// constructors and destructors
EventData::EventData():
AnalysisData(),
m_entities()
{
}

EventData::~EventData() {
}

void EventData::addEntity(const MediaticData::EntityType& type,const Entity& e)
{
  m_entities[type].push_back(e);
}

std::vector<MediaticData::EntityType> EventData::getEntityTypes() const
{
  std::vector<MediaticData::EntityType> types;
  for (std::map<MediaticData::EntityType,std::vector<Entity> >::const_iterator
    it=m_entities.begin(),it_end=m_entities.end();it!=it_end;it++) {
    types.push_back((*it).first);
  }
  return types;
}

const Entity& EventData::getMainEntity(const MediaticData::EntityType& type) const
{
  static Entity NullEntity;
  std::map<MediaticData::EntityType,std::vector<Entity> >::const_iterator
  entities=m_entities.find(type);
  if (entities!=m_entities.end()) {
    for (std::vector<Entity>::const_iterator it=(*entities).second.begin(),
      it_end=(*entities).second.end();it!=it_end;it++) {
      if ((*it).getMain()) {
        return (*it);
      }
    }
  }
  return NullEntity;
}

EventParagraph* EventData::locate( Event* event, const Entity *entity ) const {
  LOGINIT("LP::EventAnalysis");
  EventParagraph* result = 0;
  for( std::vector< EventParagraph* >::iterator pars = event->begin() ;
    pars != event->end() ; pars++ ) {
    LDEBUG << "EventData::convertToEvents: try paragraph ( "
           << (*pars)->getPosition() << ", " << (*pars)->getEnd() << " )..." << LENDL;
    if( (entity->getPosition() >= (*pars)->getPosition()) &&
        ((entity->getPosition()+entity->getLength()-1) <= (*pars)->getEnd() ) ) {
      return (*pars);
    }
  }
  return result;
}

//-------------------------------------------------------------------------------
// conversion to Events (for compatibility with EventExtraction web service)
Events* EventData::
convertToEvents(const SegmentationData* segmData) const
{
  LOGINIT("LP::EventAnalysis");
  
  Events* events=new Events();
  // Only one event = main event is inserted
  // because entities from only main event are managed
  Event* event=new Event();
  event->setMain();
  events->push_back(event);
  // may be more than one one paragraph per event

  // get list of segments
  int parNum = 1;
  std::vector<Lima::LinguisticProcessing::Segment> const &segments = segmData->getSegments();
//   const Segment *mainSegment = 0;
  // find paragraph of main event
  LDEBUG << "EventData::convertToEvents: looking for main segment..." << LENDL;
  for( std::vector<Lima::LinguisticProcessing::Segment>::const_iterator sIter = segments.begin() ;
    sIter != segments.end() ; sIter++ ) {
    LDEBUG << "EventData::convertToEvents: type of segment is " <<  sIter->getType() << LENDL;
    if( !sIter->getType().compare("MAIN_EVENT") ) {
//       mainSegment = &(*sIter);
      LDEBUG << "EventData::convertToEvents: location is ( "
             <<  sIter->getPosBegin() << ", " << sIter->getPosEnd() << " )" << LENDL;
      EventParagraph* eventPar=new EventParagraph();
      eventPar->setPosition(sIter->getPosBegin());
      eventPar->setLength(sIter->getLength());
      eventPar->setId(parNum++);
      event->push_back(eventPar);
    }
  }

  const map<Common::MediaticData::EntityType,vector<Entity> >& entities=getEntities();
//   int i = 0;
  for (map<Common::MediaticData::EntityType,vector<Entity> >::const_iterator
    it=entities.begin(),it_end=entities.end();it!=it_end;it++) {
    string entityName = Lima::Common::Misc::limastring2utf8stdstring(
             MediaticData::MediaticData::single().getEntityName((*it).first));
    LDEBUG << "EventData::convertToEvents: process entity of type " <<  entityName << LENDL;
    for(std::vector<Entity>::const_iterator e=(*it).second.begin(),e_end=(*it).second.end();e!=e_end;e++)
    {
      // int pos = e->getPosition();
      // int length = e->getlength();
      // const Lima::LinguisticProcessing::Automaton::EntityFeatures& features = e->getFeatures();
      //MediaticData::EntityType entityType = (*it).first; 
      Entity *entity = e->clone();
      EventParagraph* eventPar=locate(event, entity);
      if( eventPar == 0 ) {
        LWARN << "EventData::convertToEvents: cannot find paragraph where to place event "
             << entityName << " ( " <<  e->getPosition() << ", "
                                    << (e->getPosition()+e->getLength()-1) << " )" << LENDL;
      }
      else if( !entityName.compare( "Seisme.SEISMIC_EVENT" ) )
      {
        LDEBUG << "EventData::convertToEvents: addEventEntity( "
             << entityName << ", " <<  e->getOutputUTF8String() << " )" << LENDL;
        eventPar->addEventEntity( (*it).first, entity );
      }
      else
      {
        LDEBUG << "EventData::convertToEvents: addEntity( "
             << entityName << ", " <<  e->getOutputUTF8String() << " )" << LENDL;
        eventPar->addEntity( (*it).first, entity );
      }
    }
  }
  LDEBUG << "EventData::convertToEvents: end..." << LENDL;
  
  return events;
}


} // end namespace
} // end namespace
} // end namespace
