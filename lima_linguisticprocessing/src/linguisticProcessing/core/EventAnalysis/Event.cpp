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
 * @file     Event.cpp
 * @author   Faiza GARA
 * @date     October 2008
 * @version   $Id:
 * copyright Copyright (C) 2008 by CEA LIST
 * 
 ***********************************************************************/


#include "Event.h"

#include "common/Data/readwritetools.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"


using namespace std;
using namespace Lima::LinguisticProcessing::SpecificEntities;

namespace Lima {
namespace LinguisticProcessing
{

namespace EventAnalysis
{ 

//**********************************************************************
// constructors,destructor,copy assignment
//**********************************************************************
Event::Event():
    std::vector< EventParagraph* >(), m_entities_weight(0), m_main(false)
{
  EVENTANALYZERLOGINIT;
  LDEBUG << "Nouvel évènement" << LENDL;
}

Event::~Event()
{
  for(std::vector< EventParagraph* >::const_iterator iT=begin(); iT !=end(); iT++)
  {
    if ((*iT) !=0){
      (*iT)->clear();
    }
  }
  clear();
  
}

void Event::compute_main_entities()
{
//   EVENTANALYZERLOGINIT;
  for(std::vector<EventParagraph*>::const_iterator iT=begin();iT!=end();iT++)
  {
    std::pair<Common::MediaticData::EntityType,std::vector<Entity *> > event=(*iT)->getEventEntities();
    std::map<Common::MediaticData::EntityType,std::vector<Entity *> > otherentities=(*iT)->getOtherEntities();
    std::vector<Entity *> v_entity = event.second;
    if(v_entity.size() > 0)
    {
      Entity *entity = v_entity[0];
      entity->setMain(true);
    }
    for ( std::map<Common::MediaticData::EntityType,std::vector<Entity *> >::const_iterator iT=otherentities.begin();iT!=otherentities.end();iT++)
    {
      v_entity = (*iT).second;
      if(v_entity.size() > 0)
      {
        Entity *entity = v_entity[0];
        entity->setMain(true);
      }
    }
  }
}

void Event::compute_entities_weight(std::map<Common::MediaticData::EntityType,unsigned short> map_weights,Common::AnnotationGraphs::AnnotationData* annotation, std::string graphId)
{
  LIMA_UNUSED(annotation);
  LIMA_UNUSED(graphId);
  EVENTANALYZERLOGINIT;
  
  
//   unsigned short number_computed_entities=0;
  for(std::map<Common::MediaticData::EntityType,unsigned short>::iterator iT=map_weights.begin();iT!=map_weights.end();iT++)
  {
    if (has_entity((*iT).first)) m_entities_weight=m_entities_weight+(*iT).second;
  }
  
  LDEBUG << "computed entities_weight of event = " << m_entities_weight << LENDL;
}

bool Event::has_entity(Common::MediaticData::EntityType t) const
{
  bool res=false;
  
  if (t==m_date.first) return true;
  
  for(std::vector<EventParagraph*>::const_iterator iT=begin();iT!=end() && !res;iT++)
  {
    res=(*iT)->has_entity(t);
  }
  return res;
}
    
Common::MediaticData::EntityType Event::getEntityType(LinguisticGraphVertex v, Common::AnnotationGraphs::AnnotationData* annotationData,std::string graphId) const
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
void Event::addParagraph(Paragraph *p,bool firstpos,bool split,Common::AnnotationGraphs::AnnotationData* annotationData, std::string graphId,LinguisticGraph* graph)
{
  EVENTANALYZERLOGINIT;
  LDEBUG << "Ajout de Paragraph" << LENDL;
  
  EventParagraph *ep =new EventParagraph(p,split,annotationData,graphId,graph);
  if (this->size()==0) push_back(ep);
  else
  {
    EventParagraph *lastp =this->back();
    if ((lastp->getId()+1) == p->getId() && firstpos && (lastp->is_splitted() == false))
    {
      LDEBUG << "Les deux paragraphes sont contigus" << LENDL;
      // ce sont deux paragraphes contigüs
      // ajouter tous les élément de p dans lastp et changer l'indice de lastp
      lastp->setId(p->getId());
      lastp->setLength(lastp->getLength()+p->getLength());
      //lastp->addSentences(p);
      lastp->addEventEntities(p,annotationData,graphId,graph);
      lastp->addEntities(p,annotationData,graphId,graph);
      this->pop_back();
      this->push_back(lastp);
      delete(ep);
    }
    else
    {
      // ce sont deux paragraphes non contigüs
      this->push_back(ep);
      LDEBUG << "Les deux paragraphes appartiennent à des sections différentes" << LENDL;
    }
    delete(p);
  }
    
}




/**********************************************************************
 * Returns the next annotation. It will be no more available
 */


//**********************************************************************
// read/write in binary format
//**********************************************************************
void Event::read(std::istream& file) {
  EVENTANALYZERLOGINIT;
  m_main=Common::Misc::readCodedInt(file);
  uint64_t size=Common::Misc::readCodedInt(file);
  LDEBUG << " Read Main = " << m_main << LENDL;
  LDEBUG << " Read size of fragments " << size << LENDL;
  for (uint64_t i(0); i<size; i++) {
    EventParagraph* p=new EventParagraph();
    p->read(file);
    push_back(p);
  }
}

void Event::write(std::ostream& file) const {
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "Event::write().."<< LENDL; 
  //EVENTANALYZERLOGINIT;
  Common::Misc::writeCodedInt(file,m_main);
  Common::Misc::writeCodedInt(file,size());
  LDEBUG << " Write size of fragments " << size() << LENDL; 
  for (std::vector< EventParagraph* >::const_iterator iT=begin(); iT!=end(); iT++) {
     (*iT)->write(file);
   }
}

std::string  Event::toString(std::string parentURI, uint64_t index) const 
{
  std::string out;
  
  for (std::vector< EventParagraph* >::const_iterator iT=begin(); iT!=end(); iT++) {
    out.append((*iT)->toString(parentURI, index, m_main));
  }
  
  return out;
} 

std::ostream& operator << (std::ostream& os, const Event& text) {
  for (Event::const_iterator i(text.begin());
       i != text.end(); i++) {
    //os << (*i)->getOutputUTF8String() << std::endl;
  }
  return os;
}



} // namespace 
} // namespace
} // namespace Lima
