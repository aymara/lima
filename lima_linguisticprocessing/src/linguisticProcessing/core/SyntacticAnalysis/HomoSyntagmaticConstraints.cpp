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
  *              functions for the detection of homosyntagmatic dependency
  *              relations
  *
  * @file        HomoSyntagmaticConstraints.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2003 by CEA
  * @date        Created on  Thu Nov, 13 2003
  *
  *
  */

#include "HomoSyntagmaticConstraints.h"
#include "DependencyGraph.h"
#include "SyntacticData.h"
#include "SyntagmaticMatrix.h"

#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

#include <string>
#include <map>
#include <queue>
#include <boost/regex.hpp>

using namespace boost;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
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
Automaton::ConstraintFunctionFactory<SecondUngovernedBy>
SecondUngovernedByFactory(SecondUngovernedById);

Automaton::ConstraintFunctionFactory<GovernorOf>
GovernorOfFactory(GovernorOfId);

Automaton::ConstraintFunctionFactory<GovernedBy>
GovernedByFactory(GovernedById);

Automaton::ConstraintFunctionFactory<SameNominalChain>
SameNominalChainFactory(SameNominalChainId);

Automaton::ConstraintFunctionFactory<SameVerbalChain>
SameVerbalChainFactory(SameVerbalChainId);

Automaton::ConstraintFunctionFactory<CreateRelationBetween>
CreateRelationBetweenFactory(CreateRelationBetweenId);

Automaton::ConstraintFunctionFactory<CreateRelationWithRelated>
CreateRelationWithRelatedFactory(CreateRelationWithRelatedId);

Automaton::ConstraintFunctionFactory<CreateRelationReverseWithRelated>
CreateRelationReverseWithRelatedFactory(CreateRelationReverseWithRelatedId);

Automaton::ConstraintFunctionFactory<CreateCompoundTense>
CreateCompoundTenseFactory(CreateCompoundTenseId);

Automaton::ConstraintFunctionFactory<CreateEasyCompoundTense>
CreateEasyCompoundTenseFactory(CreateEasyCompoundTenseId);

Automaton::ConstraintFunctionFactory<FindRelationFrom>
FindRelationFromFactory(FindRelationFromId);

Automaton::ConstraintFunctionFactory<EnforcePropertiesConstraints>
EnforcePropertiesConstraintsFactory(EnforcePropertiesConstraintsId);

Automaton::ConstraintFunctionFactory<AddRelationInGraph>
AddRelationInGraphFactory(AddRelationInGraphId);

Automaton::ConstraintFunctionFactory<ModifyRelationInGraph>
ModifyRelationInGraphFactory(ModifyRelationInGraphId);

Automaton::ConstraintFunctionFactory<ClearStoredRelations>
ClearStoredRelationsFactory(ClearStoredRelationsId);
 
//**********************************************************************
ConstraintWithRelationComplement::ConstraintWithRelationComplement(
  MediaId language,
  const LimaString& complement):
    ConstraintFunction(language,complement),
    m_relation(0)
{
  if (! complement.isEmpty())
  {
    std::string str=Common::Misc::limastring2utf8stdstring(complement);
    if (str == "ANY")
    {
      m_relation=0; // default value=0
    }
    else
    {
      m_relation=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationId(str);
      if (m_relation == 0)
      {
        SALOGINIT;
        LERROR << "undefined syntactic relation [" << str << "]";
        throw LimaException();
      }
    }
  }
}

//**********************************************************************
SecondUngovernedBy::SecondUngovernedBy(
  MediaId language,
  const LimaString& complement):
    ConstraintWithRelationComplement(language,complement)
{}

bool SecondUngovernedBy::operator()(
  const LinguisticAnalysisStructure::AnalysisGraph& graph,
  const LinguisticGraphVertex& v1,
  const LinguisticGraphVertex& v2,
                                     AnalysisContent& analysis ) const
{
/*
  Critical Function : comment logging messages
*/
//   SAPLOGINIT;
//   LDEBUG << "testing SecondUngovernedBy for "
//   << v1 << " and " << v2
//   << " with relation: " << m_relation << LENDL;

  const SyntacticData* syntacticData=static_cast<const SyntacticData*>(analysis.getData("SyntacticData"));

  if (v1 == graph.firstVertex() || v1 == graph.lastVertex() ||
      v2 == graph.firstVertex() || v2 == graph.lastVertex() )
  {
//     LDEBUG << "SecondUngovernedBy: false" << LENDL;
    return false;
  }
  CEdgeDepRelTypePropertyMap map = get(edge_deprel_type, *(syntacticData-> dependencyGraph()));
  DependencyGraphVertex dv1 = syntacticData->depVertexForTokenVertex(v1);
  DependencyGraphVertex dv2 = syntacticData->depVertexForTokenVertex(v2);
  DependencyGraphOutEdgeIt it, it_end;

  // Modif YJE 2007-07-13
  // On ne peut pas utiliser edge_range pour trouver les arcs entre
  // dv1 et dv2 car le conteneur stockant les arcs dans le graphe
  // n'est pas ordonné.
  // Il faut donc parcourir tous les arcs sortant de dv1, et filtrer
  // ceux entrant dans dv2.
  //  boost::tie(it, it_end) = edge_range(dv1, dv2, *(syntacticData-> dependencyGraph()));
  const DependencyGraph *g = syntacticData-> dependencyGraph();
  boost::tie(it, it_end) = out_edges(dv1, *g);
  for (; it != it_end; it++)
  {
    // Modif YJE 2007-07-13
    // On ne garde que les arcs entrant dans dv2.
    if(target(*it, *g) != dv2)
      continue;

    if ( (map[*it] == m_relation)
          || (m_relation == 0) )
    {
//       LDEBUG << "SecondUngovernedBy: false" << LENDL;
      return false;
    }
  }
  //   LDEBUG << "SecondUngovernedBy: true" << LENDL;
  return true;
}

GovernorOf::GovernorOf(MediaId language,
                       const LimaString& complement):
    ConstraintWithRelationComplement(language,complement)
{
  //  ConstraintFunction::addFunction("GovernorOf",this);
}

bool GovernorOf::operator()(const AnalysisGraph& graph,
                            const LinguisticGraphVertex& v1,
                            AnalysisContent& analysis) const
{
/*
  Critical function : comment logging messages
*/
//  SAPLOGINIT;
//  LDEBUG << "testing GovernorOf for " << v1
//  << " with relation : " << m_relation << LENDL;

  const SyntacticData* syntacticData=static_cast<const SyntacticData*>(analysis.getData("SyntacticData"));

  if (v1 == graph.firstVertex() || v1 == graph.lastVertex() )
  {
//    LDEBUG << "GovernorOf: false" << LENDL;
    return false;
  }
  CEdgeDepRelTypePropertyMap map = get(edge_deprel_type, *(syntacticData-> dependencyGraph()));
  DependencyGraphVertex dv1 = syntacticData-> depVertexForTokenVertex(v1);
  DependencyGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end) = out_edges(dv1, *(syntacticData-> dependencyGraph()));
  for (; it != it_end; it++)
  {
    if ( (map[*it] == m_relation)
         || (m_relation == 0) )
    {
//      LDEBUG << "GovernorOf: true" << LENDL;
      return true;
    }
  }
//  LDEBUG << "GovernorOf: false" << LENDL;
  return false;
}


GovernedBy::GovernedBy(MediaId language,
                       const LimaString& complement):
    ConstraintWithRelationComplement(language,complement)
{
  //  ConstraintFunction::addFunction("GovernedBy",this);
}

bool GovernedBy::operator()(const AnalysisGraph& graph,
                            const LinguisticGraphVertex& v1,
                            AnalysisContent& analysis) const
{
/*
  Critical function : comment logging message
*/
//  SAPLOGINIT;
//  LDEBUG << "testing GovernedBy for " << v1
//  << " with relation: " << m_relation << LENDL;
  const SyntacticData* syntacticData=static_cast<const SyntacticData*>(analysis.getData("SyntacticData"));
  if (v1 == graph.firstVertex() || v1 == graph.lastVertex() )
  {
//    LDEBUG << "GovernedBy: false" << LENDL;
    return false;
  }
  CEdgeDepRelTypePropertyMap map = get(edge_deprel_type, *(syntacticData-> dependencyGraph()));
  DependencyGraphVertex dv1 = syntacticData-> depVertexForTokenVertex(v1);
  DependencyGraphInEdgeIt it, it_end;
  boost::tie(it, it_end) = in_edges(dv1, *(syntacticData-> dependencyGraph()));
  for (; it != it_end; it++)
  {
    if ( (map[*it] == m_relation)
         || (m_relation == 0) )
    {
//      LDEBUG << "GovernedBy: true" << LENDL;
      return true;
    }
  }

//  LDEBUG << "GovernedBy: false" << LENDL;
  return false;
}

SameNominalChain::SameNominalChain(MediaId language,
                                   const LimaString& complement):
    ConstraintFunction(language,complement)
{
  // no complement
}

/** @todo utiliser graph.sameNominalChain ici aussi. Voir pourquoi le
    test sur first et last vertex pose probleme ici. */
bool SameNominalChain::operator()(const AnalysisGraph& graph,
                                  const LinguisticGraphVertex& v1,
                                  const LinguisticGraphVertex& v2,
                                  AnalysisContent& /*ac*/) const
{
/*
  Critical function : comment logging message
*/
//  SAPLOGINIT;
//  LDEBUG << "testing SameNominalChain for " << v1 << " and " << v2
//  << LENDL;

  CVertexChainIdPropertyMap map = get(vertex_chain_id, *(graph.getGraph()));
  VertexChainIdProp::const_iterator it1 = map[v1].begin();
  VertexChainIdProp::const_iterator it1_end = map[v1].end();


  for (; it1 != it1_end; it1++)
  {
    VertexChainIdProp::const_iterator it2 = map[v2].begin();
    VertexChainIdProp::const_iterator it2_end = map[v2].end();
    for (; it2 != it2_end; it2++)
    {
//      LDEBUG << v1 << "/" << v2 << " : " << (*it1) << " =?= " << (*it2);
      if ( ( (*it1).chainType() == (*it2).chainType() ) &&
            ( (*it1).chainType() == NOMINAL) &&
           //                    ( (*it1).pathId() == (*it2).pathId() ) &&
           ( (*it1).chainId() == (*it2).chainId() ) )
      {
//        LDEBUG << " : yes" << LENDL;
        return true;
      }
//      LDEBUG << " : no" << LENDL;
    }
  }
  return false;

}

SameVerbalChain::SameVerbalChain(MediaId language,
                                 const LimaString& complement):
    ConstraintFunction(language,complement)
{}

/** @todo utiliser graph.SameVerbalChain ici aussi. Voir pourquoi le
    test sur first et last vertex pose probleme ici. */
bool SameVerbalChain::operator()(const AnalysisGraph& graph,
                                 const LinguisticGraphVertex& v1,
                                 const LinguisticGraphVertex& v2,
                                 AnalysisContent& /*ac*/) const
{
/*
  Critical function : comment logging message
*/
  //    return graph.SameVerbalChain(v1, v2, false);

//  SAPLOGINIT;
//  LDEBUG << "testing SameVerbalChain for " << v1 << " and " << v2 << LENDL;
  CVertexChainIdPropertyMap map = get(vertex_chain_id, *(graph.getGraph()));
  VertexChainIdProp::const_iterator it1 = map[v1].begin();
  VertexChainIdProp::const_iterator it1_end = map[v1].end();

  for (; it1 != it1_end; it1++)
  {
    VertexChainIdProp::const_iterator it2 = map[v2].begin();
    VertexChainIdProp::const_iterator it2_end = map[v2].end();
    for (; it2 != it2_end; it2++)
    {
      ChainIdStruct s1 = (*it1);
      ChainIdStruct s2 = (*it2);
//      LDEBUG << v1 << "/" << v2 << " : " << s1 << " =?= " << s2;
      if ( ( s1.chainType() == s2.chainType() ) &&
           ( s1.chainType() == VERBAL ) &&
           //                    ( (*it1).pathId() == (*it2).pathId() ) &&
           ( s1.chainId() == s2.chainId() ) )
      {
//        LDEBUG << " : return yes" << LENDL;
        return true;
      }
//      LDEBUG << " : no" << LENDL;
    }
  }
//  LDEBUG << " : return no" << LENDL;
  return false;

}


CreateRelationBetween::CreateRelationBetween(
  MediaId language,
  const LimaString& complement):
    ConstraintWithRelationComplement(language,complement)
{}

bool CreateRelationBetween::operator()(const AnalysisGraph&,
                                       const LinguisticGraphVertex& v1,
                                       const LinguisticGraphVertex& v2,
                                       AnalysisContent& analysis ) const
{
/*
  Critical function : comment logging message
*/
//  SAPLOGINIT;
//  LDEBUG << "testing CreateRelationBetween for " << v1 << " and "
//  << v2  << " with relation: " << m_relation << LENDL;
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  bool res = syntacticData->relation(v1, v2, m_relation);
//  LDEBUG << "CreateRelationBetween: " << (res?"yes":"no") << LENDL;
  return res;
}


//**********************************************************************

FindRelationFrom::FindRelationFrom(
  MediaId language,
  const LimaString& complement):
    ConstraintWithRelationComplement(language,complement)
{}

bool FindRelationFrom::operator()(const AnalysisGraph&,
                                  const LinguisticGraphVertex& v1,
                                  AnalysisContent& analysis) const
{
/*
  Critical function : comment logging message
*/
//  SAPLOGINIT;
//  LDEBUG << "testing FindRelationFRom for " << v1 << LENDL;
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  DependencyGraph* depGraph=syntacticData->dependencyGraph();


  DependencyGraphVertex dv1 = syntacticData->depVertexForTokenVertex(v1);
  DependencyGraphOutEdgeIt outItr,outItrEnd;

  std::queue<DependencyGraphVertex> toVisit;
  toVisit.push(dv1);
  while (!toVisit.empty())
  {

    DependencyGraphVertex dv=toVisit.front();
    toVisit.pop();
//    LDEBUG << "visit dep vertex " << dv << LENDL;

    for (boost::tie(outItr,outItrEnd)=out_edges(dv,*depGraph);
         outItr!=outItrEnd;
         outItr++)
    {
      if ( get(edge_deprel_type,*depGraph,*outItr) == m_relation)
        {
//          LDEBUG << "find relation " << m_relation << " for edge " << *outItr << LENDL;
          syntacticData->relation(
            syntacticData->tokenVertexForDepVertex(dv),
            syntacticData->tokenVertexForDepVertex(target(*outItr,*depGraph)),
            m_relation);
          return true;
        }
      toVisit.push(target(*outItr,*depGraph));
    }
  }
//  LDEBUG << "FindRelationFrom: no" << LENDL;
  return false;
}


//**********************************************************************

CreateRelationWithRelated::CreateRelationWithRelated(
  MediaId language,
  const LimaString& complement):
    ConstraintFunction(language,complement),
    m_relationsToFollow(),
    m_relationToCreate(0)
{
  std::string str=Common::Misc::limastring2utf8stdstring(complement);

  //parse complement: contains two relation names separated by a comma
  // uint64_t i=str.find(","); portage 32 64
  std::string::size_type i=str.find(",");
  if (i == std::string::npos)
  {
    SAPLOGINIT;
    LERROR << "Error: CreateRelationWithRelated complement must have two types" << LENDL;
    throw LimaException();
  }
  m_relationToCreate=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationId(std::string(str,i+1));

  //relations to follow : may be several separated by a "|"
  std::string rel(str,0,i);
  //uint64_t j=rel.find("|"); portage 32 64
  std::string::size_type j=rel.find("|");
  while (j!=std::string::npos)
  {
    m_relationsToFollow.
    insert(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationId(std::string(rel,0,j)));
    rel.erase(0,j+1);
    j=rel.find("|");
  }
  if (! rel.empty())
  {
    m_relationsToFollow.
    insert(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationId(rel));
  }
}

std::vector<LinguisticGraphVertex> CreateRelationWithRelated::
findRelatedVertices(const LinguisticGraphVertex& v2,
                    SyntacticData* syntacticData) const
{

//   SAPLOGINIT;

  std::vector<LinguisticGraphVertex> vfollow=
    syntacticData->findRelated(v2,m_relationsToFollow);

//   if (vfollow.empty())
//   {
//     if (logger.isDebugEnabled())
//     {
//       std::ostringstream oss;
//       std::set<Common::MediaticData::SyntacticRelationId>::const_iterator
//         it=m_relationsToFollow.begin(),
//            it_end=m_relationsToFollow.end();
//       for (; it!=it_end; it++)
//       {
//         oss << *it<< ",";
//       }
//       LDEBUG << "CreateRelationWithRelated: no relation "
//       << oss.str() << " attached to vertex " << v2 << LENDL;
//     }
//   }
  return vfollow;
}

bool CreateRelationWithRelated::operator()(
  const AnalysisGraph&,
  const LinguisticGraphVertex& v1,
  const LinguisticGraphVertex& v2,
  AnalysisContent& analysis ) const
{
/*
  Critical function : comment logging message
*/
//  SAPLOGINIT;
//  LDEBUG << "testing CreateRelationWithRelated for " << v1
//  << " and " << v2 << LENDL;

  SyntacticData* syntacticData=
    static_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  std::vector<LinguisticGraphVertex> vfollow=
    findRelatedVertices(v2,syntacticData);

  if (vfollow.empty())
  {
    return false;
  }

  // add the relations
  std::vector<LinguisticGraphVertex>::const_iterator
  v=vfollow.begin(),
    v_end=vfollow.end();
  for (; v!=v_end; v++)
  {
//    LDEBUG << "storing relation " << m_relationToCreate
//    << " between " << v1 << " and " << *v << LENDL;
    syntacticData->relation(v1, *v, m_relationToCreate);
  }
//  LDEBUG << "CreateRelationWithRelated: yes" << LENDL;
  return true;
}

//**********************************************************************
CreateRelationReverseWithRelated::CreateRelationReverseWithRelated(
  MediaId language,
  const LimaString& complement):
    CreateRelationWithRelated(language,complement)
{}

bool CreateRelationReverseWithRelated::operator()(
  const AnalysisGraph&,
  const LinguisticGraphVertex& v1,
  const LinguisticGraphVertex& v2,
  AnalysisContent& analysis ) const
{
/*
  Critical function : comment logging message
*/
//  SAPLOGINIT;

  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  std::vector<LinguisticGraphVertex> vfollow=
    findRelatedVertices(v2,syntacticData);

  if (vfollow.empty())
  {
    return false;
  }

  // add the relations
  std::vector<LinguisticGraphVertex>::const_iterator
  v=vfollow.begin(),
    v_end=vfollow.end();
  for (; v!=v_end; v++)
  {
//    LDEBUG << "storing relation " << m_relationToCreate
//    << " between " << v1 << " and " << *v << LENDL;
    syntacticData->relation(*v, v1, m_relationToCreate);
  }
//  LDEBUG << "CreateRelationReverseWithRelated: yes" << LENDL;
  return true;
}

//**********************************************************************
// complement contains symbols for category and microcategory
// (e.g.: L_NC;L_NC_GEN;)
CreateCompoundTense::CreateCompoundTense(MediaId language,
    const LimaString& complement):
    ConstraintFunction(language,complement),
    m_macro(0),
    m_micro(0),
    m_tempCompType(0)
{
  const std::string str=
    Common::Misc::limastring2utf8stdstring(complement);

  size_t firstSepPos = str.find_first_of(';');
  m_macro=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertyValue(str.substr(0, firstSepPos));

  size_t secondSepPos = str.find_first_of(';', firstSepPos+1);
  m_micro=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertyValue(str.substr(firstSepPos + 1, secondSepPos - firstSepPos - 1));

  m_tempCompType=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationId("TEMPCOMP");

  m_macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  m_genderAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("GENDER"));
  m_numberAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("NUMBER"));
  m_timeAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("TIME"));
  m_syntaxAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("SYNTAX"));
  m_personAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("PERSON"));
}

/** @param macroMicroAndType = symbols for category and microcategory
    (e.g.: L_NC;L_NC_GEN;) */
bool CreateCompoundTense::operator()(const AnalysisGraph& anagraph,
                                     const LinguisticGraphVertex& pastPartVertex,
                                     const LinguisticGraphVertex& auxVertex,
                                     AnalysisContent& analysis ) const
{
/*
  Critical function : comment logging message
*/
 SAPLOGINIT;
 LDEBUG << "creating compound tense for " << auxVertex << " and "
 << pastPartVertex << LENDL;


  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    return false;
  }

  // getting data
  DependencyGraph& depGraph= *(syntacticData->dependencyGraph());
  LinguisticGraph* graph = const_cast<LinguisticGraph*>(anagraph.getGraph());
  VertexTokenPropertyMap tokenMap = get(vertex_token, *graph);
  VertexDataPropertyMap dataMap = get(vertex_data, *graph);
  VertexChainIdPropertyMap chainsIdsMap = get(vertex_chain_id, *graph);

  MediaId language = Common::MediaticData::MediaticData::single().media(metadata->getMetaData("Lang"));

  Token* tokenAux = tokenMap[auxVertex];
  Token* tokenPastPart = tokenMap[pastPartVertex];
  const MorphoSyntacticData* dataAux = dataMap[auxVertex];
  const MorphoSyntacticData* dataPastPart = dataMap[pastPartVertex];
  LinguisticCode dataAuxMicro = dataAux->firstValue(*m_microAccessor);

  LinguisticCode tense = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).compoundTense(dataAuxMicro, dataAux->firstValue(*m_timeAccessor));
  LDEBUG << "Tense = '" << tense << "' " << LENDL;

  if (m_macroAccessor->empty(m_macro) || m_microAccessor->empty(m_micro))
  {
   LDEBUG << "CreateCompoundTense: false because macro="
   << m_macro << " and micro=" << m_micro << LENDL;
    return false;
  }

  // creer un full token
  // this version changes the head verb of the sentence to be the past participle verb
  LimaString verbFlex = tokenAux->stringForm() + Common::Misc::utf8stdstring2limastring("_") + tokenPastPart->stringForm();
  StringsPoolIndex verbLemma = *((dataPastPart->allLemma()).begin());

  // this version keeps the auxiliary verb
//   LimaString verbFlex = tokenAux->stringForm() ;
  LinguisticElement elem(*(dataPastPart->begin()));
//   StringsPoolIndex verbLemma = elem.normalizedForm;

  Token* tokenNewVerb = new Token(Common::MediaticData::MediaticData::changeable().stringsPool(language)[verbFlex],
                                  verbFlex,
                                  tokenAux->position(),
                                  verbFlex.size());
  tokenNewVerb->setStatus(tokenMap[auxVertex]->status());

  /// if the anagraph is not set to delete the tokens, we have to do it
  if (!anagraph.ownsTokens())
  {
    syntacticData->ownedTokens().push_back(tokenNewVerb);
  }

  // creer un MorphoSyntacticData
  LDEBUG << "Creating a DicoWord: "
      << int(m_macro) << " / " << Common::Misc::limastring2utf8stdstring(verbFlex) << " / "
      << int(m_micro) << " / " << verbLemma << LENDL;
  MorphoSyntacticData* dataNewVerb = new MorphoSyntacticData();
  /// if the anagraph is not set to delete the morphosyntactic data, we have to do it
  if (!anagraph.ownsMorphData())
  {
    syntacticData->ownedMorphosyntacticData().push_back(dataNewVerb);
  }
  elem.inflectedForm = Common::MediaticData::MediaticData::changeable().stringsPool(language)[verbFlex];
  elem.lemma = verbLemma;
  elem.normalizedForm = *((dataPastPart->allNormalizedForms()).begin());
  m_macroAccessor->writeValue( m_macro, elem.properties);
  m_microAccessor->writeValue( m_micro, elem.properties);
  // set the new morphosyntactic data tense to be the found tense
  m_timeAccessor->writeValue( tense, elem.properties);
  // keeps the genre and syntax from the participle
  // copies the person and number from the auxiliary
  m_personAccessor->writeValue( dataAux->firstValue(*m_personAccessor), elem.properties);
  m_numberAccessor->writeValue( dataAux->firstValue(*m_numberAccessor), elem.properties);
  dataNewVerb->push_back(elem);

  // creer le noeud et ses 2 arcs

  LinguisticGraphVertex newVertex;
  DependencyGraphVertex newDepVertex;
  boost::tie (newVertex, newDepVertex) = syntacticData->addVertex();
  LDEBUG << "New vertices are " << newVertex << " (pos) and " << newDepVertex << " (dep)" << LENDL;

  tokenMap[newVertex] = tokenNewVerb;
  dataMap[newVertex] = dataNewVerb;


  // affecter les chaines verbales (ensemble des chaines comprises dans
  // l'intersection des ensembles de chaines de l'auxiliaire et du participe
  VertexChainIdProp& auxChains = chainsIdsMap[auxVertex];
  VertexChainIdProp& pastPartChains = chainsIdsMap[pastPartVertex];
  std::set< uint64_t> auxChainsIds;
  VertexChainIdProp::iterator auxChainsIt, auxChainsIt_end;
  auxChainsIt = auxChains.begin(); auxChainsIt_end = auxChains.end();
  for (; auxChainsIt != auxChainsIt_end; auxChainsIt++)
    auxChainsIds.insert((*auxChainsIt).chainId());
  std::set< uint64_t > pastPartChainsIds;
  VertexChainIdProp::iterator pastPartChainsIt, pastPartChainsIt_end;
  pastPartChainsIt = pastPartChains.begin(); pastPartChainsIt_end = pastPartChains.end();
  for (; pastPartChainsIt != pastPartChainsIt_end; pastPartChainsIt++)
    pastPartChainsIds.insert((*pastPartChainsIt).chainId());
  std::set< uint64_t > newVerbChainsIds;
  std::set_intersection(
    auxChainsIds.begin(), auxChainsIds.end(),
    pastPartChainsIds.begin(), pastPartChainsIds.end(),
    std::insert_iterator< std::set< uint64_t > >(newVerbChainsIds, newVerbChainsIds.end()));

  std::set< LinguisticAnalysisStructure::ChainIdStruct > newVerbChains;
  auxChainsIt = auxChains.begin(); auxChainsIt_end = auxChains.end();
  for (; auxChainsIt != auxChainsIt_end; auxChainsIt++)
  {
    if ( newVerbChainsIds.find( (*auxChainsIt).chainId() ) != newVerbChainsIds.end() )
    {
      newVerbChains.insert(*auxChainsIt);
    }
  }

  chainsIdsMap[auxVertex] = newVerbChains;
  chainsIdsMap[pastPartVertex] = newVerbChains;
  chainsIdsMap[newVertex] = newVerbChains;

  // creer les relations necessaires dans le graphe morphosyntaxique
  // les relations sont creees entre noeuds ayant au moins une chaine en commun
  std::set< std::pair< LinguisticGraphVertex, LinguisticGraphVertex > > edgesToRemove;

  // on ne doit pas creer d'arc en double, il faut donc garder trace des arcs ajoutes
  std::set< std::pair< LinguisticGraphVertex, LinguisticGraphVertex > > addedEdges;
  
  //1. entre les noeuds avant l'auxiliaire et le nouveau noeud
  LDEBUG << "The auxiliary has " << in_degree(auxVertex, *graph) << " in edges." << LENDL;
  LinguisticGraphInEdgeIt auxInEdgesIt, auxInEdgesIt_end;
  boost::tie(auxInEdgesIt, auxInEdgesIt_end) = in_edges(auxVertex, *graph);
  for (; auxInEdgesIt != auxInEdgesIt_end; auxInEdgesIt++)
  {
    LDEBUG << "auxInEdge: " << source(*auxInEdgesIt, *graph) << " -> " << target(*auxInEdgesIt, *graph) << LENDL;
    LinguisticGraphVertex auxInVertex = source(*auxInEdgesIt, *graph);
    if (addedEdges.find(std::make_pair(auxInVertex,newVertex)) != addedEdges.end())
    {
      // l'arc a deja ete ajoute, abandonner
      continue;
    }
    LDEBUG << "edge to add (step1): " << auxInVertex << " -> " << newVertex << LENDL;
    edgesToRemove.insert(std::make_pair(source(*auxInEdgesIt,*graph),target(*auxInEdgesIt,*graph)));
    addedEdges.insert(std::make_pair(auxInVertex,newVertex));
  }
  //2. entre le nouveau noeud et les noeuds qui etaient entre l'auxiliaire et
  //   le verbe
  LinguisticGraphOutEdgeIt auxOutEdgesIt, auxOutEdgesIt_end;
  boost::tie(auxOutEdgesIt, auxOutEdgesIt_end) = out_edges(auxVertex, *graph);
  for (; auxOutEdgesIt != auxOutEdgesIt_end; auxOutEdgesIt++)
  {
      LinguisticGraphVertex auxOutVertex = target(*auxOutEdgesIt, *graph);
      if (auxOutVertex ==  pastPartVertex) continue;
      chainsIdsMap[auxOutVertex] = newVerbChains;
      if (addedEdges.find(std::make_pair(newVertex,auxOutVertex)) != addedEdges.end())
      {
      // l'arc a deja ete ajoute, abandonner
        continue;
      }
//       bool success;
//       LinguisticGraphEdge e;
//       boost::tie(e, success) = add_edge(newVertex, auxOutVertex, *graph);
//       if (success)
//       {
        LDEBUG << "edge to add (step2): " << newVertex << " -> " << auxOutVertex << LENDL;
        edgesToRemove.insert(std::make_pair(source(*auxOutEdgesIt,*graph),target(*auxOutEdgesIt,*graph)));
        addedEdges.insert(std::make_pair(newVertex,auxOutVertex));
//         break;
//       }
  }
  //3. entre les noeuds qui etaient entre l'auxiliaire et le participe (avant
  //   le participe) et les noeuds qui etaient apres le participe ;
  //   egalement, relie le nouveau noeud aux noeuds qui etaient apres
  //   l'auxiliaire au cas ou l'auxiliaire et le participe etaient contigus
  LinguisticGraphInEdgeIt pastPartInEdgesIt, pastPartInEdgesIt_end;
  boost::tie(pastPartInEdgesIt, pastPartInEdgesIt_end) = in_edges(pastPartVertex, *graph);
  for (; pastPartInEdgesIt != pastPartInEdgesIt_end; pastPartInEdgesIt++)
  {
    LinguisticGraphVertex pastPartInVertex = source(*pastPartInEdgesIt, *graph);
    if (pastPartInVertex == auxVertex)
    {
      LDEBUG << "no vertex between past participle and auxiliary" << LENDL;
      pastPartInVertex = newVertex;

    }
    LDEBUG << "past participle in vertex  " << pastPartInVertex << LENDL;
    edgesToRemove.insert(std::make_pair(source(*pastPartInEdgesIt,*graph),target(*pastPartInEdgesIt,*graph)));

    LinguisticGraphOutEdgeIt pastPartOutEdgesIt, pastPartOutEdgesIt_end;
    boost::tie(pastPartOutEdgesIt, pastPartOutEdgesIt_end) = out_edges(pastPartVertex, *graph);
    for (; pastPartOutEdgesIt != pastPartOutEdgesIt_end; pastPartOutEdgesIt++)
    {
      LinguisticGraphVertex pastPartOutVertex = target(*pastPartOutEdgesIt, *graph);
      LDEBUG << "past participle out vertex " << pastPartOutVertex << LENDL;

      chainsIdsMap[pastPartInVertex] = newVerbChains;
      if (addedEdges.find(std::make_pair(pastPartInVertex,pastPartOutVertex)) != addedEdges.end())
      {
      // l'arc a deja ete ajoute, abandonner
        continue;
      }
//       bool success;
//       LinguisticGraphEdge e;
//       boost::tie(e, success) = add_edge(pastPartInVertex, pastPartOutVertex, *graph);
//       if (success)
//       {
      LDEBUG << "edge to add (step3): " << pastPartInVertex << " -> " << pastPartOutVertex << LENDL;
        edgesToRemove.insert(std::make_pair(source(*pastPartOutEdgesIt,*graph),target(*pastPartOutEdgesIt,*graph)));
        addedEdges.insert(std::make_pair(pastPartInVertex,pastPartOutVertex));
//         break;
//       }
    }
  }
  std::set< std::pair< LinguisticGraphVertex, LinguisticGraphVertex > >::const_iterator
      edgesToRemoveIt, edgesToRemoveIt_end;
  edgesToRemoveIt = edgesToRemove.begin();
  edgesToRemoveIt_end = edgesToRemove.end();
  for (; edgesToRemoveIt != edgesToRemoveIt_end; edgesToRemoveIt++)
  {
    LDEBUG << "remove edge " << (*edgesToRemoveIt).first << " -> " << (*edgesToRemoveIt).second << LENDL;
    remove_edge( (*edgesToRemoveIt).first, (*edgesToRemoveIt).second,*graph);
  }

  std::set< std::pair< LinguisticGraphVertex, LinguisticGraphVertex > >::const_iterator addedEdgesIt, addedEdgesIt_end;
  addedEdgesIt = addedEdges.begin(); addedEdgesIt_end = addedEdges.end();
  for (; addedEdgesIt != addedEdgesIt_end; addedEdgesIt++)
  {
    bool success;
    LinguisticGraphEdge e;
    boost::tie(e, success) = add_edge(addedEdgesIt->first, addedEdgesIt->second, *graph);
    if (success)
    {
      LDEBUG << "edge added : " << e.m_source << " -> " << e.m_target << LENDL;
    }
    else
    {
      LERROR << "could not add edge : " << addedEdgesIt->first << " -> " <<  addedEdgesIt->second << LENDL;
    }
  }

  // copier vers le noeud du nouveau verbe toutes les relations de
  // dépendance (sauf TEMPCOMP) qui avaient pour source ou destination
  // l'auxiliaire ou le participe passé
  EdgeDepRelTypePropertyMap edgeTypeMap = get( edge_deprel_type, depGraph);

  DependencyGraphVertex auxDepVertex =
      syntacticData-> depVertexForTokenVertex(auxVertex);
  DependencyGraphInEdgeIt auxDepVertexInEdgeIt, auxDepVertexInEdgeIt_end;
  boost::tie(auxDepVertexInEdgeIt, auxDepVertexInEdgeIt_end) =
      in_edges(auxDepVertex,depGraph);
  for (; auxDepVertexInEdgeIt != auxDepVertexInEdgeIt_end;
         auxDepVertexInEdgeIt++)
  {
    DependencyGraphEdge e;
    bool success;
    boost::tie(e, success) = add_edge(source(*auxDepVertexInEdgeIt,depGraph),newDepVertex,depGraph);
    if (success)
    {
      LDEBUG << "edge added : " << e.m_source << " -> " << e.m_target << LENDL;
      edgeTypeMap[e] = edgeTypeMap[*auxDepVertexInEdgeIt];
    }
  }
  DependencyGraphOutEdgeIt auxDepVertexOutEdgeIt, auxDepVertexOutEdgeIt_end;
  boost::tie(auxDepVertexOutEdgeIt, auxDepVertexOutEdgeIt_end) =
      out_edges(auxDepVertex,depGraph);
  for (; auxDepVertexOutEdgeIt != auxDepVertexOutEdgeIt_end;
         auxDepVertexOutEdgeIt++)
  {
    DependencyGraphEdge e;
    bool success;
    boost::tie(e, success) = add_edge(newDepVertex, target(*auxDepVertexOutEdgeIt,depGraph),depGraph);
    if (success)
    {
      LDEBUG << "edge added : " << e.m_source << " -> " << e.m_target << LENDL;
      edgeTypeMap[e] = edgeTypeMap[*auxDepVertexOutEdgeIt];
    }
  }
  clear_vertex(auxDepVertex, depGraph);

  DependencyGraphVertex pastPartDepVertex =
      syntacticData-> depVertexForTokenVertex(pastPartVertex);
  DependencyGraphInEdgeIt pastPartDepVertexInEdgeIt, pastPartDepVertexInEdgeIt_end;
  boost::tie(pastPartDepVertexInEdgeIt, pastPartDepVertexInEdgeIt_end) =
      in_edges(pastPartDepVertex,depGraph);
  for (; pastPartDepVertexInEdgeIt != pastPartDepVertexInEdgeIt_end;
         pastPartDepVertexInEdgeIt++)
  {
    DependencyGraphEdge e;
    bool success;
    boost::tie(e, success) =
        add_edge(source(*pastPartDepVertexInEdgeIt,depGraph),newDepVertex,depGraph);
    if (success)
    {
      LDEBUG << "edge added : " << e.m_source << " -> " << e.m_target << LENDL;
      edgeTypeMap[e] = edgeTypeMap[*pastPartDepVertexInEdgeIt];
    }
  }
  DependencyGraphOutEdgeIt pastPartDepVertexOutEdgeIt, pastPartDepVertexOutEdgeIt_end;
  boost::tie(pastPartDepVertexOutEdgeIt, pastPartDepVertexOutEdgeIt_end) =
      out_edges(pastPartDepVertex,depGraph);
  for (; pastPartDepVertexInEdgeIt != pastPartDepVertexInEdgeIt_end;
         pastPartDepVertexInEdgeIt++)
  {
    DependencyGraphEdge e;
    bool success;
    boost::tie(e, success) =
        add_edge(newDepVertex, target(*pastPartDepVertexOutEdgeIt,depGraph),depGraph);
    if (success)
    {
      LDEBUG << "edge added : " << e.m_source << " -> " << e.m_target << LENDL;
      edgeTypeMap[e] = edgeTypeMap[*pastPartDepVertexOutEdgeIt];
    }
  }
  clear_vertex(pastPartDepVertex, depGraph);


  /// @todo Creer les annotation idoines, recopier les annotations des anciens
  /// noeuds vers les nouveaux
  AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
  annotationData->annotate(agv,Common::Misc::utf8stdstring2limastring("PosGraph"),newVertex);
  annotationData->addMatching("PosGraph",newVertex,"annot",agv);
  annotationData->annotate(agv,Common::Misc::utf8stdstring2limastring("CpdTense"),1);
  AnnotationGraphVertex annotAuxVertex = *(annotationData->matches("PosGraph",auxVertex,"annot").begin());
  AnnotationGraphVertex annotPastPartVertex = *(annotationData->matches("PosGraph",pastPartVertex,"annot").begin());
  annotationData->annotate(agv, annotAuxVertex, Common::Misc::utf8stdstring2limastring("Aux"), 1);
  annotationData->annotate(agv, annotPastPartVertex, Common::Misc::utf8stdstring2limastring("PastPart"), 1);


  bool res = syntacticData->relation(auxVertex,
                                     pastPartVertex,
                                     m_tempCompType);

  LDEBUG << "CreateCompoundTense: " << res << LENDL;
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  if (recoData == 0)
  {
    recoData = new RecognizerData();
    analysis.setData("RecognizerData", recoData);
  }
  recoData->setNextVertex(newVertex);

  LDEBUG << "aux vertex "<<auxDepVertex<<" out edges num: " << out_degree(auxDepVertex, depGraph) << LENDL;
  LDEBUG << "past part vertex "<<pastPartDepVertex<<" out edges num: " << out_degree(pastPartDepVertex, depGraph) << LENDL;
  return res;

  //    return false;
}

CreateEasyCompoundTense::CreateEasyCompoundTense(MediaId language,
    const LimaString& complement):
    ConstraintFunction(language,complement),
    m_macro(0),
    m_micro(0),
    m_tempCompType(0)
{
  const std::string str=
      Common::Misc::limastring2utf8stdstring(complement);

  size_t firstSepPos = str.find_first_of(';');
  m_macro=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertyValue(str.substr(0, firstSepPos));

  size_t secondSepPos = str.find_first_of(';', firstSepPos+1);
  m_micro=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertyValue(str.substr(firstSepPos + 1, secondSepPos - firstSepPos - 1));

  m_tempCompType=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationId("TEMPCOMP");

  m_macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  m_genderAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("GENDER"));
  m_numberAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("NUMBER"));
  m_timeAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("TIME"));
  m_syntaxAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("SYNTAX"));
  m_personAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("PERSON"));
}

bool CreateEasyCompoundTense::operator()(const AnalysisGraph& /*anagraph*/,
                                   const LinguisticGraphVertex& pastPartVertex,
                                   const LinguisticGraphVertex& auxVertex,
                                   AnalysisContent& analysis ) const
{
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));


  bool res = syntacticData->relation(auxVertex,
                                     pastPartVertex,
                                     m_tempCompType);

  return res;
}

EnforcePropertiesConstraints::EnforcePropertiesConstraints(
  MediaId language,
  const LimaString& complement):
  ConstraintFunction(language,complement), m_language(language)
{
//   SAPLOGINIT;
//   LDEBUG << "Initializing EnforcePropertiesConstraints" << LENDL;
    boost::regex linere("[^,]+");
  std::string str=Common::Misc::limastring2utf8stdstring(complement);
  std::string::const_iterator start, end;
  start = str.begin();
  end = str.end();
  boost::match_results<std::string::const_iterator> what;
  while (regex_search(start, end, what, linere))
  {
    std::string category(what[0].first, what[0].second);
//     LDEBUG << "   adding category: " << category << LENDL;
    const Common::PropertyCode::PropertyAccessor* categ = &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor(category));
    if (categ!=0)
    {
      m_categories.push_back(categ);
    }
    start = what[0].second;
  }
}

bool EnforcePropertiesConstraints::operator()(const AnalysisGraph&,
    const LinguisticGraphVertex& v1,
    const LinguisticGraphVertex& v2,
    AnalysisContent& analysis ) const
{
/*
  Critical function : comment logging message
*/

//  SAPLOGINIT;
//  LDEBUG << "testing EnforcePropertiesConstraints for "
//    << v1 << " and " << v2 << LENDL;
  bool result = true;

  // cannot use AnalysisGraph because it is const -> use AnalysisContent
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  LinguisticGraph* posgraph=anagraph->getGraph();

  //  SyntacticData* syntacticData = dynamic_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  VertexDataPropertyMap map =  get(vertex_data, *posgraph);
  MorphoSyntacticData* data1 = map[v1];
  MorphoSyntacticData* data2 = map[v2];

  std::vector<const Common::PropertyCode::PropertyAccessor*>::const_iterator
      categ = m_categories.begin(), categ_end = m_categories.end();
  for (; categ != categ_end; categ++)
  {
    std::set< LinguisticCode > categ1 = data1->allValues(**categ);
    std::set< LinguisticCode > categ2 = data2->allValues(**categ);
//    LDEBUG << "    on property " << (*categ)->getPropertyName() << ", there is " << categ1.size() << " and " << categ2.size() << " values" << LENDL;
    if (categ1.size()!=0 && categ2.size()!=0)
    {
      for (std::set< LinguisticCode >::iterator it=categ1.begin();it!=categ1.end();it++)
      {
        std::string str1 = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager((*categ)->getPropertyName()).getPropertySymbolicValue(*it);
//        LDEBUG << "    categ1 " << str1 << LENDL;
      }
      for (std::set< LinguisticCode >::iterator it=categ2.begin();it!=categ2.end();it++)
      {
        std::string str2 = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager((*categ)->getPropertyName()).getPropertySymbolicValue(*it);
//        LDEBUG << "    categ2 " << str2 << LENDL;
      }
      std::set< LinguisticCode > common;
      std::set_intersection(categ1.begin(), categ1.end(),
                            categ2.begin(), categ2.end(),
                            std::insert_iterator< std::set< LinguisticCode> >(common, common.end()));
      ExcludePropertyPredicate epp(*categ,common);
//      LDEBUG << "      sizes before erase: " << data1->size() << " / " << data2->size() << LENDL;
      data1->erase(remove_if(data1->begin(),data1->end(),epp),data1->end());
      data2->erase(remove_if(data2->begin(),data2->end(),epp),data2->end());
//      LDEBUG << "      sizes after  erase: " << data1->size() << " / " << data2->size() << LENDL;
      if (data1->empty() || data2->empty()) {
        result = false;
        break;
      }
    }
  }

  if (!result)
  {
    SAPLOGINIT;
    LINFO << "EnforcePropertiesConstraints: "
    << (result?"true":"false") << LENDL;
    LINFO << " but returns true: constraints checking "
    << "should have been done with agreement constraints" << LENDL;
  }
//  else
//  {
//    LDEBUG << "EnforcePropertiesConstraints: "
//    << (result?"true":"false") << LENDL;
//  }
  return true;
}


//----------------------------------------------------------------------
AddRelationInGraph::AddRelationInGraph(MediaId language,
                                       const LimaString& complement):
    ConstraintWithRelationComplement(language,complement)
{}

bool AddRelationInGraph::
operator()(const LinguisticAnalysisStructure::AnalysisGraph&,
           const LinguisticGraphVertex& src,
           const LinguisticGraphVertex& dest,
           AnalysisContent& analysis ) const
{
//   SAPLOGINIT;
//   LDEBUG << "adding new relation in the dependency graph" << LENDL;

  SyntacticData* syntacticData=
    static_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  try
  {
    syntacticData->addRelationNoChain(m_relation, src, dest);
  }
  catch (const std::exception& )
  {
    SAPLOGINIT;
    LERROR << "relation (" << m_relation << "," << src << ","
    << dest << ") not added" << LENDL;
    return false;
  }
  return true;
}

bool AddRelationInGraph::operator()(AnalysisContent& analysis ) const
{

//   SAPLOGINIT;
//   LDEBUG << "adding stored relations in the dependency graph" << LENDL;

  AnalysisGraph* anagraph=
    static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  SyntacticData* syntacticData=
    static_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  LinguisticGraphVertex src, dest;
  Common::MediaticData::SyntacticRelationId relation;

  boost::tie(src,dest,relation) = syntacticData->relation();
  uint64_t nbAdded(0);

  std::set<std::pair<LinguisticGraphVertex,Common::MediaticData::SyntacticRelationId> > relationsAdded;

  while (!((src == anagraph->firstVertex()) &&
           (dest == anagraph->lastVertex()) &&
           (relation == 0)))
  {

    if (relation == 0)
    {
      SAPLOGINIT;
      LERROR << "no type specified for relation between " << src
      << " and " << dest << ": ignored" << LENDL;
      boost::tie(src,dest,relation) = syntacticData->relation();
      continue;
    }

    try
    {
      syntacticData->addRelationNoChain(relation, src, dest);
      nbAdded++;
      relationsAdded.insert(std::make_pair(src,relation));
    }
    catch (const std::exception& )
    {
      SAPLOGINIT;
      LERROR << "relation (" << relation << "," << src << ","
      << dest << ") not added" << LENDL;
    }
    boost::tie(src,dest,relation) = syntacticData->relation();
  }

  // return true is some additions worked (even if some did not work)
  return (nbAdded>0);
}

//----------------------------------------------------------------------
ModifyRelationInGraph::ModifyRelationInGraph(MediaId language,
    const LimaString& complement):
    ConstraintWithRelationComplement(language,complement)
{}

bool ModifyRelationInGraph::operator()(const LinguisticAnalysisStructure::AnalysisGraph& ,
                                       const LinguisticGraphVertex& ,
                                       const LinguisticGraphVertex& ,
                                       AnalysisContent& /*ac*/) const
{
  SAPLOGINIT;
  LERROR << "invalid action ! modify relation in the dependency graph !" << LENDL;
  return true;
}

bool ModifyRelationInGraph::operator()(AnalysisContent& analysis) const
{

//   SAPLOGINIT;
//   LDEBUG << "modify stored relations in the dependency graph" << LENDL;

  AnalysisGraph* anagraph=
    static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  SyntacticData* syntacticData=
    static_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  LinguisticGraphVertex src, dest;
  Common::MediaticData::SyntacticRelationId relation;

  boost::tie(src,dest,relation) = syntacticData->relation();
  uint64_t nbModified(0);

  while (!((src == anagraph->firstVertex()) &&
           (dest == anagraph->lastVertex()) &&
           (relation == 0)))
  {

    if (relation == 0)
    {
      SAPLOGINIT;
      LERROR << "no type specified for relation between " << src
      << " and " << dest << ": ignored" << LENDL;
      boost::tie(src,dest,relation) = syntacticData->relation();
      continue;
    }

    try
    {
      syntacticData->modifyRelationNoChain(relation, src, dest, m_relation);
      nbModified++;
    }
    catch (const std::exception& )
    {
      SAPLOGINIT;
      LERROR << "relation (" << relation << "," << src << ","
      << dest << ") not modified" << LENDL;
    }
    boost::tie(src,dest,relation) = syntacticData->relation();
  }

  // return true is some additions worked (even if some did not work)
  return (nbModified>0);
}

//----------------------------------------------------------------------
ClearStoredRelations::ClearStoredRelations(MediaId language,
    const LimaString& complement):
    ConstraintFunction(language,complement)
{}

bool ClearStoredRelations::operator()(AnalysisContent& analysis) const
{
/*
  Critical function : comment logging messages
*/

//  SAPLOGINIT;
//  LDEBUG << "clearing stored relations" << LENDL;

  SyntacticData* syntacticData=
    static_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  syntacticData->clearStoredRelations();
  return true;
}

} // end namespace
} // end namespace
} // end namespace
