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
 * @file     Paragraph.cpp
 * @author   Faiza GARA
 * @date     October 2008
 * @version   $Id:
 * copyright Copyright (C) 2008 by CEA LIST
 * 
 ***********************************************************************/


#include "Paragraph.h"

#include "common/Data/readwritetools.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"



using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing
{

namespace EventAnalysis
{ 

//**********************************************************************
// constructors,destructor,copy assignment
//**********************************************************************
Paragraph::Paragraph(): m_sentences(),m_otherentities(),m_evententities(),m_dates(),m_length(0),m_position(0),m_id(0)
{
}



Paragraph::~Paragraph()
{
  m_otherentities.clear();
  m_evententities.second.clear();
  m_sentences.clear();
  m_dates.clear();
}

std::pair<Common::MediaticData::EntityType,std::deque<LinguisticGraphVertex> > Paragraph::extractEventEntitiesBeforeVertex(LinguisticGraphVertex v,LinguisticGraph* graph)
{
  EVENTANALYZERLOGINIT;
  LDEBUG << "Paragraph::extractEventEntitiesBeforeVertex m_evententities.first " <<  m_evententities.first;
  std::deque<LinguisticGraphVertex>  evententities;
  Token *token=get(vertex_token,*graph,v);
  
  bool end =m_evententities.second.empty(); 
  while (! end)
  {
    LinguisticGraphVertex v_event = m_evententities.second.front();
    Token *token_event=get(vertex_token,*graph,v_event);
    if(token_event->position() > token->position())
      end=true;
    else
    {
      evententities.push_back(v_event);
      m_evententities.second.pop_front();
      end =m_evententities.second.empty(); 
    }
  }
  return make_pair(m_evententities.first,evententities);
  
}

bool Paragraph::has_entity(Common::MediaticData::EntityType t) const
{
  bool res=false;
  
  if (m_otherentities.find(t)!=m_otherentities.end())
  {
    return true;
  }
  
  if (m_evententities.first==t && m_evententities.second.size()> 0) return true;
  
  return res;
}

std::map<Common::MediaticData::EntityType, std::deque<LinguisticGraphVertex> > Paragraph::extractEntitiesBeforeVertex(LinguisticGraphVertex v,LinguisticGraph* graph)
{
  std::map<Common::MediaticData::EntityType, std::deque<LinguisticGraphVertex> > entities;
  Token *token=get(vertex_token,*graph,v);
  for(std::map<Common::MediaticData::EntityType, std::deque<LinguisticGraphVertex> >::iterator iT =m_otherentities.begin();iT !=m_otherentities.end();iT++)
  {
    bool end =(*iT).second.empty(); 
    std::deque<LinguisticGraphVertex> d;
    while (! end)
    {
    
      LinguisticGraphVertex v_entity = (*iT).second.front();
      Token *token_entity=get(vertex_token,*graph,v_entity);
      if(token_entity->position() > token->position())
        end=true;
      else
      {
        d.push_back(v_entity);
        (*iT).second.pop_front();
        end =(*iT).second.empty(); 
      }
    }
    if(d.size()>0) entities[(*iT).first]=d;
  }
  
  return entities;
}



} // namespace 
} // namespace
} // namespace Lima
