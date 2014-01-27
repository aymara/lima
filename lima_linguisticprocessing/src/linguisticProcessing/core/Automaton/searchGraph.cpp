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
 * @file       searchGraph.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue May 24 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "searchGraph.h"

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

//**********************************************************************
// functions to walk the graph forward or backward
//**********************************************************************
ForwardSearch::ForwardSearch():m_current() {}
BackwardSearch::BackwardSearch():m_current() {}

SearchGraph* 
ForwardSearch::createNew() {
  return new ForwardSearch();
}
SearchGraph* 
BackwardSearch::createNew() {
  return new BackwardSearch();
}
LinguisticGraphVertex ForwardSearch::
endOfGraph(const AnalysisGraph& graph) {
  return graph.lastVertex();
}
LinguisticGraphVertex BackwardSearch::
endOfGraph(const AnalysisGraph& graph) {
  return graph.firstVertex();
}

void ForwardSearch::
findNextVertices(const LinguisticGraph* graph,
                 const LinguisticGraphVertex& current) 
{
  m_current.push_back(make_pair(current,out_edges(current,*graph)));
}

bool ForwardSearch::
getNextVertex(const LinguisticGraph* graph,
              LinguisticGraphVertex& next) 
{
  if (m_current.back().second.first == 
      m_current.back().second.second) {
    return false;
  }
  else {
    next=target(*(m_current.back().second.first),*graph);
    m_current.back().second.first++;
    return true;
  }
}

void BackwardSearch::
findNextVertices(const LinguisticGraph* graph,
                 const LinguisticGraphVertex& current) 
{
  m_current.push_back(make_pair(current,in_edges(current,*graph)));
}

bool BackwardSearch::
getNextVertex(const LinguisticGraph* graph,
              LinguisticGraphVertex& next) 
{
  if (m_current.back().second.first == 
      m_current.back().second.second) {
    return false;
  }
  else {
    next=source(*(m_current.back().second.first),*graph);
    m_current.back().second.first++;
    return true;
  }
}


} // end namespace
} // end namespace
} // end namespace
