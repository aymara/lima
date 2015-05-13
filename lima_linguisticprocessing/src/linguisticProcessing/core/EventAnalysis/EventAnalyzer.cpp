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
  * @file        EventAnalyzer.cpp
  * @author      Faiza Gara (faiza.gara@cea.fr) 

  *              Copyright (c) 2008 by CEA
  * @date        Created on April 2008
  *
  */
 
#include "EventAnalyzer.h"
#include "Events.h"

 
#include "common/AbstractFactoryPattern/SimpleFactory.h"

#include "common/time/traceUtils.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

#include <iostream>
#include <queue>


using namespace std;
//using namespace boost;
using namespace boost::tuples;

using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::Automaton;


namespace Lima
{
namespace LinguisticProcessing
{
namespace EventAnalysis
{
SimpleFactory<MediaProcessUnit,EventAnalyzer> EventAnalyzerFactory(EVENTANALYZERPU_CLASSID);



EventAnalyzer::EventAnalyzer() : 
m_graphId("PosGraph"),
m_language(),
m_dateEntity(),
m_eventEntity(),
m_set_otherentities(),
m_entitiesWeights(),
map_paragraphs()
{}


void EventAnalyzer::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  EVENTANALYZERLOGINIT;

  LDEBUG << "EventAnalyzer::init()...";
  m_language=manager->getInitializationParameters().media;
  try
  {
    m_graphId=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
    
  } // use default value Posgraph
  
  try
  {
    string dateEntity=unitConfiguration.getParamsValueAtKey("DateEntity");
    m_dateEntity=Common::MediaticData::MediaticData::single().getEntityType(Common::Misc::utf8stdstring2limastring(dateEntity));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
    LERROR << "No DateEntity defined in "<<unitConfiguration.getName()<<" configuration group for language " << m_language;
  }
  try
  {
    string eventEntity=unitConfiguration.getParamsValueAtKey("EventEntity");
    m_eventEntity=Common::MediaticData::MediaticData::single().getEntityType(Common::Misc::utf8stdstring2limastring(eventEntity));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
     LERROR << "No EventEntity defined in "<<unitConfiguration.getName()<<" configuration group for language " << m_language;
  }
    try
  {
    deque<string> otherEntities = unitConfiguration.getListsValueAtKey("OtherEntities");
    for(deque<string>::iterator itr=otherEntities.begin();itr !=otherEntities.end();itr++)
    {
      m_set_otherentities.insert(Common::MediaticData::MediaticData::single().getEntityType(Common::Misc::utf8stdstring2limastring(*itr)));
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
     LERROR << "No OtherEntities defined in "<<unitConfiguration.getName()<<" configuration group for language " << m_language;
  }
   try
  {
    std::map<std::string,std::string>& weights=unitConfiguration.getMapAtKey("EntitiesWeights");
    LDEBUG << "Weights map size =" << weights.size();
    for (std::map<std::string,std::string>::const_iterator it=weights.begin();
         it!=weights.end();
         it++)
    { 
      LDEBUG << "Init EntitiesWeights: "
             << it->first << ", "
             << "entityType=" << Common::MediaticData::MediaticData::single().
                     getEntityType(Common::Misc::utf8stdstring2limastring(it->first))
             << " , weight " << atoi((it->second).c_str());
       m_entitiesWeights[Common::MediaticData::MediaticData::single().getEntityType(Common::Misc::utf8stdstring2limastring(it->first))]=atoi((it->second).c_str());
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchMap& )
 {
    LERROR << "No map 'EntitiesWeigths' in "<<unitConfiguration.getName()<<" configuration group for language " << m_language;

  }
  
}

/**
 * 
 * @param analysis 
 * @return 
 */
LimaStatusCode EventAnalyzer::process(
  AnalysisContent& analysis) const
{
  EVENTANALYZERLOGINIT;
  TimeUtils::updateCurrentTime();
  LDEBUG << "start EventAnalyzer";
  
  // ici normalement on peut prendre soit analysis graph soit le Posgraph, cela doit être paramétré
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graphId));
  if (anagraph==0)
  {
    LERROR << "no "<< m_graphId << " ! abort";
    return MISSING_DATA;
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "no AnnotationData ! abort";
    return MISSING_DATA;
  }
  SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    LERROR << "no SentenceBoundaries ! abort";
    return MISSING_DATA;
  }
  SegmentationData* pb=static_cast<SegmentationData*>(analysis.getData("ParagraphBoundaries"));
  if (pb==0)
  {
    LERROR << "no ParagraphBoundaries ! abort";
    return MISSING_DATA;
  }
  std::vector<Paragraph*> v_paragraph;
  compute_paragraphs(v_paragraph,anagraph->getGraph(),anagraph,sb,pb,annotationData);
  
  std::map<std::string,Event*> map_event;
  compute_events(map_event,v_paragraph,anagraph->getGraph(),annotationData,m_graphId);
  
  Events * eventData=new Events();
  
  // Computes entities weight of events
  uint64_t max_weight=0;
  Event* main_event=0;
  for (std::map<std::string,Event*>::const_iterator iT=map_event.begin();iT!=map_event.end();iT++)
  {
    (*iT).second->compute_entities_weight(m_entitiesWeights,annotationData,m_graphId);
    if ((*iT).second->get_weight() > max_weight)
    {
      max_weight=(*iT).second->get_weight();
      main_event=(*iT).second;
    }
    
    eventData->push_back((*iT).second);
  }
  LDEBUG << "Le nombre d'évènements différents est égal à = " << map_event.size();
  
  LDEBUG << "set new data 'EventData' of type Events";
  analysis.setData("EventData",eventData);
  
  if (main_event!=0)
  {
    LDEBUG << "Le meilleur poids est égal à = " << max_weight;
    main_event->setMain();
    main_event->compute_main_entities();
  }
  
  int i=0;
  for (std::vector<Event*>::const_iterator iT= eventData->begin(); iT!= eventData->end();iT++)
  {
    i++;
    LDEBUG << "Event  N° " << i;
    LDEBUG << " a pour poids " << (*iT)->get_weight();
    LDEBUG << " a pour valeur main " << (*iT)->getMain();
    LDEBUG << " est composé des fragments de texte suivants ";
    int j=0;
    for(std::vector<EventParagraph*>::const_iterator iT1= (*iT)->begin(); iT1!= (*iT)->end();iT1++)
    {
      LDEBUG << "    paragraph " << j << " position = " << (*iT1)->getPosition() << " , longueur = " << (*iT1)->getLength() ;
      std::map<Common::MediaticData::EntityType,std::vector<Entity *> > otherEntities=(*iT1)->getOtherEntities();
      LDEBUG << "      les entités sont";
      for(std::map<Common::MediaticData::EntityType,std::vector<Entity *> >::const_iterator iT2= otherEntities.begin(); iT2!= otherEntities.end();iT2++)
      {
        LDEBUG << "        type=" << (*iT2).first;
        for(std::vector<Entity *>::const_iterator iT3=(*iT2).second.begin();iT3!=(*iT2).second.end();iT3++)
        {
          LDEBUG << "        position=" << (*iT3)->getPosition() << "  ,longueur = " <<(*iT3)->getLength()<< ", main =" << (*iT3)->getMain();
          Lima::LinguisticProcessing::Automaton::EntityFeatures features= (*iT3)->getFeatures();
          for (Automaton::EntityFeatures::const_iterator 
               featureItr=features.begin(),features_end=features.end();
               featureItr!=features_end; featureItr++)
          {
            LDEBUG << "        Feature=" << featureItr->getName() << ",  value=" << featureItr->getValueString();
          }
        }
      }
      std::pair<Common::MediaticData::EntityType,std::vector<Entity *> > eventEntities=(*iT1)->getEventEntities();
      LDEBUG << "        Event type=" << eventEntities.first;
      for(std::vector<Entity *>::const_iterator iT3=eventEntities.second.begin();iT3!=eventEntities.second.end();iT3++)
      {
        LDEBUG << "        position=" << (*iT3)->getPosition() << "  ,longueur = " <<(*iT3)->getLength()<< ", main =" << (*iT3)->getMain();
        Lima::LinguisticProcessing::Automaton::EntityFeatures features= (*iT3)->getFeatures();
        for (Automaton::EntityFeatures::const_iterator 
             featureItr=features.begin(),features_end=features.end();
             featureItr!=features_end; featureItr++)
        {
          LDEBUG << "        Feature=" << featureItr->getName() << ",  value=" << featureItr->getValueString();
        }
      }
      j++;
    }
  }
  
  TimeUtils::logElapsedTime("EventAnalyzer");
  return SUCCESS_ID;
}

void EventAnalyzer::compute_events(std::map<std::string,Event*>& map_event, std::vector<Paragraph*> v_par,LinguisticGraph* graph,Common::AnnotationGraphs::AnnotationData* annotationData, std::string graphId) const
{
  EVENTANALYZERLOGINIT;
  
  
  for (uint64_t i=0; i<v_par.size();i++)
  {
    Paragraph *p=v_par[i];
      
    
    if (p->toFilter() ||  (p->getDatesSize()==0))
    {
      // ignorer le paragraphe complètement et le supprimer
      delete(v_par[i]);
      LDEBUG << "Paragraph numéro : " << (i+1) << "  a filtrer ";
    }
    else
    {        
      LDEBUG << "Paragraph numéro : " << (i+1) << " a étudier ayant comme nombre de date = " << p->getDatesSize();
      // Il faut juste ajouter le paragraphe dans l'évènement concerné
      if (p->getDatesSize()==1)
      {
        LDEBUG << "   La date est " << p->getDate().first;
        std::pair<string,LinguisticGraphVertex> date=p->getDate();
        if (map_event.find(date.first)!=map_event.end())
        {
          Event *ev=map_event[date.first];
          ev->addParagraph(p,true,false,annotationData,graphId,graph);
          map_event[date.first]=ev;
        }
        else
        {
          Event *ev=new Event();
          ev->setDate(make_pair(m_dateEntity,date));
          LDEBUG << "Creation d'un nouvel évènement ";
          ev->addParagraph(p,true,false,annotationData,graphId,graph);
          map_event[date.first]=ev;
        }
      }
      else // Cas complexe
      {
        bool first_time=true;
        while(p->getDatesSize()>1)
        {
          Paragraph *p1 =new Paragraph();
          p1->setPosition(p->getPosition());
          std::pair<std::string,LinguisticGraphVertex> date1=p->extractDate();
          std::pair<std::string,LinguisticGraphVertex> date2=p->getDate();
          Token *t_date2=get(vertex_token,*graph,date2.second);
          p1->setPosition(p->getPosition());
          p1->addDate(date1.first,date1.second);
          LinguisticGraphVertex split=date2.second;
          bool end=(p->getSentencesSize()==0);
          while(!end)
          {
            LinguisticGraphVertex sentence=p->extractSentence();
            Token *t_sentence=get(vertex_token,*graph,sentence);
            if(t_sentence->position() < t_date2->position())
              split=sentence;
            else
            {
              p->addSentence(sentence);
              end=true;
            }          
          }
        
          Token *t_split=get(vertex_token,*graph,split);
          p1->setLength(t_split->position()-p->getPosition());
          p->setPosition(t_split->position());
          p->setLength(p->getLength()-p1->getLength());
          
          std::map<Common::MediaticData::EntityType, std::deque<LinguisticGraphVertex> > entities = p->extractEntitiesBeforeVertex(split,graph);
          
          std::pair<Common::MediaticData::EntityType, std::deque<LinguisticGraphVertex> > evententities = p->extractEventEntitiesBeforeVertex(split,graph);
          
          LDEBUG << "EventAnalyzer evententities type " << evententities.first;
          p1->addEventEntities(evententities);
          p1->addEntities(entities);
          
          if (p1->toFilter())
          {
            delete(p1);
          }
          else
          {
            std::pair<string,LinguisticGraphVertex> date=p1->getDate();
            if (map_event.find(date.first)!=map_event.end())
            {
              Event *ev=map_event[date.first];
              ev->addParagraph(p1,first_time,true,annotationData,graphId,graph);
              map_event[date.first]=ev;
            }
            else
            {
              Event *ev=new Event();
              ev->setDate(make_pair(m_dateEntity,date));
              LDEBUG << "Creation d'un nouvel évènement ";
              ev->addParagraph(p1,first_time,true,annotationData,graphId,graph);
              map_event[date.first]=ev;
            }
          }
          first_time=false;
        }// end while
        // traiter la dernière date
        if (p->toFilter())
        {
          delete(p);
        }
          else{
          std::pair<string,LinguisticGraphVertex> date=p->getDate();
          if (map_event.find(date.first)!=map_event.end())
          {
            Event *ev=map_event[date.first];
            ev->addParagraph(p,false,false,annotationData,graphId,graph);
            map_event[date.first]=ev;
          }
          else
          {
            Event *ev=new Event();
            ev->setDate(make_pair(m_dateEntity,date));
            LDEBUG << "Creation d'un nouvel évènement ";
            ev->addParagraph(p,false,false,annotationData,graphId,graph);
            map_event[date.first]=ev;
          }
        }
      }
    }   
  }
}

void EventAnalyzer::compute_paragraphs(std::vector<Paragraph*>& v_par, 
                                       LinguisticGraph* graph,
                                       AnalysisGraph* anagraph,
                                       SegmentationData* sb,
                                       SegmentationData* pb, 
                                       Common::AnnotationGraphs::AnnotationData* annotationData) const
{
  EVENTANALYZERLOGINIT;
  

//   LinguisticGraphVertex v;
  
  const LinguisticGraphVertex firstVx = anagraph->firstVertex();
  const LinguisticGraphVertex lastVx = anagraph->lastVertex();
  
  std::set<LinguisticGraphVertex> visited;
  std::queue<LinguisticGraphVertex> toVisit;
  LDEBUG << "compute_paragraphs:  push  vertex " << firstVx;
  toVisit.push(firstVx);

  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  
  Paragraph *p=new Paragraph();
  uint64_t id_paragraph=1;
  p->setId(id_paragraph);
    
  Token* token=0;
  Token* previous_token=0;
  uint64_t position;
//   uint64_t length=0;
  
  v_par.push_back(p);
  std::string current_date= "00-00-00";
  
  while (!toVisit.empty())
  {
    LinguisticGraphVertex v=toVisit.front();
    LDEBUG << "compute_paragraphs:  pop  vertex " << v;
    toVisit.pop();
    if (v != lastVx) {
    

      for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph);
         outItr!=outItrEnd;
         outItr++)
      {
        LinguisticGraphVertex next=target(*outItr,*graph);
        if (visited.find(next)==visited.end())
        {
          visited.insert(next);
          LDEBUG << "compute_paragraphs:  push vertex " << next;
          toVisit.push(next);
        }
      }
    }
    if (v != firstVx && v != lastVx)
    {
      LDEBUG << "Traitement du vertex " << v;
      LDEBUG << "current_date du vertex " << current_date;
      token = get(vertex_token, *graph, v);  
     // it is a vertex of a new paragraph
      if(is_a_bound(v,pb) )
      {
       
        LDEBUG << "Je suis dans le Début d'un nouveau paragraphe ";
        current_date= "00-00-00";
       // créer la map du vertex
        uint64_t par_position=v_par[v_par.size()-1]->getPosition();
        uint64_t last_position=previous_token->position()+previous_token->length()-1;
        v_par[v_par.size()-1]->setLength(last_position-par_position+1);
       
        position= token->position();
        v_par.push_back(new Paragraph());
        id_paragraph++;
        v_par[v_par.size()-1]->setId(id_paragraph);
        v_par[v_par.size()-1]->setPosition(position);
      }
  
      if (is_specific_entity(v,annotationData))
      {
        LDEBUG << "Je suis dans un vertex de type Entité nommée ";
        // verify if it is a date
        if(is_specific_entity(v,annotationData,m_dateEntity))
        {
          string date= getDate(v,annotationData,m_dateEntity);
          LDEBUG << "Je suis dans Date ";
          LDEBUG << "Valeur de la Date =" << date;
          if (date.compare("00-00-00")==0)
          {
               // ignorer la date
            LDEBUG << "Date mal normalisée à ignorer";
          }
          else
          {
            v_par[v_par.size()-1]->addDate(date,v);
            current_date=date;             
          }
        }
        // verify if it is a event
        else if(is_specific_entity(v,annotationData,m_eventEntity))
        {
         // mettre l'entités nommées dans le bon para
          LDEBUG << "Je suis dans Evenement ";
          v_par[v_par.size()-1]->addEventEntity(m_eventEntity,v);
        }
        // verify if it is is the set of entities domain
        else if(is_specific_entity_in(v,annotationData,m_set_otherentities))
        {
         // mettre l'entités nommées dans le bon para
          LDEBUG << "Je suis dans les autres types d'EN ";
          Common::MediaticData::EntityType e =getEntityType(v,annotationData,m_graphId);
          v_par[v_par.size()-1]->addEntity(e,v);
        }
        //ignore if it is a another type of entity               
      }
       
     // it is the last position of the current sentence
      else if (is_a_bound(v,sb))
      {
        LDEBUG << "Je suis dans fin d'une phrase ";
       // ajouter la position de la phrase dans le paragraphe
        v_par[v_par.size()-1]->addSentence(v);
      }
    }
    else
    {
      if (v == lastVx)
      {
          uint64_t par_position=v_par[v_par.size()-1]->getPosition();
          uint64_t last_position=previous_token->position()+previous_token->length()-1;
          v_par[v_par.size()-1]->setLength(last_position-par_position+1);
          LDEBUG << "par_position " << par_position << ", last_position" << last_position;
        
      }
    }
    previous_token=token;
  }
  
}

 
bool EventAnalyzer::is_a_bound(LinguisticGraphVertex v, SegmentationData* tb) const
{
// ??OME2  for (SegmentationData::const_iterator it=tb->begin(),it_end=tb->end();
  for (std::vector<Segment>::const_iterator it=(tb->getSegments()).begin(),
       it_end=(tb->getSegments()).end();
       it!=it_end; it++) 
  {
    if ((*it).getLastVertex() == v) return true;
  }
  return false;
}
 
bool EventAnalyzer::is_specific_entity(LinguisticGraphVertex v,AnnotationData* annotationData) const
{
  // OME
  EVENTANALYZERLOGINIT;
  // OME 
  LDEBUG << "is_specific_entity at " << v << " according to annot?";
  std::set< AnnotationGraphVertex > matches = annotationData->matches(m_graphId,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
//     AnnotationGraphVertex vx=*it;
    
    if (annotationData->hasAnnotation(*it, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      LDEBUG << "       ...return true";
      return true;
    }
  }
  LDEBUG << "       ...return false";
  return false;
}

bool EventAnalyzer::is_specific_entity(LinguisticGraphVertex v,AnnotationData* annotationData,EntityType t) const
{
  EVENTANALYZERLOGINIT;
  std::set< AnnotationGraphVertex > matches = annotationData->matches(m_graphId,v,"annot");

  // OME 
  std::string entityName = Common::Misc::limastring2utf8stdstring(
        Common::MediaticData::MediaticData::single().getEntityName(t) );
  LDEBUG << "is_specific_entity(" << entityName << " at " << v << " ) ?";
    
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    // OME
    LDEBUG << "Looking at annotation graph vertex " << vx << " for " << entityName;
    
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      EntityType  e;
      e=annotationData->annotation(vx,Common::Misc::utf8stdstring2limastring("SpecificEntity")).pointerValue< SpecificEntityAnnotation>()->getType();
      if (e==t) {
        // OME
  LDEBUG << "is_specific_entity(" << entityName << " at " << v << " ) return true";
  return true;
      }

    }
  }
  // OME
  LDEBUG << "is_specific_entity(" << entityName << " at " << v << " ) return false";
  return false;
}

Common::MediaticData::EntityType EventAnalyzer::getEntityType(LinguisticGraphVertex v, Common::AnnotationGraphs::AnnotationData* annotationData,std::string graphId) const
{
  Common::MediaticData::EntityType e;
  std::set< AnnotationGraphVertex > matches = annotationData->matches(graphId,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      
      e=annotationData->annotation(vx,Common::Misc::utf8stdstring2limastring("SpecificEntity")).pointerValue< SpecificEntityAnnotation>()->getType();
      return e;
    }
  }
  return e;  
}

std::string EventAnalyzer::getDate(LinguisticGraphVertex v,AnnotationData* annotationData,EntityType t) const
{
  EVENTANALYZERLOGINIT;
  std::string normalizedForm="00-00-00";
  std::string year="00";
  std::string month="00";
  std::string day="00";
  
  std::set< AnnotationGraphVertex > matches = annotationData->matches(m_graphId,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    LDEBUG << "Looking at annotation graph vertex " << vx;
    
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      
      
      EntityType  e;
      e=annotationData->annotation(vx,Common::Misc::utf8stdstring2limastring("SpecificEntity")).pointerValue< SpecificEntityAnnotation>()->getType();
      if (e == t) 
      {
      EntityFeatures  features;
      features=annotationData->annotation(vx,Common::Misc::utf8stdstring2limastring("SpecificEntity")).pointerValue< SpecificEntityAnnotation>()->getFeatures();
      for (Automaton::EntityFeatures::const_iterator 
               featureItr=features.begin(),features_end=features.end();
               featureItr!=features_end; featureItr++)
          {
            LDEBUG << "Looking for feature=" << featureItr->getName() << ",  value=" << featureItr->getValueString();
             if (featureItr->getName().compare("date") == 0)
             return (featureItr->getValueString());
             if (featureItr->getName().compare("year") == 0)
               year=featureItr->getValueString();
             if (featureItr->getName().compare("month") == 0)
               month=featureItr->getValueString();
             if (featureItr->getName().compare("day") == 0)
               day=featureItr->getValueString();
          }
       }
    }
  }
  normalizedForm= year;
  normalizedForm.insert(normalizedForm.end(),'-');
  normalizedForm.append(month);
  normalizedForm.insert(normalizedForm.end(),'-');
  normalizedForm.append(day);
  LDEBUG << "Returned normalizedForm = " << normalizedForm;
  return (normalizedForm);
}

bool EventAnalyzer::is_specific_entity_in(LinguisticGraphVertex v,AnnotationData* annotationData,set<EntityType> eset) const
{
  EVENTANALYZERLOGINIT;
  // OME 
  LDEBUG << "is_specific_entity_in( at " << v << " ) ?";
  std::set< AnnotationGraphVertex > matches = annotationData->matches(m_graphId,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    // OME
    LDEBUG << "Looking at annotation graph vertex " << vx;
    
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      EntityType  e;
      e=annotationData->annotation(vx,Common::Misc::utf8stdstring2limastring("SpecificEntity")).pointerValue< SpecificEntityAnnotation>()->getType();
      if (eset.find(e)!=eset.end())
      {
        LDEBUG << "is_specific_entity_in( at " << v << " )  return true";
  return true;
      }
    }
  }
  // OME
  LDEBUG << "is_specific_entity_in( at " << v << " )  return false";
  return false;
}

} // closing namespace EventAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
