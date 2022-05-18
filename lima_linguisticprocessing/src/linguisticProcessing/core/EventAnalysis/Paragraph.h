// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

#ifndef PARAGRAPH_H
#define PARAGRAPH_H


#include "EventAnalysisExport.h"
#include <deque>
#include <string>
#include <iostream>

#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

namespace Lima {
namespace LinguisticProcessing
{

namespace EventAnalysis
{ 

/**
 * This class represents a list of elements, that are pointers on
 * polymmorphic annotations that can be datatype property annotations
 * or object property annotations
 * 
 **/
class LIMA_EVENTANALISYS_EXPORT Paragraph 
{
public:
    Paragraph();
    ~Paragraph();
  
  void addSentence(LinguisticGraphVertex);
  void addEntity(Common::MediaticData::EntityType,LinguisticGraphVertex);  
  void addEventEntities(Paragraph *p);  
  void addEntities(Paragraph *p);  
  void addEventEntities(std::pair<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> >);  
  void addEntities(std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> >);  
  void addSentences(Paragraph *p);
  void addEventEntity(Common::MediaticData::EntityType,LinguisticGraphVertex);  
  void addDate(std::string,LinguisticGraphVertex);
  void setLength(uint64_t);
  void setPosition(uint64_t);
  void setId(uint64_t);
  bool toFilter();
  uint64_t getDatesSize();
  uint64_t getSentencesSize();
  std::pair<std::string,LinguisticGraphVertex> getDate();
  std::pair<std::string,LinguisticGraphVertex> extractDate();
  
  LinguisticGraphVertex extractSentence();
  uint64_t getPosition();
  uint64_t getLength();
  uint64_t getId();
  
  bool has_entity(Common::MediaticData::EntityType t) const;
  std::pair<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > extractEventEntitiesBeforeVertex(LinguisticGraphVertex v,LinguisticGraph* graph);
  
  std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > extractEntitiesBeforeVertex(LinguisticGraphVertex v,LinguisticGraph* graph);
  
  std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > getOtherEntities() const;
  
  std::pair<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > getEventEntities() const;
  
private:
  std::deque<LinguisticGraphVertex> m_sentences;
  std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> >m_otherentities;
  std::pair<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > m_evententities;
  std::deque<std::pair<std::string, LinguisticGraphVertex> > m_dates;
  uint64_t m_length;
  uint64_t m_position;
  uint64_t m_id;
  
  std::deque<LinguisticGraphVertex> getSentences();
  std::deque<std::pair<std::string, LinguisticGraphVertex> >getDates();
  
  std::pair<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > extractEventEntitiesBeforeVertex(LinguisticGraphVertex);
  
  std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > extractEntitiesBeforeVertex(LinguisticGraphVertex);
};

inline std::pair<std::string,LinguisticGraphVertex> Paragraph::getDate()
{
  return m_dates.front();
}

inline std::pair<std::string,LinguisticGraphVertex> Paragraph::extractDate()
{
  std::pair<std::string,LinguisticGraphVertex> date=m_dates.front();
  m_dates.pop_front();
  return date;
}

inline LinguisticGraphVertex Paragraph::extractSentence()
{
  LinguisticGraphVertex s=m_sentences.front();
  m_sentences.pop_front();
  return s;
}

inline bool Paragraph::toFilter()
{
  return (m_evententities.second.size()==0);
}

inline uint64_t  Paragraph::getDatesSize()
{
  return (m_dates.size());
}

inline uint64_t  Paragraph::getSentencesSize()
{
  return (m_sentences.size());
}



inline uint64_t Paragraph::getPosition()
{
  return m_position;
}

inline uint64_t Paragraph::getId()
{
  return m_id;
}

inline uint64_t Paragraph::getLength()
{
  return m_length;
}

inline std::pair<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > Paragraph::getEventEntities() const
{
  return m_evententities;
}

inline std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > Paragraph::getOtherEntities() const
{
  return m_otherentities;
}

inline std::deque<LinguisticGraphVertex> Paragraph::getSentences()
{
  return m_sentences;
}


inline void Paragraph::setLength( uint64_t length ) {
  m_length=length;
}

inline void Paragraph::setId( uint64_t id ) {
  m_id=id;
}

inline void Paragraph::addDate(std::string date, LinguisticGraphVertex v) {
  m_dates.push_back(make_pair(date,v));
}

inline void Paragraph::setPosition( uint64_t p ) {
  m_position=p;
}


inline void Paragraph::addEntity(Common::MediaticData::EntityType t,LinguisticGraphVertex e) {
  if (m_otherentities.find(t) == m_otherentities.end())
  {
    std::deque<LinguisticGraphVertex> d;
    d.push_back(e);
    m_otherentities.insert(make_pair(t,d));
  }
  else
  {
    m_otherentities[t].push_back(e);
  }
  
}

inline void Paragraph::addSentence(LinguisticGraphVertex s) {
  m_sentences.push_back(s);
}


inline void Paragraph::addEventEntity(Common::MediaticData::EntityType t, LinguisticGraphVertex e) {
  if (m_evententities.first == t)
    m_evententities.second.push_back(e);
  else
  {
    m_evententities.first=t;
    std::deque<LinguisticGraphVertex> d;
    d.push_back(e);
    m_evententities.second=d;
  }
}

inline void Paragraph::addEntities(Paragraph *p) {
  std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > e = p->getOtherEntities();
  for(std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> >::const_iterator iT=e.begin();iT!=e.end();iT++)
  {
    m_otherentities[(*iT).first].insert(m_otherentities[(*iT).first].end(),(*iT).second.begin(),(*iT).second.end());
  }
  
}

inline void Paragraph::addEntities(std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > e) {
  for(std::map<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> >::const_iterator iT=e.begin();iT!=e.end();iT++)
  {
    m_otherentities[(*iT).first].insert(m_otherentities[(*iT).first].end(),(*iT).second.begin(),(*iT).second.end());
  }
}

inline void Paragraph::addSentences(Paragraph *p) {
  std::deque<LinguisticGraphVertex> s =p->getSentences();
  m_sentences.insert(m_sentences.end(),s.begin(),s.end());
}


inline void Paragraph::addEventEntities(Paragraph *p) {
  std::deque<LinguisticGraphVertex> e =p->getEventEntities().second;
  m_evententities.first=p->getEventEntities().first;
  m_evententities.second.insert(m_evententities.second.begin(),e.begin(),e.end());
}

inline void Paragraph::addEventEntities(std::pair<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > e) {
  m_evententities.first=e.first;
  m_evententities.second.insert(m_evententities.second.begin(),e.second.begin(),e.second.end());
}

} 
} 
}// namespace Lima
#endif 
