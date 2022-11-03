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

#include "CoordinationConstraints.h"
#include "DependencyGraph.h"
#include "SyntacticData.h"
#include "SyntagmaticMatrix.h"
#include "SimplificationData.h"

#include "linguisticProcessing/core/Automaton/constraintFunction.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/MediaticData/mediaticData.h"

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
#define SACLOGINIT LOGINIT("LP::SyntacticAnalysis::Coordination")

/// @todo deal with simplication rules

//**********************************************************************
// factories for constraint functions defined in this file
Automaton::ConstraintFunctionFactory<DefineString>
  DefineStringFactory(DefineStringId);

Automaton::ConstraintFunctionFactory<SameString>
  SameStringFactory(SameStringId);

Automaton::ConstraintFunctionFactory<DefineModel>
  DefineModelFactory(DefineModelId);

Automaton::ConstraintFunctionFactory<SetInstance>
  SetInstanceFactory(SetInstanceId);
  
  
//**********************************************************************
DefineString::DefineString(MediaId language,
                                     const LimaString& complement):
                                     ConstraintFunction(language,complement),
  m_language(language)
{
}

bool DefineString::operator()(const Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph& graph,
                                     const LinguisticGraphVertex& v1,
                                     const LinguisticGraphVertex& v2,
                                   AnalysisContent& analysis) const
{
  SACLOGINIT;
  LDEBUG << "testing DefineString for " << v1 << " and " << v2 << " with relation: " /*<< (static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).
    getEntityNames("SyntacticSimplification")[m_relation])*/;
  std::string complement = Common::Misc::limastring2utf8stdstring(m_complement);
  auto simplificationData = std::dynamic_pointer_cast<SimplificationData>(analysis.getData("SimplificationData"));
  if (simplificationData==0)
  {
    SACLOGINIT;
    LERROR << "No simplificationData in DefineString constraint";
    return false;
  }
  if (simplificationData->coordStringsDefs().find(complement) != simplificationData->coordStringsDefs().end())
  {
    SACLOGINIT;
    LERROR << "simplificationData already contains a " << complement << " coord string definition";
    return false;
  }
  if (v1 == graph.firstVertex() || v1 == graph.lastVertex() ||
      v2 == graph.firstVertex() || v2 == graph.lastVertex() )
  {
  LDEBUG << "DefineString: false";
    return false;
  }
  simplificationData->coordStringsDefs().insert(std::make_pair(complement,std::vector<LinguisticGraphVertex>()));
  std::vector<LinguisticGraphVertex>& stringDef = (*simplificationData->coordStringsDefs().find(complement)).second;
  LDEBUG << "Add " << v1 << " to string " << complement;
  stringDef.push_back(v1);
  
  LinguisticGraphVertex next = v1;
  while (next != v2)
  {
    next = boost::target(*(boost::out_edges(next, *graph.getGraph()).first),*graph.getGraph());
    LDEBUG << "Add " << next << " to string " << complement;
    stringDef.push_back(next);
  }
  
  LDEBUG << "DefineString: true";
  return true;
}


SameString::SameString(MediaId language,
                   const LimaString& complement):
                   ConstraintFunction(language,complement),
  m_language(language)
{
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
}

bool SameString::operator()(const Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph& /*agraph*/,
                              const LinguisticGraphVertex& v1,
                              const LinguisticGraphVertex& v2,
                              AnalysisContent& analysis) const
{

  SACLOGINIT;
  LDEBUG << "Testing SameString on " << v1 << " and " << v2;

  std::string complement = Common::Misc::limastring2utf8stdstring(m_complement);
  auto syntacticData = std::dynamic_pointer_cast<SyntacticData>(analysis.getData("SyntacticData"));
  auto simplificationData = std::dynamic_pointer_cast<SimplificationData>(analysis.getData("SimplificationData"));
  LinguisticGraph* graph = syntacticData->graph();
  if (simplificationData==0)
  {
    SACLOGINIT;
    LERROR << "No simplificationData in DefineString constraint";
    return false;
  }
  std::vector<LinguisticGraphVertex> stringDef;
  if (simplificationData->coordStringsDefs().find(complement) == simplificationData->coordStringsDefs().end())
  {
    SACLOGINIT;
    LWARN << "No such coord string defined: " << complement;
//     return false;
  }
  else
  {
    stringDef = (*simplificationData->coordStringsDefs().find(complement)).second;
  }

  if (simplificationData->coordStrings().find(complement) == simplificationData->coordStrings().end())
  {
    simplificationData->coordStrings().insert(std::make_pair(complement, std::vector< std::vector < LinguisticGraphVertex > >()));
  }

  std::vector<LinguisticGraphVertex> newString;
  LDEBUG << "Add " << v1 << " to new SameString " << complement;
  newString.push_back(v1);
  LinguisticGraphVertex next = v1;
  while (next != v2)
  {
    next = boost::target(*(boost::out_edges(next, *graph).first),*graph);
    LDEBUG << "Add " << next << " to new string " << complement;
    newString.push_back(next);
  }
  if (!stringDef.empty() && newString.size() != stringDef.size())
  {
    LDEBUG << "New string and model for " << complement << " have different sizes";
    
    return false;
  }
  if (stringDef.empty() && (*simplificationData->coordStrings().find(complement)).second.empty() )
  {
    LDEBUG << "No definition and no same string yet : using myself";
    stringDef = newString;
  }
  else if (stringDef.empty())
  {
    LDEBUG << "No definition yet : using first found same string";
    stringDef = *(*simplificationData->coordStrings().find(complement)).second.begin();
  }
  // testing equality of the new string and its definition
  std::vector<LinguisticGraphVertex>::const_iterator dit  = stringDef.begin();
  std::vector<LinguisticGraphVertex>::const_iterator nit  = newString.begin();

  for (; dit != stringDef.end(); dit++, nit++)
  {

    MorphoSyntacticData* msdDef=get(vertex_data,*graph,*dit);
    std::set<LinguisticCode> microFilters = msdDef->allValues(*m_microAccessor);
    std::list<LinguisticCode> lmicroFilters;
    std::copy(microFilters.begin(),microFilters.end(),std::insert_iterator< std::list<LinguisticCode> >(lmicroFilters,lmicroFilters.end()));
    MorphoSyntacticData* msdNew=get(vertex_data,*graph,*nit);
    if (!(msdNew!=0 && msdNew->hasUniqueMicro(*m_microAccessor,lmicroFilters)))
    {
      LDEBUG << "new string vertex " << *nit << " and model string vertex " << *dit << " have different micro categories";
      return false;
    }
  }
  LDEBUG << "Add string " << v1 << "/" << v2 << " to same strings of " << complement;
  simplificationData->coordStrings()[complement].push_back(newString);
  return true;
}

//----------------------------------------------------------------------
DefineModel::DefineModel(MediaId language,
                                                 const LimaString& complement):
                                                 ConstraintFunction(language,complement)
{
}

bool DefineModel::operator()(const Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
                            const LinguisticGraphVertex& /*v1*/,
                            const LinguisticGraphVertex& /*v2*/,
                            AnalysisContent& /*analysis*/) const
{
  
  SACLOGINIT;
  LDEBUG << "clearing stored subsentences";
  
  //SimplificationData* simplificationData =
  //  static_cast<SimplificationData*>(analysis.getData("SimplificationData"));
  
  return true;
}

//----------------------------------------------------------------------
SetInstance::SetInstance(MediaId language,
const LimaString& complement):
ConstraintFunction(language,complement)
{
}

bool SetInstance::operator()(const Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
                             const LinguisticGraphVertex& /*v1*/,
                             const LinguisticGraphVertex& /*v2*/,
                             AnalysisContent& /*analysis*/) const
{
  SACLOGINIT;
  LDEBUG << "clearing stored subsentences";
  
  //SimplificationData* simplificationData =
  //static_cast<SimplificationData*>(analysis.getData("SimplificationData"));

  return true;
}

} // end namespace SyntacticAnalysis
} // end namespace LinguisticProcessing
} // end namespace Lima
