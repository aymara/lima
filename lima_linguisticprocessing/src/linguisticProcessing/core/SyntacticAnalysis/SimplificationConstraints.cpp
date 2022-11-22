// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @brief       this file contains the implementations of several constraint
  *              functions for the detection of subsentences
  *
  * @file        SimplificationConstraints.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2005 by CEA
  * @date        Created on  Tue Mar, 15 2005
  *
  */

#include "SimplificationConstraints.h"
#include "DependencyGraph.h"
#include "SyntacticData.h"
#include "SyntagmaticMatrix.h"
#include "SimplificationData.h"

#include "linguisticProcessing/core/Automaton/constraintFunction.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/MediaticData/mediaticData.h"
#include "common/LimaCommon.h"

#include <string>

using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{
#define SASLOGINIT  LOGINIT("LP::SyntacticAnalysis::Simplify")

/// @todo deal with simplication rules

//**********************************************************************
// factories for constraint functions defined in this file
Automaton::ConstraintFunctionFactory<SubsentenceBounds>
  SubsentenceBoundsFactory(SubsentenceBoundsId);

Automaton::ConstraintFunctionFactory<Simplify>
  SimplifyFactory(SimplifyId);

Automaton::ConstraintFunctionFactory<ClearStoredSubsentences>
  ClearStoredSubsentencesFactory(ClearStoredSubsentencesId);


//**********************************************************************
SubsentenceBounds::SubsentenceBounds(MediaId language,
                                     const LimaString& complement):
  ConstraintWithRelationComplement(language,complement),
  m_language(language)
{
}

bool SubsentenceBounds::operator()(const Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph& graph,
                                   const LinguisticGraphVertex& v,
                                   AnalysisContent& analysis) const
{
  return operator()(graph,v,v,analysis);
}
  
bool SubsentenceBounds::operator()(const Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph& graph,
                                     const LinguisticGraphVertex& v1,
                                     const LinguisticGraphVertex& v2,
                                   AnalysisContent& analysis) const
{
  SASLOGINIT;
  LDEBUG << "testing SubsentenceBounds for " << v1 << " and " << v2 << " with relation: " /*<< (static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).
    getEntityNames("SyntacticSimplification")[m_relation])*/;
  
  auto simplificationData = std::dynamic_pointer_cast<SimplificationData>(analysis.getData("SimplificationData"));
  if (simplificationData==0)
  {
    SASLOGINIT;
    LERROR << "No simplificationData in SubsentenceBounds constraint";
    return false;
  }
  if (v1 == graph.firstVertex() || v1 == graph.lastVertex() ||
      v2 == graph.firstVertex() || v2 == graph.lastVertex() )
  {
  LDEBUG << "SubsentenceBounds: false";
    return false;
  }
  simplificationData->subSentBounds(boost::make_tuple(v1,v2, m_relation));
  LDEBUG << "SubsentenceBounds: true";
  return true;
}


Simplify::Simplify(MediaId language,
                   const LimaString& complement):
  ConstraintWithRelationComplement(language,complement),
  m_language(language)
{
}

/** @note Current version supposes a disambiguated graph, i.e. only one path from src to tgt */
bool Simplify::operator()(RecognizerMatch& /*unused*/,
                        AnalysisContent& analysis) const
{

  SASLOGINIT;
  LDEBUG << "Doing simplification";

  auto syntacticData = std::dynamic_pointer_cast<SyntacticData>(analysis.getData("SyntacticData"));
  auto simplificationData = std::dynamic_pointer_cast<SimplificationData>(analysis.getData("SimplificationData"));
  LinguisticGraph* graph = syntacticData->graph();
  if (simplificationData==0)
  {
    SASLOGINIT;
    LERROR << "No simplificationData in SubsentenceBounds constraint";
    return false;
  }
  std::list< boost::tuple< LinguisticGraphVertex, LinguisticGraphVertex, SyntacticRelationId > >::const_iterator simplificationsIt, simplificationsIt_end;
  simplificationsIt = simplificationData->subSentBounds().begin();
  simplificationsIt_end = simplificationData->subSentBounds().end();
  bool simplificationDone = false;
  for (; simplificationsIt != simplificationsIt_end; simplificationsIt++)
  {
    LinguisticGraphVertex first, last;
    SyntacticRelationId type;
    boost::tie(first, last, type) = (*simplificationsIt);
      
    LDEBUG << "Match "
/*      << (static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).
          getEntityNames("SyntacticSimplification")[type])*/
      /*<< " was: " << result*/ << " with bounds "
        << first << " / "
        << last;

    if (boost::in_degree(first, *graph)==0 || boost::out_degree(last, *graph)==0)
    {
      return false;
    }
    LDEBUG << "first ("<<first<<") has " << boost::in_degree(first, *graph) << " in edges";
    LinguisticGraphEdge inEdge = *(boost::in_edges(first, *graph).first);
    boost::remove_edge(inEdge, *graph);

    LDEBUG << "last ("<<last<<") has " << boost::out_degree(last, *graph) << " out edges";
    LinguisticGraphEdge outEdge = *(boost::out_edges(last, *graph).first);
    boost::remove_edge(outEdge, *graph);
  
    LDEBUG << "Old edges are "
    << inEdge.m_source << " -> " << inEdge.m_target << " / "
    << outEdge.m_source << " -> " << outEdge.m_target;
    LDEBUG << "New edge is "
      << source(inEdge,*graph) << " -> "
      << target(outEdge,*graph);
    std::pair<LinguisticGraphEdge, bool> addingResult = boost::add_edge(source(inEdge,*graph), target(outEdge,*graph), *graph);
    if (addingResult.second == false)
    {
      SASLOGINIT;
      LERROR << "Was not able to add a simplification edge.";
      simplificationData->clearBounds();
      return false;
    }
    
    simplificationData->addSimplification(inEdge, outEdge, type);
    simplificationDone = true;
  }
  simplificationData->clearBounds();
  return simplificationDone;
}

//----------------------------------------------------------------------
ClearStoredSubsentences::ClearStoredSubsentences(MediaId language,
                                                 const LimaString& complement):
  ConstraintWithRelationComplement(language,complement)
{
}

bool ClearStoredSubsentences::operator()(AnalysisContent& analysis) const
{
  
  SASLOGINIT;
  LDEBUG << "clearing stored subsentences";
  
  auto simplificationData = std::dynamic_pointer_cast<SimplificationData>(analysis.getData("SimplificationData"));
  
  simplificationData->clearBounds();
  return true;
}

} // end namespace SyntacticAnalysis
} // end namespace LinguisticProcessing
} // end namespace Lima
