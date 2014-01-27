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
    getEntityNames("SyntacticSimplification")[m_relation])*/ << LENDL;
  std::string complement = Common::Misc::limastring2utf8stdstring(m_complement);
  SimplificationData* simplificationData =
      static_cast<SimplificationData*>(analysis.getData("SimplificationData"));
  if (simplificationData==0)
  {
    SACLOGINIT;
    LERROR << "No simplificationData in DefineString constraint" << LENDL;
    return false;
  }
  if (simplificationData->coordStringsDefs().find(complement) != simplificationData->coordStringsDefs().end())
  {
    SACLOGINIT;
    LERROR << "simplificationData already contains a " << complement << " coord string definition" << LENDL;
    return false;
  }
  if (v1 == graph.firstVertex() || v1 == graph.lastVertex() ||
      v2 == graph.firstVertex() || v2 == graph.lastVertex() )
  {
  LDEBUG << "DefineString: false" << LENDL;
    return false;
  }
  simplificationData->coordStringsDefs().insert(std::make_pair(complement,std::vector<LinguisticGraphVertex>()));
  std::vector<LinguisticGraphVertex>& stringDef = (*simplificationData->coordStringsDefs().find(complement)).second;
  LDEBUG << "Add " << v1 << " to string " << complement << LENDL;
  stringDef.push_back(v1);
  
  LinguisticGraphVertex next = v1;
  while (next != v2)
  {
    next = boost::target(*(boost::out_edges(next, *graph.getGraph()).first),*graph.getGraph());
    LDEBUG << "Add " << next << " to string " << complement << LENDL;
    stringDef.push_back(next);
  }
  
  LDEBUG << "DefineString: true" << LENDL;
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
  LDEBUG << "Testing SameString on " << v1 << " and " << v2 << LENDL;

  std::string complement = Common::Misc::limastring2utf8stdstring(m_complement);
  SyntacticData* syntacticData =
    static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  SimplificationData* simplificationData =
    static_cast<SimplificationData*>(analysis.getData("SimplificationData"));
  LinguisticGraph* graph = syntacticData->graph();
  if (simplificationData==0)
  {
    SACLOGINIT;
    LERROR << "No simplificationData in DefineString constraint" << LENDL;
    return false;
  }
  std::vector<LinguisticGraphVertex> stringDef;
  if (simplificationData->coordStringsDefs().find(complement) == simplificationData->coordStringsDefs().end())
  {
    SACLOGINIT;
    LWARN << "No such coord string defined: " << complement << LENDL;
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
  LDEBUG << "Add " << v1 << " to new SameString " << complement << LENDL;
  newString.push_back(v1);
  LinguisticGraphVertex next = v1;
  while (next != v2)
  {
    next = boost::target(*(boost::out_edges(next, *graph).first),*graph);
    LDEBUG << "Add " << next << " to new string " << complement << LENDL;
    newString.push_back(next);
  }
  if (!stringDef.empty() && newString.size() != stringDef.size())
  {
    LDEBUG << "New string and model for " << complement << " have different sizes" << LENDL;
    
    return false;
  }
  if (stringDef.empty() && (*simplificationData->coordStrings().find(complement)).second.empty() )
  {
    LDEBUG << "No definition and no same string yet : using myself" << LENDL;
    stringDef = newString;
  }
  else if (stringDef.empty())
  {
    LDEBUG << "No definition yet : using first found same string" << LENDL;
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
      LDEBUG << "new string vertex " << *nit << " and model string vertex " << *dit << " have different micro categories" << LENDL;
      return false;
    }
  }
  LDEBUG << "Add string " << v1 << "/" << v2 << " to same strings of " << complement << LENDL;
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
  LDEBUG << "clearing stored subsentences" << LENDL;
  
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
  LDEBUG << "clearing stored subsentences" << LENDL;
  
  //SimplificationData* simplificationData =
  //static_cast<SimplificationData*>(analysis.getData("SimplificationData"));

  return true;
}

} // end namespace SyntacticAnalysis
} // end namespace LinguisticProcessing
} // end namespace Lima
