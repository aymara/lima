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
 * @file     Paragraph.h
 * @author   Faiza GARA
 * @date     April 2008
 * copyright Copyright (C) 2007 by CEA LIST
 * version   $Id: 
 * 
 * @brief  
 * 
 * 
 ***********************************************************************/

#ifndef EVENTPARAGRAPH_H
#define EVENTPARAGRAPH_H

#include "EventAnalysisExport.h"
#include "linguisticProcessing/core/EventAnalysis/Paragraph.h"
#include "linguisticProcessing/core/EventAnalysis/Entity.h"

#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"

namespace Lima {
namespace LinguisticProcessing
{

namespace EventAnalysis
{ 


class LIMA_EVENTANALISYS_EXPORT EventParagraph 
{
public:
    EventParagraph();
    
    EventParagraph(Paragraph *p,Common::AnnotationGraphs::AnnotationData*, std::string graphId,LinguisticGraph* graph);
    
    EventParagraph(Paragraph *p, bool splitted,Common::AnnotationGraphs::AnnotationData*, std::string graphId,LinguisticGraph* graph);
    
    bool is_splitted();
    
    bool has_entity(Common::MediaticData::EntityType t) const;
    
    void addEventEntities(Paragraph *p,Common::AnnotationGraphs::AnnotationData*, std::string graphId,LinguisticGraph* graph);
    
    void addEntities(Paragraph *p,Common::AnnotationGraphs::AnnotationData*, std::string graphId,LinguisticGraph* graph);
    
    std::map<Common::MediaticData::EntityType,std::vector<Entity *> > getOtherEntities();
    
    std::pair<Common::MediaticData::EntityType,std::vector<Entity *> > getEventEntities();
        
    uint64_t getId() const;
    uint64_t getPosition() const;
    uint64_t getStart() const;
    uint64_t getEnd() const;
    uint64_t getLength() const;
    
    void setId(uint64_t id);
    void setPosition(uint64_t position);
    void setLength(uint64_t length);
    void clear();
    
    // direct insertion of a known entity
    void addEntity(const Common::MediaticData::EntityType& type, Entity* entity);
    void addEventEntity(const Common::MediaticData::EntityType& type, Entity* entity);
    
    virtual ~EventParagraph();
    
     //@{ binary input/output
    virtual void read(std::istream& file);
    virtual void write(std::ostream& file) const;
    
    std::string toString(std::string parentURI,uint64_t index, bool main) const;
  
  private:
    bool m_splitted;
    std::map<Common::MediaticData::EntityType,std::vector<Entity *> >m_otherentities;
    std::pair<Common::MediaticData::EntityType,std::vector<Entity *> > m_evententities;
    uint64_t m_length;
    uint64_t m_position;
    uint64_t m_id;
    
    Lima::LinguisticProcessing::Automaton::EntityFeatures getEntityFeatures(LinguisticGraphVertex, Common::AnnotationGraphs::AnnotationData*, std::string graphId);

};

inline bool EventParagraph::is_splitted(){ return (m_splitted == true);}

inline std::map<Common::MediaticData::EntityType,std::vector<Entity *> > EventParagraph::getOtherEntities(){return m_otherentities;}
    
inline std::pair<Common::MediaticData::EntityType,std::vector<Entity *> > EventParagraph::getEventEntities(){return m_evententities;}

inline uint64_t EventParagraph::getId() const {return m_id;}
inline uint64_t EventParagraph::getPosition() const {return m_position;}
inline uint64_t EventParagraph::getStart() const {return m_position;}
inline uint64_t EventParagraph::getEnd() const {return (m_position+m_length-1);}
inline uint64_t EventParagraph::getLength() const {return m_length;}

inline void EventParagraph::setId(uint64_t id){m_id=id;}
inline void EventParagraph::setPosition(uint64_t position){m_position=position;}
inline void EventParagraph::setLength(uint64_t length){m_length=length;}
} 
} 
}// namespace Lima
#endif 
