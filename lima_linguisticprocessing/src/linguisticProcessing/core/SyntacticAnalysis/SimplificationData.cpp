// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief       Data used for the simplification of sentences allowing easier heterosyntagmatic analysis
  *
  * @file        SimplificationData.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2005 by CEA
  * @date        Created on Mar, 24 2005
  * @version     $Id$
  *
  */

#include "SimplificationData.h"

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

/** @brief 
  * @param graph @b IN/OUT <I>LinguisticGraph*</I> the graph containing
  *        morphological data
  */
SimplificationData::SimplificationData(LinguisticAnalysisStructure::AnalysisGraph* ag) :
    m_simplificationDone(false) 
{
  LIMA_UNUSED(ag)
}

SimplificationData::~SimplificationData() 
{
}

void SimplificationData::addSimplification(
    LinguisticGraphEdge first, 
    LinguisticGraphEdge last, 
                                            Common::MediaticData::SyntacticRelationId type)
{
  if (m_simplifications.find(m_sentence) == m_simplifications.end())
  {
    m_simplifications.insert(std::make_pair(m_sentence, std::list< boost::tuple< LinguisticGraphEdge, LinguisticGraphEdge, Common::MediaticData::SyntacticRelationId > >()));
  }
  m_simplifications[m_sentence].push_back(boost::make_tuple(first, last, type));
  m_simplificationDone = true;
}
  
std::list< boost::tuple< LinguisticGraphEdge, LinguisticGraphEdge, Common::MediaticData::SyntacticRelationId > >& SimplificationData::simplifications(LinguisticGraphVertex sent) 
{
  if (m_simplifications.find(sent) == m_simplifications.end())
  {
    m_simplifications.insert(std::make_pair(sent, std::list< boost::tuple< LinguisticGraphEdge, LinguisticGraphEdge, Common::MediaticData::SyntacticRelationId > >()));
  }
  return m_simplifications[sent];
}

void SimplificationData::subSentBounds(const boost::tuple<LinguisticGraphVertex, LinguisticGraphVertex, Common::MediaticData::SyntacticRelationId >& bounds) 
{
  std::list< boost::tuple< LinguisticGraphVertex, LinguisticGraphVertex, Common::MediaticData::SyntacticRelationId > >::iterator it, it_end;
  it = m_subSentBounds.begin(); it_end = m_subSentBounds.end();
  for (; it != it_end ; it++)
  {
    if ( ( (*it).get<0>() == bounds.get<0>()
           || (*it).get<1>() == bounds.get<0>()
           || (*it).get<0>() == bounds.get<1>()
           || (*it).get<1>() == bounds.get<1>())
         && ((*it).get<2>() == bounds.get<2>()) )
    {
      m_subSentBounds.erase(it);
      break;
    }
  }
  m_subSentBounds.push_back(bounds);
}

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
