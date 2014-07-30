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
 * @file     EventParagraph.cpp
 * @author   Faiza GARA
 * @date     April 2008
 * @version   $Id:
 * copyright Copyright (C) 2008 by CEA LIST
 * 
 ***********************************************************************/


#include "EventParagraph.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"



using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::SpecificEntities;

namespace Lima {
namespace LinguisticProcessing
{

namespace EventAnalysis
{ 

//**********************************************************************
// constructors,destructor,copy assignment
//**********************************************************************
EventParagraph::EventParagraph() : m_splitted(false), m_otherentities(), m_evententities(), m_length(0), m_position(0), m_id(0)
{
  
}

EventParagraph::EventParagraph(Paragraph *p,Common::AnnotationGraphs::AnnotationData* annotationData, std::string graphId,LinguisticGraph* graph)
{
  m_length=p->getLength();
  m_id=p->getId();
  m_position=p->getPosition();
  this->addEventEntities(p,annotationData,graphId,graph);
  this->addEntities(p,annotationData,graphId,graph);
  m_splitted=false;
}

EventParagraph::EventParagraph(Paragraph *p, bool splitted,Common::AnnotationGraphs::AnnotationData* annotationData, std::string graphId,LinguisticGraph* graph)
{
  m_length=p->getLength();
  m_id=p->getId();
  m_position=p->getPosition();
  this->addEventEntities(p,annotationData,graphId,graph);
  this->addEntities(p,annotationData,graphId,graph);
  m_splitted=splitted;
}

Lima::LinguisticProcessing::Automaton::EntityFeatures EventParagraph::getEntityFeatures(LinguisticGraphVertex v, Common::AnnotationGraphs::AnnotationData* annotationData, std::string graphId)
{
  EntityFeatures  features;
  
  std::set< AnnotationGraphVertex > matches = annotationData->matches(graphId,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;

    
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      
      
      EntityType  e;
      e=annotationData->annotation(vx,Common::Misc::utf8stdstring2limastring("SpecificEntity")).pointerValue< SpecificEntityAnnotation>()->getType();
      
        
      features=annotationData->annotation(vx,Common::Misc::utf8stdstring2limastring("SpecificEntity")).pointerValue< SpecificEntityAnnotation>()->getFeatures();
      return features;
      
    }
  }
  return features;
}

bool EventParagraph::has_entity(Common::MediaticData::EntityType t) const
{
  bool res=false;
  
  if (m_otherentities.find(t)!=m_otherentities.end())
  {
    return true;
  }
  
  if (m_evententities.first==t && m_evententities.second.size()> 0) return true;
  
  return res;
}

void EventParagraph::addEventEntities(Paragraph *p,Common::AnnotationGraphs::AnnotationData* annotationData, std::string graphId,LinguisticGraph* graph)
{
  std::pair<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > event_pair= p->getEventEntities();
  EVENTANALYZERLOGINIT;
  
  LDEBUG << "EventParagraph::addEventEntities EventEntitie type " << event_pair.first;
  std::vector<Entity *> v_entity;
  for(std::deque<LinguisticGraphVertex>::const_iterator iT= event_pair.second.begin(); iT != event_pair.second.end();iT++)
  {
    Entity *e = new Entity();
    Token* token = get(vertex_token, *graph, *iT);  
    e->setLength(token->length());
    e->setPosition(token->position());
    e->setMain(false);
    Lima::LinguisticProcessing::Automaton::EntityFeatures features=getEntityFeatures(*iT, annotationData, graphId);
    e->setFeatures(features);
    v_entity.push_back(e);
  }
  m_evententities=make_pair(event_pair.first,v_entity);
}
    
void EventParagraph::addEntities(Paragraph *p,Common::AnnotationGraphs::AnnotationData* annotationData, std::string graphId,LinguisticGraph* graph)
{
  std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > map_entities= p->getOtherEntities();
  for(std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> >::const_iterator iT1 = map_entities.begin(); iT1!=map_entities.end(); iT1++)
  {
    std::vector<Entity *> v_entity;
    for(std::deque<LinguisticGraphVertex>::const_iterator iT= iT1->second.begin(); iT != iT1->second.end();iT++)
    {
      Entity *e = new Entity();
      Token* token = get(vertex_token, *graph, *iT);  
      e->setLength(token->length());
      e->setPosition(token->position());
      e->setMain(false);
      Lima::LinguisticProcessing::Automaton::EntityFeatures features=getEntityFeatures(*iT, annotationData, graphId);
      e->setFeatures(features);
      v_entity.push_back(e);
    }
    m_otherentities[iT1->first]=v_entity;
  }
}

// Direct insertion of a known entity
void EventParagraph::addEventEntity(const Common::MediaticData::EntityType& type, Entity* entity)
{
  m_evententities.first=type;
  m_evententities.second.push_back(entity);
}

void EventParagraph::addEntity(const Common::MediaticData::EntityType& type, Entity* entity)
{
  m_otherentities[type].push_back(entity);
}

void EventParagraph::read(std::istream& file)
{
  m_position= Common::Misc::readCodedInt(file);
  m_length= Common::Misc::readCodedInt(file);
  EntityType t;
  t.setTypeId(static_cast<EntityTypeId>(Common::Misc::readCodedInt(file)));
  t.setGroupId(static_cast<EntityGroupId>(Common::Misc::readCodedInt(file)));
  m_evententities.first=t;
  int size=Common::Misc::readCodedInt(file);
  for (int i=0; i< size; i++)
  {
    Entity *e=new Entity();
    e->read(file);
    m_evententities.second.push_back(e);
  }
  int other_size=Common::Misc::readCodedInt(file);
  for (int i=0; i< other_size; i++)
  {
    EntityType to;
    to.setTypeId(static_cast<EntityTypeId>(Common::Misc::readCodedInt(file)));
    to.setGroupId(static_cast<EntityGroupId>(Common::Misc::readCodedInt(file)));
    int to_size=Common::Misc::readCodedInt(file);
    std::vector<Entity*> v;
    for (int j=0; j< to_size; j++)
    {
      Entity *e=new Entity();
      e->read(file);
      v.push_back(e);
    }
    m_otherentities[to]=v;
  }
}

void EventParagraph::write(std::ostream& file) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventParagraph::write().."<< LENDL; 
  Common::Misc::writeCodedInt(file,m_position);
  Common::Misc::writeCodedInt(file,m_length);
  LDEBUG << "EventParagraph::write: write typeId of m_evententities..."
      << m_evententities.first.getTypeId(); 
  Common::Misc::writeCodedInt(file,m_evententities.first.getTypeId());
  LDEBUG << "EventParagraph::write: write groupId of m_evententities..."
      << m_evententities.first.getGroupId(); 
  Common::Misc::writeCodedInt(file,m_evententities.first.getGroupId());
  Common::Misc::writeCodedInt(file,m_evententities.second.size());
  for(std::vector<Entity*>::const_iterator iT=m_evententities.second.begin();iT!=m_evententities.second.end();iT++)
  {
    LDEBUG << "EventParagraph::write m_evententities"<< LENDL; 
    (*iT)->write(file);
  }
  LDEBUG << "EventParagraph::write size of m_otherentities (" << m_otherentities.size() << ")...."<< LENDL; 
  Common::Misc::writeCodedInt(file,m_otherentities.size());
  for(std::map<EntityType,std::vector<Entity *> >::const_iterator iT=m_otherentities.begin();
      iT!=m_otherentities.end();iT++)
  {
  LDEBUG << "EventParagraph::write m_otherentity...."<< LENDL; 
    Common::Misc::writeCodedInt(file,(*iT).first.getTypeId());
    Common::Misc::writeCodedInt(file,(*iT).first.getGroupId());
    Common::Misc::writeCodedInt(file,(*iT).second.size());
    for(std::vector<Entity *>::const_iterator iT2 =(*iT).second.begin();iT2!=(*iT).second.end();iT2++)
    {
      (*iT2)->write(file);
    }
  }
}

EventParagraph::~EventParagraph()
{
  clear();
}

void EventParagraph::clear()
{
  for(std::vector<Entity *>::const_iterator iT=m_evententities.second.begin();iT!=m_evententities.second.end();iT++)
  {
    if ((*iT)!=0) delete (*iT);
  }
  m_evententities.second.clear();
  for(std::map<Common::MediaticData::EntityType,std::vector<Entity *> >::const_iterator iTm=m_otherentities.begin();iTm!=m_otherentities.end();iTm++)
  {
    
    for(std::vector<Entity *>::const_iterator iT=(*iTm).second.begin();iT!=(*iTm).second.end();iT++)
    {
      if ((*iT)!=0) delete (*iT);
    }
  }
  m_otherentities.clear();
}

std::string EventParagraph::toString(std::string parentURI, uint64_t index, bool main) const
{
  LIMA_UNUSED(parentURI);
  LIMA_UNUSED(index);
  LIMA_UNUSED(main);
  std::ostringstream oss;
  oss << "<rdf:rdf xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\""
      << " xmlns:dc=\"http://purl.org/dc/elements/1.1/\"" << std::endl;
  oss << "<dc:creator>CEA LIST Specific entities extraction service</dc:creator>";
  oss << "</rdf:rdf>";
//   return oss.str();
  
  std::string out;
  for(std::vector<Entity *>::const_iterator iT=m_evententities.second.begin();iT!=m_evententities.second.end();iT++)
  {
    
  }
  
  for(std::map<Common::MediaticData::EntityType,std::vector<Entity *> >::const_iterator iTm=m_otherentities.begin();iTm!=m_otherentities.end();iTm++)
  {
    
    for(std::vector<Entity *>::const_iterator iT=(*iTm).second.begin();iT!=(*iTm).second.end();iT++)
    {
      
    }
  }
  return out;
}


} // namespace 
} // namespace
} // namespace Lima
