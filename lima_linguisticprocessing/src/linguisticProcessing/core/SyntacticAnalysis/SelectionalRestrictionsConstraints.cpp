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
* @brief       this file contains the definitions of several constraint
*              functions for the disambiguation of dependencies based on
*              selectional restrictions
*
* @file        SelectionalRestrictionsConstraints.h
* @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

*              Copyright (c) 2010 by CEA
* @date        Created on  Sun May, 16 2010
* @version     $Id:  $
*
*
*/

#include "SelectionalRestrictionsConstraints.h"
#include "SelectionalPreferences.h"
#include "DependencyGraph.h"
#include "SyntacticData.h"
#include "SyntagmaticMatrix.h"

#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <string>
#include <map>
#include <queue>
#include <boost/regex.hpp>

using namespace boost;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::PropertyCode;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

//**********************************************************************
// factories for constraint functions defined in this file
Automaton::ConstraintFunctionFactory<StoreForDisambiguation>
StoreForDisambiguationFactory(StoreForDisambiguationId);

Automaton::ConstraintFunctionFactory<DisambiguateWith>
DisambiguateWithFactory(DisambiguateWithId);

 
//**********************************************************************
StoreForDisambiguation::StoreForDisambiguation(
  MediaId language,
  const LimaString& complement):
    ConstraintWithRelationComplement(language,complement)
{}

bool StoreForDisambiguation::operator()(
  const LinguisticAnalysisStructure::AnalysisGraph& graph,
  const LinguisticGraphVertex& v1,
  const LinguisticGraphVertex& v2,
  AnalysisContent& analysis ) const
{
/*
  Critical Function : comment logging messages
*/
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  if (v1 == graph.firstVertex() || v1 == graph.lastVertex() ||
    v2 == graph.firstVertex() || v2 == graph.lastVertex() )
  {
    //     LDEBUG << "SecondUngovernedBy: false";
    return false;
  }
#ifdef DEBUG_LP
  SAPLOGINIT;
  LDEBUG << "StoreForDisambiguation " << v1 << ", " << v2 << ", " << m_relation;
#endif
  syntacticData->storeRelationForSelectionalConstraint(v1, v2, m_relation);
  return true;
}

DisambiguateWith::DisambiguateWith(MediaId language,
                       const LimaString& complement):
    ConstraintWithRelationComplement(language,complement),
    m_language(language)
{
  //  ConstraintFunction::addFunction("GovernorOf",this);
  try {
    AbstractResource* res=LinguisticResources::changeable().
    getResource(language,"selectionalPreferences");
    if (res==0)
    {
      SELOGINIT;
      LWARN << "no resource 'selectionalPreferences'";
    }
    else {
      m_preferences=static_cast<SelectionalPreferences*>(res);
    }
  }
  catch (std::exception& e) {
    // resources are not loaded when compiling the rules
    SELOGINIT;
    LWARN << "Exception caught: " << e.what();
  }
  
}

bool DisambiguateWith::operator()(const AnalysisGraph& graph,
                            const LinguisticGraphVertex& v1,
                            const LinguisticGraphVertex& v2,
                            AnalysisContent& analysis) const
{
/*
  Critical function : comment logging messages
*/
#ifdef DEBUG_LP
  SAPLOGINIT;
  LDEBUG << "DisambiguateWith " << v1 << ", " << v2;
#endif
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  SyntacticData::Relation oldRelation = syntacticData->relationStoredForSelectionalConstraint();
  FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(m_language);
  
  
  if (v1 == graph.firstVertex() || v1 == graph.lastVertex() ||
    v2 == graph.firstVertex() || v2 == graph.lastVertex() )
  {
    //     LDEBUG << "SecondUngovernedBy: false";
    return false;
  }
  // collect all data necessary
  // dummy implementation:
  //   get old target (noun), get old target COD target (verb), get current node
  //   get current det source of detsub if any
  AnalysisGraph* posgraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph==0)
  {
    SAPLOGINIT;
    LERROR << "no graph 'PosGraph' available !";
    return false;
  }
  LinguisticGraph* lingGraph = const_cast<LinguisticGraph*>(posgraph->getGraph());
  //   LDEBUG << "There is " << out_degree(v2, *lingGraph) << " edges out of " << v2;
//   VertexTokenPropertyMap tokenMap = get(vertex_token, *lingGraph);
  VertexDataPropertyMap dataMap = get(vertex_data, *lingGraph);
//   Token* v1Token = tokenMap[v1];
  MorphoSyntacticData* v1Data = dataMap[v1];
//   Token* v2Token = tokenMap[v2];
  MorphoSyntacticData* v2Data = dataMap[v2];
  std::string rel = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(m_relation);
//   Token* ov1Token = tokenMap[oldRelation.get<0>()];
  MorphoSyntacticData* ov1Data = dataMap[oldRelation.get<0>()];
//   Token* ov2Token = tokenMap[oldRelation.get<1>()];
  MorphoSyntacticData* ov2Data = dataMap[oldRelation.get<1>()];
  std::string orel = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(oldRelation.get<2>());
  
  // compute the preferred attachment
  // (if no clear preference, choose the closest left attachment)
  // dummy implementation
  //   if verb = manger and old target = food and det = "" and current is food
  //   then choose old relation
  //   else (at least if current is location) choose new one
  double preference = 0;
  const PropertyAccessor& macroAccessor = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MACRO");
  
#ifdef DEBUG_LP
  LDEBUG << "DisambiguateWith " << Common::Misc::limastring2utf8stdstring(sp[*(v2Data->allLemma().begin())])
      << ", " << Common::Misc::limastring2utf8stdstring(sp[*(ov2Data->allLemma().begin())])
      << ", " << Common::Misc::limastring2utf8stdstring(sp[*(v1Data->allLemma().begin())]);
#endif
  std::string oldRelationName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(oldRelation.get<2>());
  double oldProba = m_preferences->dependencyProbability(Common::Misc::limastring2utf8stdstring(sp[*(ov2Data->allLemma().begin())]),
                                                         *(ov2Data->allValues(macroAccessor).begin()),
                                                         oldRelationName,
                                                         Common::Misc::limastring2utf8stdstring(sp[*(ov1Data->allLemma().begin())]),
                                                         *(ov1Data->allValues(macroAccessor).begin()));
  double newProba = m_preferences->dependencyProbability(Common::Misc::limastring2utf8stdstring(sp[*(v2Data->allLemma().begin())]),
                                                         *(v2Data->allValues(macroAccessor).begin()),
                                                        Common::Misc::limastring2utf8stdstring(m_complement),
                                                        Common::Misc::limastring2utf8stdstring(sp[*(v1Data->allLemma().begin())]),
                                                         *(v1Data->allValues(macroAccessor).begin()));

/*  if ( !( (Common::Misc::limastring2utf8stdstring(sp[*(v2Data->allLemma().begin())]) == "manger")
    && (Common::Misc::limastring2utf8stdstring(sp[*(ov2Data->allLemma().begin())]) == "bonbon")
    && (Common::Misc::limastring2utf8stdstring(sp[*(v1Data->allLemma().begin())]) == "fraise")
    ) )*/
#ifdef DEBUG_LP
  LDEBUG << "Old proba=" << oldProba << "; new proba=" << newProba;
#endif
  preference = newProba - oldProba;
  
  // if old one: don't do anything
  // else if new one, remove the old dependency, return true
  if (preference > 0) /// @TODO implement the test
  {
    return syntacticData->removeDependency(oldRelation.get<0>(), oldRelation.get<1>(), oldRelation.get<2>());
  }
  
  return false;
}



} // end namespace
} // end namespace
} // end namespace
