// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       searchGraph.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue May 24 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2019 by CEA LIST
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
ForwardSearch::createNew() const {
  return new ForwardSearch();
}
SearchGraph*
BackwardSearch::createNew() const {
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

#ifdef DEBUG_LP
LIMA_AUTOMATON_EXPORT std::ostream& output(std::ostream& os, const BackwardSearch::Vertex2EdgePair& x, const LinguisticGraph* graph) {
  LIMA_UNUSED(graph);
  os << x.first << ":(" /*<< *(x.second.first) << "-" << *(x.second.second)*/ << ") ";
  return os;
}

LIMA_AUTOMATON_EXPORT std::ostream& output(std::ostream& os, const BackwardSearch& x, const LinguisticGraph* graph) {
  os << "bs{ ";
  for (auto it = x.m_current.begin(); it != x.m_current.end(); it++)
    output(os, *it, graph);;
  os << " }";
  return os;
}

LIMA_AUTOMATON_EXPORT std::ostream& output(std::ostream& os, const ForwardSearch::Vertex2EdgePair& x, const LinguisticGraph* graph) {
  os << x.first << ":(" << target(*(x.second.first),*graph) << "-" << target(*(x.second.second),*graph) << ") ";
  return os;
}

LIMA_AUTOMATON_EXPORT std::ostream& output(std::ostream& os, const ForwardSearch& x, const LinguisticGraph* graph) {
  os << "fs{ ";
  for (auto it = x.m_current.begin(); it != x.m_current.end(); it++)
    output(os, *it, graph);
  os << " }";
  return os;
}

LIMA_AUTOMATON_EXPORT std::ostream& output(std::ostream& os, const SearchGraph *x, const LinguisticGraph* graph) {
  const ForwardSearch *pForward = dynamic_cast<const ForwardSearch*>(x);
  if (pForward != NULL) {
    output(os, *pForward, graph);
    return os;
  }

  const BackwardSearch *pBackward = dynamic_cast<const BackwardSearch*>(x);
  if (pBackward != NULL)
    output(os, *pBackward, graph);
  else
  {
    AULOGINIT;
    LIMA_EXCEPTION("Unexpected type of LinguisticGraph");
  }
  return os;
}
#endif

} // end namespace
} // end namespace
} // end namespace
