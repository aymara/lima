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

#include "coreferentAnnotation.h"
 
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/HomoSyntagmaticConstraints.h"
#include "linguisticProcessing/core/Automaton/basicConstraintFunctions.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"

#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"

#include <iostream>


using namespace boost;
using namespace std;
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;


namespace Lima
{
namespace LinguisticProcessing
{
namespace Coreferences
{


int DumpCoreferent::dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const
{
  PROCESSORSLOGINIT;
  try
  {
    ga.value<CoreferentAnnotation>().dump(os);
    return SUCCESS_ID;
  }
  catch (const boost::bad_any_cast& )
  {
    LERROR << "This annotation is not a CoreferentAnnotation ; nothing dumped";
    return UNKNOWN_ERROR;
  }
}

 /** general test functions */


bool CoreferentAnnotation::isIncludedInNounPhrase(
  const LinguisticGraph* g,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  const std::set< LinguisticCode >& inNpCategs,
  const Common::PropertyCode::PropertyAccessor* microAccessor) const
{
  // use PosGraph, should be only one categ
  MorphoSyntacticData* data = get(vertex_data, *g, m_morphVertex);
  if (data ==0 || data->empty()) { return false; };
  LinguisticCode categ=data->firstValue(*microAccessor);
  return (inNpCategs.find(categ)!=inNpCategs.end()||GovernedBy(language,utf8stdstring2limastring("COMPADV"))(*anagraph,m_morphVertex,ac));
}



LinguisticGraphVertex CoreferentAnnotation::npHeadVertex(
  const SyntacticData* sd,
  MediaId language,
  std::set<DependencyGraphVertex>* alreadyProcessed) const
{
  DependencyGraphVertex headNode = sd->depVertexForTokenVertex(m_morphVertex);
  alreadyProcessed->insert(headNode);
  CEdgeDepRelTypePropertyMap map = get(edge_deprel_type, *(sd-> dependencyGraph()));
  DependencyGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::out_edges(headNode, *(sd->dependencyGraph()));

  if (it == it_end) return sd->tokenVertexForDepVertex(headNode);
  else
  {
    for (; it != it_end; it++)
    {
      SyntacticRelationId relationId(static_cast<SyntacticRelationId>(map[*it]));
      std::string relationName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationName(relationId);
      
      
      if (static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).isACompoundRel(relationName)||relationName=="MOD_N")
      {
        headNode = target(*it, *(sd->dependencyGraph())) ;
        CoreferentAnnotation ca(m_id, sd->tokenVertexForDepVertex(headNode));
        if (alreadyProcessed->find(headNode)==alreadyProcessed->end())
          return ca.npHeadVertex(sd,language,alreadyProcessed);
      }
    }
    return m_morphVertex;
  }
}



bool CoreferentAnnotation::isTaggedAsOneOfThese(
  const LinguisticGraph* g,
  const std::set< LinguisticCode >& categs,
  const Common::PropertyCode::PropertyAccessor* microAccessor) const
{
  // use PosGraph, should be only one categ
  MorphoSyntacticData* data = get(vertex_data, *g, m_morphVertex);
  if (data ==0 || data->empty()) { return false; };
  LinguisticCode categ=data->firstValue(*microAccessor);
  return (categs.find(categ)!=categs.end());
}





  /** test functions for the initial classification */

bool CoreferentAnnotation::isPronoun(
  const LinguisticGraph* g,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const LinguisticCode& L_PRON) const
{
  MorphoSyntacticData* data = get(vertex_data, *g, m_morphVertex);
  if (data ==0 || data->empty()) { return false; };
  return (data->firstValue(*macroAccessor) == L_PRON);
}


bool CoreferentAnnotation::isDefinite(
  const SyntacticAnalysis::SyntacticData* sd,
  const std::string& definiteRel,
  const std::set< LinguisticCode >& definiteCategs,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
                                      MediaId language) const
{
  DependencyGraphVertex dv = sd->depVertexForTokenVertex(m_morphVertex);
  CEdgeDepRelTypePropertyMap map = get(edge_deprel_type, *(sd-> dependencyGraph()));
  CVertexDataPropertyMap dataMap = get(vertex_data, *(sd-> graph()));
  DependencyGraphInEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::in_edges(dv, *(sd->dependencyGraph()));
  for (; it != it_end; it++)
  {
    SyntacticRelationId relationId(static_cast<SyntacticRelationId>(map[*it]));
    std::string relationName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationName(relationId);

    if (relationName == definiteRel)
    {
      MorphoSyntacticData* data = dataMap[sd->tokenVertexForDepVertex(source(*it, *(sd->dependencyGraph())))];
      if (data ==0 || data->empty()) { return false; };
      return (definiteCategs.find(data->firstValue(*microAccessor)) != definiteCategs.end());
    }
  }
  return false;
}



bool CoreferentAnnotation::isPleonastic(
  const SyntacticAnalysis::SyntacticData* sd,
  const std::string& pleonRel,
  MediaId language) const
{
//   COREFSOLVERLOGINIT; 
  DependencyGraphVertex dv = sd->depVertexForTokenVertex(m_morphVertex);
  CEdgeDepRelTypePropertyMap map = get(edge_deprel_type, *(sd-> dependencyGraph()));
  DependencyGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::out_edges(dv, *(sd->dependencyGraph()));
  for (; it != it_end; it++)
  {
    SyntacticRelationId relationId(static_cast<SyntacticRelationId>(map[*it]));
    std::string relationName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationName(relationId);

    if (relationName == pleonRel)
    {
      return true;
    }
  }
  return false;
}


std::string CoreferentAnnotation::referentType(
  const SyntacticAnalysis::SyntacticData* sd,
  const LinguisticGraph* g,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const std::map<std::string, std::deque<std::string> >& relLocalDef,
  const std::set< LinguisticCode >& definiteCategs,
  const std::set< LinguisticCode >& reflexiveReciprocalCategs,
  const std::set< LinguisticCode >& undefPronounsCategs,
  const std::set< LinguisticCode >& possPronounsCategs,
  const Common::PropertyCode::PropertyAccessor* personAccessor,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  MediaId language) const
{
//   COREFSOLVERLOGINIT;
  LinguisticCode L_PRON = (*tagLocalDef.find("PronMacroCategory")).second;
  std::string pleonRel = *(*relLocalDef.find("PleonasticRelation")).second.begin();
  std::string defRel = *(*relLocalDef.find("DefiniteRelation")).second.begin();
  if (isPronoun(g,macroAccessor, L_PRON))
  {
    if (isTaggedAsOneOfThese(g,reflexiveReciprocalCategs,microAccessor))
    {
      return "reflPron";
    }
    if (isN3PPronoun(personAccessor, anagraph))
    {
      return "N3Ppron";
    }
    if (isTaggedAsOneOfThese(g,undefPronounsCategs,microAccessor))
    {
      return "undefPron";
    }
    if (isTaggedAsOneOfThese(g,possPronounsCategs,microAccessor))
    {
      return "possPron";
    }
    Token* token = get(vertex_token, *anagraph->getGraph(), m_morphVertex);
    if (token != 0 && (limastring2utf8stdstring(token->stringForm())=="on"|| limastring2utf8stdstring(token->stringForm())=="-t-on"||limastring2utf8stdstring(token->stringForm())=="On"))
    {
      return "on";
    }
    return isPleonastic(sd,pleonRel,language)?"pleon":"other";
  }
  else
  {
    return isDefinite(sd,defRel,definiteCategs,microAccessor,language)?"def":"undef";
  }
}
// is non-3rd-Person Pronoun
bool CoreferentAnnotation::isN3PPronoun(
  const Common::PropertyCode::PropertyAccessor* personAccessor,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph) const
{
  MorphoSyntacticData* data = get(vertex_data, *anagraph->getGraph(), m_morphVertex);
    return(data->firstValue(*personAccessor)!=static_cast<LinguisticCode>(48));
}

  /** test functions for the salience weighting */

bool CoreferentAnnotation::beginWithColon(
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  LinguisticGraphVertex& beginSentence) const
{
  Token* token = get(vertex_token, *anagraph->getGraph(), beginSentence);
  return (token != 0 && limastring2utf8stdstring(token->stringForm())==":");
}

bool CoreferentAnnotation::endWithColon(
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  LinguisticGraphVertex& endSentence) const
{
  Token* token = get(vertex_token, *anagraph->getGraph(), endSentence);
  return (token != 0 && limastring2utf8stdstring(token->stringForm())==":");
}

bool CoreferentAnnotation::isInAppos(
  const SyntacticData* sd,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  MediaId language,
  std::set<DependencyGraphVertex>* alreadyProcessed) const
{
  alreadyProcessed->insert(m_morphVertex);
  if (GovernorOf(language,utf8stdstring2limastring("APPOS"))(*anagraph,m_morphVertex,ac))
    return true;
  else
  {
    LinguisticGraphVertex head = npHeadVertex(sd,language,new std::set<DependencyGraphVertex>());
    if (alreadyProcessed->find(head)==alreadyProcessed->end())
      return CoreferentAnnotation(head,0).isInAppos(sd,anagraph,ac,language,alreadyProcessed);
    else return false;
  }
}

bool CoreferentAnnotation::isInQuantA(
  const SyntacticData* sd,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph) const
{
  DependencyGraphInEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::in_edges(m_morphVertex, *(sd->dependencyGraph()));
  for (; it != it_end; it++)
  {
    Token* token = get(vertex_token, *anagraph->getGraph(),source(*it, *(sd->dependencyGraph())));
    if (token!= 0 && (limastring2utf8stdstring(token->stringForm()) == "quant à"
    ||limastring2utf8stdstring(token->stringForm()) == "Quant à"
    ||limastring2utf8stdstring(token->stringForm()) == "quant aux"
    ||limastring2utf8stdstring(token->stringForm()) == "Quant aux"))
    {
      return true;
    }
  }
  return false;
}

bool CoreferentAnnotation::isInSubordinate(
  const SyntacticData* sd,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  set<LinguisticGraphVertex>* alreadyProcessed) const
{
  
  DependencyGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::out_edges(sd->depVertexForTokenVertex(m_morphVertex), *(sd->dependencyGraph()));
  if (it == it_end) return false;
  //else
  LinguisticGraphVertex qv(0);
  // for each outer relation of *this
  for (; it != it_end; it++)
  {
    qv = target(*it,*anagraph->getGraph());
    if  (CoreferentAnnotation(0,qv).isVerb(macroAccessor,tagLocalDef, anagraph))
    {
      // if verb is governed by a "COMPL" relation( = conjonctive subordinate)
      // if verb is governing a "MOD_N" relation( = relative subordinate)
      if(/*GovernedBy(language,utf8stdstring2limastring("COMPL"))(*anagraph,qv,ac)||*/GovernorOf(language,utf8stdstring2limastring("MOD_N"))(*anagraph,qv,ac))
      {
        return true;
      }
      else if (alreadyProcessed->find(qv)==alreadyProcessed->end())
      {
        alreadyProcessed->insert(qv);
        return CoreferentAnnotation(0,qv).isInSubordinate(sd, macroAccessor,tagLocalDef, language, anagraph, ac, alreadyProcessed);
      }
    }
  }  
  if (qv!=0 && alreadyProcessed->find(qv)==alreadyProcessed->end())
  {
    alreadyProcessed->insert(qv);
    return CoreferentAnnotation(0,qv).isInSubordinate(sd, macroAccessor,tagLocalDef, language, anagraph, ac, alreadyProcessed);
  }
  //else
  return false;
}


/** returns true if *this governs a macroDependencyRelation (cf. parameters) 
difference with GovernorOf() is:
- a macroDependencyRelation is a macro syntactic dependency 
- test is for a specific dependency but for any target vertex
*/
bool CoreferentAnnotation::isFunctionMasterOf(
  const SyntacticData* sd,
  const std::deque<std::string>& macroDependencyRelation,
  MediaId language) const 
{
  CEdgeDepRelTypePropertyMap map = get(edge_deprel_type, *(sd-> dependencyGraph()));
  DependencyGraphOutEdgeIt it, it_end;

  boost::tie(it, it_end) = boost::out_edges(sd->depVertexForTokenVertex(m_morphVertex), *(sd->dependencyGraph()));
  if (it == it_end) return false;
  else
  {
    // for each outer relation of anaphora
    for (; it != it_end; it++)
    {
      SyntacticRelationId relationId(static_cast<SyntacticRelationId>(map[*it]));
      std::string relationName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationName(relationId);
      
      std::deque<std::string>::const_iterator it2;
      for (it2 = macroDependencyRelation.begin( );
            it2 != macroDependencyRelation.end( ); 
            it2++ )
      {
        // check if the outer relation is func relation
        if (relationName == *it2)
        {
          return true;
        }
        else if (relationName == "COORD2")
        {
//           CEdgeDepRelTypePropertyMap map2 = get(edge_deprel_type, *(sd-> dependencyGraph()));
          DependencyGraphOutEdgeIt it3, it3_end;
          boost::tie(it3, it3_end) = boost::out_edges(target(*it,*sd-> dependencyGraph()), *(sd->dependencyGraph()));
          if (it3 == it3_end) return false;
          // for each outer relation of coordination conjunction
          for (; it3 != it3_end; it3++)
          {
            CoreferentAnnotation ca(0,target(*it3,*sd-> dependencyGraph()));
            if (ca.isFunctionMasterOf(sd,macroDependencyRelation, language)) return true;
          }
        }
      }
    }
  }
  return false;
}

  /** test functions for the syntactic filter and the reflexive binding algorithm */

bool CoreferentAnnotation::isVerb(
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph
) const
{
//   COREFSOLVERLOGINIT; 
  MorphoSyntacticData* data = get(vertex_data, *anagraph->getGraph(), m_morphVertex);
  if (data ==0 || data->empty()) { return false; };
  LinguisticCode L_VERB = (*tagLocalDef.find("VerbMacroCategory")).second;
  
  if(data->firstValue(*macroAccessor)==L_VERB) 
  {
    return true;
  }
  return false;
}

bool CoreferentAnnotation::isConjCoord(
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  const LinguisticCode& conjCoord) const
{
//   COREFSOLVERLOGINIT; 
  MorphoSyntacticData* data = get(vertex_data, *anagraph->getGraph(), m_morphVertex);
  if (data ==0 || data->empty()) { return false; }; 
  if(data->firstValue(*microAccessor)==conjCoord) 
  {
    return true;
  }
  return false;
}


bool CoreferentAnnotation::isGovernedByMasculineCoordinate(
  const SyntacticData* sd,
  const Common::PropertyCode::PropertyAccessor* genderAccessor,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const
  {    
//  TimeUtils::logElapsedTime(limastring2utf8stdstring(token->stringForm()));
  DependencyGraph g = *(sd->dependencyGraph());
  if (GovernedBy(language, utf8stdstring2limastring("COORD1"))(*anagraph,sd->depVertexForTokenVertex(m_morphVertex),ac))
  {
    DependencyGraphInEdgeIt it, it_end;
    boost::tie(it, it_end) = boost::in_edges(sd->depVertexForTokenVertex(m_morphVertex), *(sd->dependencyGraph()));
    if (it==it_end) return false;
    if (GovernedBy(language, utf8stdstring2limastring("COORD2"))(*anagraph,source(*it, *anagraph->getGraph()),ac))
    {
      DependencyGraphInEdgeIt it2, it2_end;
      boost::tie(it2, it2_end) = boost::in_edges(source(*it, g),g);
      if (it2==it2_end) return false;
      MorphoSyntacticData* data1 = get(vertex_data, *anagraph->getGraph(), source(*it2, g));
      if (data1 ==0 || data1->empty()) { return false; };
      for (MorphoSyntacticData::iterator msdataIt = data1->begin(); msdataIt!=data1->end(); msdataIt++)
      {
        if (genderAccessor->readValue(msdataIt->properties)==1)
          return true;
        else if (CoreferentAnnotation(0, sd->tokenVertexForDepVertex(source(*it2, *(sd->dependencyGraph())))).isGovernedByMasculineCoordinate(sd,genderAccessor,language,anagraph,ac)) 
          return true;
      }
    }
  }
  else if (GovernedBy(language, utf8stdstring2limastring("APPOS"))(*anagraph,sd->depVertexForTokenVertex(m_morphVertex),ac))
  {
    DependencyGraphInEdgeIt it, it_end;
    boost::tie(it, it_end) = boost::in_edges(sd->depVertexForTokenVertex(m_morphVertex), *(sd->dependencyGraph()));
    if (it==it_end) return false;
    MorphoSyntacticData* data1 = get(vertex_data, *anagraph->getGraph(), source(*it, g));
    if (data1 ==0 || data1->empty()) { return false; };
    for (MorphoSyntacticData::iterator msdataIt = data1->begin(); msdataIt!=data1->end(); msdataIt++)
    {
      if (genderAccessor->readValue(msdataIt->properties)==1)
        return true;
      else return CoreferentAnnotation(0, sd->tokenVertexForDepVertex(source(*it, *(sd->dependencyGraph())))).isGovernedByMasculineCoordinate(sd,genderAccessor,language,anagraph,ac);
    }
  }
  else
    return false;
  return false;
}

// P and N have incompatible agreement features. (gender, number, person)
bool CoreferentAnnotation::isAgreementCompatibleWith(
  const CoreferentAnnotation& ca,
  const SyntacticData* sd,
  const Common::PropertyCode::PropertyAccessor* genderAccessor,
  const Common::PropertyCode::PropertyAccessor* personAccessor,
  const Common::PropertyCode::PropertyAccessor* numberAccessor,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
        AnalysisContent& ac) const
  {
//     COREFSOLVERLOGINIT; 
//  TimeUtils::logElapsedTime("init Ac");
//  TimeUtils::updateCurrentTime();
    //   LDEBUG << "isArgumentCompatibleWith";
    MorphoSyntacticData* data1 = get(vertex_data, *anagraph->getGraph(), m_morphVertex);
    if (data1 ==0 || data1->empty()) { return false; };
    bool isPlural = false;
    for (MorphoSyntacticData::iterator it = data1->begin(); it!=data1->end(); it++)
    {
      if (numberAccessor->readValue(it->properties)==8)
      {
        isPlural = true;
        break;
      }
    }
    bool isMascPlural = false;
    for (MorphoSyntacticData::iterator it = data1->begin(); it!=data1->end(); it++)
    {
      if (numberAccessor->readValue(it->properties)==8 && genderAccessor->readValue(it->properties)==1)
      {
        isMascPlural = true;
        break;
      }
    }
    MorphoSyntacticData* data2 = get(vertex_data, *anagraph->getGraph(), ca.m_morphVertex);
    if (data2 ==0 || data2->empty()) { return false; };
//  TimeUtils::logElapsedTime("before A");
//       TimeUtils::updateCurrentTime();
    bool genderAgreement = Automaton::GenderAgreement(language)(*anagraph, m_morphVertex, ca.m_morphVertex,ac)
    || (isMascPlural
        && ca.isGovernedByMasculineCoordinate(sd,genderAccessor,language,anagraph,ac));
//  TimeUtils::logElapsedTime("GA");
//       TimeUtils::updateCurrentTime();
    bool NumberAgreement = Automaton::NumberAgreement(language)(*anagraph, m_morphVertex, ca.m_morphVertex,ac)
      || (isPlural 
          && 
            (GovernedBy(language, utf8stdstring2limastring("COORD1"))(*anagraph,sd->depVertexForTokenVertex(ca.m_morphVertex),ac)
            ||
            GovernedBy(language, utf8stdstring2limastring("APPOS"))(*anagraph,sd->depVertexForTokenVertex(ca.m_morphVertex),ac))
          );
//  TimeUtils::logElapsedTime("NA");
//       TimeUtils::updateCurrentTime();
    bool PersonAgreement = 
    (data1->firstValue(*personAccessor)==data2->firstValue(*personAccessor) ||
    // anaphora = pronom classifi��la 3e personne, candidate = common NP non-classified
    (data1->firstValue(*personAccessor)==48 && data2->firstValue(*personAccessor)==0));
//  TimeUtils::logElapsedTime("PA");
//  TimeUtils::updateCurrentTime();
    return (genderAgreement && NumberAgreement && PersonAgreement);
  }

// P is in the Argument Domain of N <=> P and N are both argument of the same head
bool CoreferentAnnotation::isInTheArgumentDomainOf(
  const CoreferentAnnotation& ca,
  const SyntacticData* sd,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const LinguisticCode& conjCoord,
  set<LinguisticGraphVertex>* alreadyProcessed) const
{
//   COREFSOLVERLOGINIT; 
  alreadyProcessed->insert(m_morphVertex);
//   LDEBUG << "isInTheArgumentDomainOf" ;
//   LDEBUG << "check: " << m_morphVertex << "for: " << ca.m_morphVertex;
  DependencyGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::out_edges(sd->depVertexForTokenVertex(m_morphVertex), *(sd->dependencyGraph()));

  if (it == it_end)
  {
     return false;
  }
//   else
  // for each head of anaphora
  for (; it != it_end; it++)
  {

    DependencyGraphVertex headNode = target(*it, *(sd->dependencyGraph()));
    // check if this head is also head of candidate (except for the anaphora which are "circumstantial" adjuncts)
    if ((
      // laisse les compléments circonstanciels être anaphores d'un antécédent argument 
        !SecondUngovernedBy(language, utf8stdstring2limastring("COD_V"))(*anagraph, npHeadVertex(sd,language,new set<DependencyGraphVertex>()),headNode,ac) 
      ||
        !SecondUngovernedBy(language, utf8stdstring2limastring("CodPrev"))(*anagraph, npHeadVertex(sd,language,new set<DependencyGraphVertex>()),headNode,ac) 
      ||
        !SecondUngovernedBy(language, utf8stdstring2limastring("CoiPrev"))(*anagraph, npHeadVertex(sd,language,new set<DependencyGraphVertex>()),headNode,ac) 
      ||
        !SecondUngovernedBy(language, utf8stdstring2limastring("SUJ_V"))(*anagraph, npHeadVertex(sd,language,new set<DependencyGraphVertex>()),headNode,ac) 
      ||
        !SecondUngovernedBy(language, utf8stdstring2limastring("SujInv"))(*anagraph, npHeadVertex(sd,language,new set<DependencyGraphVertex>()),headNode,ac) 
      )
    && 
      !SecondUngovernedBy(language, LimaString())(*anagraph, ca.npHeadVertex(sd,language,new set<DependencyGraphVertex>()),headNode,ac)
)
    {
      return true;
    }
//     else
    {
      CoreferentAnnotation anaHead(0,sd->tokenVertexForDepVertex(headNode));
      if (
      // candidat dirige une quelconque dépendance
      GovernorOf(language,LimaString())(*anagraph,sd->depVertexForTokenVertex(ca.m_morphVertex),ac)
      && (  // target(ana) est verbe non subordonné
//             language==
//             &&
            ( anaHead.isVerb(macroAccessor,tagLocalDef, anagraph)
            && (
              !GovernedBy(language,utf8stdstring2limastring("COMPL"))(*anagraph,headNode,ac)
            &&
              !GovernedBy(language,utf8stdstring2limastring("SUJ_V_REL"))(*anagraph,headNode,ac)
//             ||
//               !GovernedBy(language,utf8stdstring2limastring("CodPrev"))(*anagraph,headNode,ac)
            ) )
          ||
            // target(ana) est conjonction de coordination et a un COORD1
            (anaHead.isConjCoord(microAccessor, anagraph,conjCoord) && GovernorOf(language,LimaString())(*anagraph,headNode,ac))
          )
       &&
       // pas encore traité
       alreadyProcessed->find(anaHead.m_morphVertex)==alreadyProcessed->end())
      {
        return  anaHead.isInTheArgumentDomainOf(CoreferentAnnotation(0,ca.npHeadVertex(sd,language, new  set<DependencyGraphVertex>())), sd, language, anagraph, ac, macroAccessor, microAccessor, tagLocalDef, conjCoord, alreadyProcessed)
       /* ||
        ca.isInTheArgumentDomainOf(*this, sd, language, anagraph, ac, macroAccessor, microAccessor, tagLocalDef, conjCoord, alreadyProcessed)*/;
      }
    }
  }
//   TimeUtils::logElapsedTime("fin sf2");
//   TimeUtils::updateCurrentTime();
  return false;
}

// P is in the Argument Domain of N <=> P and N are both argument of the same head
bool CoreferentAnnotation::isInTheArgumentDomainOf2(
  const CoreferentAnnotation& ca,
  const SyntacticData* sd,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const LinguisticCode& conjCoord,
  set<LinguisticGraphVertex>* alreadyProcessed) const
{
//   COREFSOLVERLOGINIT; 
// cerr << "trace" <<endl;
  alreadyProcessed->insert(m_morphVertex);
//   LDEBUG << "isInTheArgumentDomainOf" ;
//   LDEBUG << "check: " << m_morphVertex << "for: " << ca.m_morphVertex;
  DependencyGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::out_edges(sd->depVertexForTokenVertex(m_morphVertex), *(sd->dependencyGraph()));
  if (it == it_end)
  {
//      TimeUtils::logElapsedTime("fin sf2");
//      TimeUtils::updateCurrentTime();
     return false;
  }
//   else
  // for each head of anaphora
  for (; it != it_end; it++)
  {
// cerr << "trace1" <<endl;
    DependencyGraphVertex headNode = target(*it, *(sd->dependencyGraph()));
    // check if this head is also head of candidate 
    if (     
      !SecondUngovernedBy(language, LimaString())(*anagraph, ca.npHeadVertex(sd,language,new set<DependencyGraphVertex>()),headNode,ac)
    ) 
    {
      /*TimeUtils::logElapsedTime("fin sf2");
      TimeUtils::updateCurrentTime();
      */return true;
    }
//     else
    {
// cerr << "trace2" <<endl;
      CoreferentAnnotation anaHead(0,sd->tokenVertexForDepVertex(headNode));
      if (
      // candidat dirige une quelconque dépendance
      GovernorOf(language,LimaString())(*anagraph,sd->depVertexForTokenVertex(ca.m_morphVertex),ac)
       &&
       // pas encore traité
       alreadyProcessed->find(anaHead.m_morphVertex)==alreadyProcessed->end())
      {
        return  anaHead.isInTheArgumentDomainOf(CoreferentAnnotation(0,ca.npHeadVertex(sd,language, new  set<DependencyGraphVertex>())), sd, language, anagraph, ac, macroAccessor, microAccessor, tagLocalDef, conjCoord, alreadyProcessed)
       /* ||
        ca.isInTheArgumentDomainOf(*this, sd, language, anagraph, ac, macroAccessor, microAccessor, tagLocalDef, conjCoord, alreadyProcessed)*/;
      }
    }
  }
//   TimeUtils::logElapsedTime("fin sf2");
//   TimeUtils::updateCurrentTime();
  return false;
}

// P is in the Adjunct Domain of N <=> N is an argument of a head H, P is the object of a preposition PREP, PREP is an adjunct of H
bool CoreferentAnnotation::isInTheAdjunctDomainOf(
  const CoreferentAnnotation& ca,
  const SyntacticAnalysis::SyntacticData* sd,
  const LinguisticGraph* graph,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const std::map<std::string, std::deque<std::string> >& relLocalDef,
                                                  MediaId language) const
{
//   COREFSOLVERLOGINIT; 
//   LDEBUG << "isInTheAdjunctDomainOf" ;
  // for each HEAD of candidate
  DependencyGraphOutEdgeIt it, it_end;
  DependencyGraphVertex headNode = sd->depVertexForTokenVertex(ca.m_morphVertex);
  boost::tie(it, it_end) = boost::out_edges(headNode, *(sd->dependencyGraph()));
  if (it == it_end) 
  {
     return false;
  }
  // else
  for (; it != it_end; it++)
  {
    headNode = target(*it, *(sd->dependencyGraph()));
    if (isInThePrepAdjunctNP(headNode, sd, graph, macroAccessor, tagLocalDef, relLocalDef,language))
    {
      return true;
    }
  }
  return false;
}

// P is the object of a preposition PREP, 
// and PREP is an adjunct of Q
bool CoreferentAnnotation::isInThePrepAdjunctNP(
  const DependencyGraphVertex& qv,
  const SyntacticAnalysis::SyntacticData* sd,
  const LinguisticGraph* graph,
    //const Common::PropertyCode::PropertyManager& microManager,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const std::map<std::string, std::deque<std::string> >& relLocalDef,
                                                MediaId language) const
{
//   COREFSOLVERLOGINIT; 
//  LDEBUG << "isInThePrepAdjunctOf";
    MorphoSyntacticData* data = get(vertex_data, *graph, qv);
    if (data ==0 || data->empty()) { return false; };
    LinguisticCode L_VERB = (*tagLocalDef.find("VerbMacroCategory")).second;
    // if  head node is not a verbal form 
    if(data->firstValue(*macroAccessor)==L_VERB) 
    {
      return false;
    }
    //else
    // for each PREP of anaphora 
    // <=> for each inner relation and if it is a PrepRelation

//     CEdgeDepRelTypePropertyMap map = get(edge_deprel_type, *(sd-> dependencyGraph()));
    DependencyGraphInEdgeIt it2, it2_end;
    boost::tie(it2, it2_end) = boost::in_edges(sd->depVertexForTokenVertex(m_morphVertex), *(sd->dependencyGraph()));
    if (it2==it2_end) return false;
    CoreferentAnnotation catmp(0, sd->tokenVertexForDepVertex(source(*it2, *(sd->dependencyGraph()))));
    if(catmp.isFunctionMasterOf(sd,(*relLocalDef.find("PrepRelation")).second,language))
    {
//    // test if PREP is an adjunct of HEAD
      if (isFunctionMasterOf(sd,(*relLocalDef.find("AdjunctRelation")).second,language))
        return true; 
    }
    return false;
}

// P is an argument of a head H, N is not a pronoun, and N is contained in H.
bool CoreferentAnnotation::sf4(
  const CoreferentAnnotation& ca, 
  const SyntacticData* sd,
  //const LinguisticGraph* g,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const LinguisticCode& L_PRON,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const
{
//   COREFSOLVERLOGINIT; 
//   LDEBUG << "sf4" <<  LENDL;
  const LinguisticGraph* g = anagraph->getGraph();
  if (ca.isPronoun(g, macroAccessor, L_PRON))
  {
    return false;
  }
  //else
  DependencyGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::out_edges(sd->depVertexForTokenVertex(m_morphVertex), *(sd->dependencyGraph()));
  if (it == it_end)
  {
    return false;
  }
  //else
  // for each head of anaphora
  for (; it != it_end; it++)
  {
    DependencyGraphVertex headNode = target(*it, *(sd->dependencyGraph()));
    if (ca.isContainedIn(headNode, language, anagraph, ac))
    {
      return true;
    } 
  }
  return false;
}

// P is in the NP domain of N 
// <=>
// N is a determiner of a noun Q, and:
//    (i) P is an argument of Q.
// or (ii) P is an object of a preposition PREP and PREP is an adjunct of Q.
bool CoreferentAnnotation::isInTheNpDomainOf(
  const CoreferentAnnotation& ca,
  const SyntacticData* sd,
  const Common::PropertyCode::PropertyAccessor* /*macroAccessor*/,
  const std::map<std::string,LinguisticCode>& /*tagLocalDef*/,
  const std::map<std::string,std::deque<std::string> >& /*relLocalDef*/,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* /*anagraph*/,
  AnalysisContent& /*ac*/
  ) const
{
//   COREFSOLVERLOGINIT; 
//   LDEBUG << "isInTheNpDomainOf";
//   DependencyGraphVertex* qv = new DependencyGraphVertex();
//   if (ca.isDeterminer(qv,sd, relLocalDef, language, anagraph, ac))
//   {  
//       //  if this is an argument of qv, return true.
//       DependencyGraphInEdgeIt itQ, itQ_end;
//       boost::tie(itQ, itQ_end) = boost::in_edges(*qv, *(sd->dependencyGraph()));
//       for (;itQ!=itQ_end; itQ++)
//       {  
//         DependencyGraphVertex pv = source(*itQ, *(sd->dependencyGraph())) ;
//         if (m_morphVertex==sd->tokenVertexForDepVertex(pv))
//    return true;
//       }
//       // else if this is an object of a preposition PREP and PREP is an adjunct of qv, return true. 
//       if (isInThePrepAdjunctNP(*qv,sd, anagraph->getGraph(), macroAccessor, tagLocalDef, relLocalDef))
//         return true;
//     
//   } 
return ca.npHeadVertex(sd, language, new set<DependencyGraphVertex>())==npHeadVertex(sd, language, new set<DependencyGraphVertex>());
}

// P is contained in a phrase Q <=> 
//    (i) P is either an argument or an adjunct of Q (ie. P is immediately contained in Q.
// or (ii) P is immediately contained in some phrase R, and R is contained in Q.
bool CoreferentAnnotation::isContainedIn(
  const DependencyGraphVertex& dv,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const
{
//   COREFSOLVERLOGINIT; 
//   LDEBUG << "isContainedIn";
  return (!SecondUngovernedBy(language,
                      LimaString())(*anagraph, dv, m_morphVertex ,ac));
}


// !! test if *this is determiner of ANY which is returned by the pointer qv
bool CoreferentAnnotation::isDeterminer(
  DependencyGraphVertex* qv,
  const SyntacticData* sd,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const
{
//   COREFSOLVERLOGINIT; 
//   LDEBUG << "isDeterminer";
  std::deque<std::string> detRels = (*relLocalDef.find("NPDeterminerRelation")).second;
  for (std::deque<std::string>::iterator itDet= detRels.begin(); itDet != detRels.end(); itDet++)
  {
    DependencyGraphOutEdgeIt itN, itN_end;
    boost::tie(itN, itN_end) = boost::out_edges(sd->depVertexForTokenVertex(this->m_morphVertex), *(sd->dependencyGraph()));
    for (;itN!=itN_end; itN++)
    {
      *qv = target(*itN, *(sd->dependencyGraph())) ;
      bool res = !SecondUngovernedBy(language,
                      utf8stdstring2limastring(*itDet))(*anagraph, m_morphVertex,*qv,ac);
      return (res);
    }
  }
  return false;
}


// P is a determiner of a noun Q, and N is contained in Q.
bool CoreferentAnnotation::sf6(
  const CoreferentAnnotation& ca,
  const SyntacticData* sd,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const
{
//   COREFSOLVERLOGINIT; 
//   LDEBUG << "sf6";
  DependencyGraphVertex* qv = new DependencyGraphVertex();
// if (this->isDeterminer(qv,sd, relLocalDef, language, anagraph, ac))
//  if (ca.isContainedIn(*qv, language, anagraph, ac))
//  return true;
// return false;
  return (this->isDeterminer(qv,sd, relLocalDef, language, anagraph, ac)     && ca.isContainedIn(*qv, language, anagraph, ac));
}

// N is an argument of a verb V,
// there is an NP Q in the argument domain of N such that Q has no noun determiner,
// and
//    (i) A is an argument of Q.
// or (ii) A is an argument of a preposition PREP and PREP is an adjunct of Q.
bool CoreferentAnnotation::aba4(
  const CoreferentAnnotation& ca,
  const SyntacticData* sd,
  const std::map<std::string,LinguisticCode >& tagLocalDef,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  MediaId language,
  const std::set< LinguisticCode >& inNpCategs,
  LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  const LinguisticCode& conjCoord,
  std::deque<Vertices>* npCandidates) const
{
//   Token* token1 = get(vertex_token, *anagraph->getGraph(), m_morphVertex);
//   Token* token2 = get(vertex_token, *anagraph->getGraph(), ca.m_morphVertex);
//   if (token1!=0)
//    TimeUtils::logElapsedTime("token 1 : " + limastring2utf8stdstring(token1->stringForm()));
//   if (token2!=0)
//    TimeUtils::logElapsedTime("token 2 : " + limastring2utf8stdstring(token2->stringForm()));
//  TimeUtils::updateCurrentTime();
//   LinguisticGraph* graph = anagraph->getGraph();
  std::deque<std::string> detRels = (*relLocalDef.find("NPDeterminerRelation")).second;
  std::deque<std::string> adjunctRels = (*relLocalDef.find("AdjunctRelation")).second;
  LinguisticCode L_VERB = (*tagLocalDef.find("VerbMacroCategory")).second;
  DependencyGraphOutEdgeIt itN, itN_end;
  boost::tie(itN, itN_end) = boost::out_edges(ca.m_morphVertex, *(sd->dependencyGraph()));
  // for each token dv of which N is argument
//  TimeUtils::logElapsedTime("before boucle");
//  TimeUtils::updateCurrentTime();
  for (;itN!=itN_end; itN++)
  {
    DependencyGraphVertex dv = target(*itN, *(sd->dependencyGraph())) ;
    MorphoSyntacticData* data = get(vertex_data, *anagraph->getGraph(), sd->tokenVertexForDepVertex(dv));
    if (data ==0 || data->empty()) { continue; };
    // if dv is a verb
    if (data->firstValue(*macroAccessor) == L_VERB)
    {
      // for each token Q which is NP
//       LinguisticGraphVertexIt itg, itg_end;
//       boost::tie(itg, itg_end) = vertices(*anagraph->getGraph());
//  TimeUtils::logElapsedTime("before second boucle");
//  TimeUtils::updateCurrentTime();
      for (std::deque<Vertices>::iterator itm = npCandidates->begin();
         itm != npCandidates->end( ); 
         itm++)
      {
        for (Vertices::iterator candidateItr = (*itm).begin( );
             candidateItr != (*itm).end( ); 
             candidateItr++ )
        {
          if ((*candidateItr)->isIncludedInNounPhrase(anagraph->getGraph(), language, anagraph, ac, inNpCategs, microAccessor) )
          {
            // if Q is in the argument domain of N
            // and if Q has no determiner
            if ((*candidateItr)->isInTheArgumentDomainOf(*this,sd, language, anagraph, ac, macroAccessor, microAccessor, tagLocalDef, conjCoord, new set<LinguisticGraphVertex>()))
            {
              bool hasDeterminer = false;
//   TimeUtils::logElapsedTime("before third boucle");
//  TimeUtils::updateCurrentTime();
              for (std::deque<std::string>::iterator itDet= detRels.begin(); itDet != detRels.end(); itDet++)
              {
                if(GovernedBy(language,
                      utf8stdstring2limastring(*itDet))(*anagraph, (*candidateItr)->m_morphVertex,ac))
                  hasDeterminer = true;
              }
//  TimeUtils::logElapsedTime("end third boucle");
//  TimeUtils::updateCurrentTime();
              if (!hasDeterminer)
              {
                // if A is an argument of Q, return true
                if (!SecondUngovernedBy(language,
                        LimaString())(*anagraph, m_morphVertex,(*candidateItr)->m_morphVertex,ac))
                {
//                    TimeUtils::logElapsedTime("end aba4");
//  TimeUtils::updateCurrentTime();
                  return true;
                }
//               // if A is an argument of a preposition PREP and PREP is an adjunct of Q, return true
//               DependencyGraphOutEdgeIt itA, itA_end;
//               boost::tie(itA, itA_end) = boost::out_edges(m_morphVertex, *(sd->dependencyGraph()));
//               // for each token dv of which N is argument
//               for (;itA!=itA_end; itA++)
//               {  
//                 DependencyGraphVertex dv = target(*itN, *(sd->dependencyGraph())) ;
//                 MorphoSyntacticData* data = get(vertex_data, *graph, sd->tokenVertexForDepVertex(dv));
//                 if (data ==0 || data->empty()) { continue; };
//                 // if dv is a prep
//                 if (data->firstValue(*macroAccessor) == L_PREP)
//                 {
//                   for (std::deque<std::string>::iterator itAdju= adjunctRels.begin(); itAdju != adjunctRels.end(); itAdju++)
//                   {
//                     if (!SecondUngovernedBy(language,
//                       utf8stdstring2limastring(*itAdju))(*anagraph, dv,*candidateItr->m_morphVertex,ac))
//                  return true; 
//                   }
//                 }
//               }
              }
            } 
          }
        }
//  TimeUtils::logElapsedTime("end secondboucle");
//  TimeUtils::updateCurrentTime();
      }
    }
  }
 /*TimeUtils::logElapsedTime("end aba4");
 TimeUtils::updateCurrentTime();
 */ return false;
}


// [higher slot] subj > agent > obj > (iobj|pobj) [lower slot]
int CoreferentAnnotation::getSlotValue(
  const SyntacticData* sd,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  const std::map<std::string, int>& slotValues,
                                       MediaId language) const
{
  COREFSOLVERLOGINIT;
  std::map<std::string,std::deque<std::string> >::const_iterator rel = relLocalDef.find("SubjectRelation");
  if (rel == relLocalDef.end())
  {
    LERROR << "\"SubjectRelation\" not defined in s2-lp-xxx.xml"<< LENDL;
  }
  else if (isFunctionMasterOf(sd,(*rel).second, language))
  {
    return (*slotValues.find("SubjectRelation")).second;
  }

  rel = relLocalDef.find("AgentRelation");
  if (rel == relLocalDef.end())
  {
    LERROR << "\"AgentRelation\" not defined in s2-lp-xxx.xml"<< LENDL;
  } 
  else if(isFunctionMasterOf(sd,(*rel).second,language))
  {
    return (*slotValues.find("AgentRelation")).second;
  }
  rel = relLocalDef.find("CODRelation");
  if (rel == relLocalDef.end())
  {
    LERROR << "\"CODRelation\" not defined in s2-lp-xxx.xml"<< LENDL;
  } 
  else if (isFunctionMasterOf(sd,(*rel).second,language)) 
    return (*slotValues.find("CODRelation")).second;

  rel = relLocalDef.find("COIRelation");
  if (rel == relLocalDef.end())
  {
    LERROR << "\"COIRelation\" not defined in s2-lp-xxx.xml"<< LENDL;
  } 
  else if (isFunctionMasterOf(sd,(*rel).second,language)) 
    return (*slotValues.find("COIRelation")).second;

  rel = relLocalDef.find("AdjunctRelation");
  if (rel == relLocalDef.end())
  {
    LERROR << "\"AdjunctRelation\" not defined in s2-lp-xxx.xml"<< LENDL;
  } 
  else if (isFunctionMasterOf(sd,(*rel).second,language)) 
    return (*slotValues.find("AdjunctRelation")).second;

  return 0;
}

// A is a determiner of a noun Q, 
// and
//    (i) Q is in the argument domain of N,
//        and N fills a higher argument slot than Q.
// or (ii) Q is in the adjunct domain of N.
bool CoreferentAnnotation::aba5(
  const CoreferentAnnotation& ca,
  const SyntacticData* sd,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  MediaId language,
  LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  const std::map<std::string, int>& slotValues,
  const LinguisticCode& conjCoord) const
{
//   COREFSOLVERLOGINIT; 
//   LDEBUG << "aba5";
  LinguisticGraph* graph = anagraph->getGraph();
  LinguisticCode L_NC = (*tagLocalDef.find("NomCommunMacroCategory")).second;
  LinguisticCode L_NP = (*tagLocalDef.find("NomPropreMacroCategory")).second;
  bool res = false;
  DependencyGraphVertex* qv = new DependencyGraphVertex();
  if (ca.isDeterminer(qv,sd, relLocalDef, language, anagraph, ac))
  {
    MorphoSyntacticData* data = get(vertex_data,*graph,sd->tokenVertexForDepVertex(*qv));
    if (data ==0 || data->empty()) { return false; };
    // if *qv is a noun
    if (data->firstValue(*macroAccessor) == L_NC || data->firstValue(*macroAccessor) == L_NP)
    {
      // if Q is in the argument domain of N,
      CoreferentAnnotation caQ(0,*qv);
      if (caQ.isInTheArgumentDomainOf(ca,sd, language, anagraph, ac, macroAccessor, microAccessor, tagLocalDef, conjCoord, new set<LinguisticGraphVertex>()))
      {
        // and if N fills a higher argument slot than Q.
        if (ca.getSlotValue(sd, relLocalDef, slotValues,language) > caQ.getSlotValue(sd, relLocalDef, slotValues,language))
        {
          return true;
        }
      }
      // or if Q is in the adjunct domain of N.
      if (caQ.isInTheAdjunctDomainOf(*this,sd,graph,macroAccessor,tagLocalDef,relLocalDef,language))
      {
        return true;
      }
    }
  }
  return res;
}




  /** main functions of the global algorithm (called by Corefsolver) */

int CoreferentAnnotation::classify(
  LinguisticGraph* graph, 
  SyntacticData* sd, 
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const std::map<std::string, std::deque<std::string> >& relLocalDef,
  const std::set< LinguisticCode >& definiteCategs,
  const std::set< LinguisticCode >& reflexiveReciprocalCategs,
  const std::set< LinguisticCode >& undefPronouns,
  const std::set< LinguisticCode >& possPronouns,
  const bool& resolveDefinites,
  const bool& resolveN3PPronouns,
  const Common::PropertyCode::PropertyAccessor* personAccessor,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                                   MediaId language)
{ 
  int res = 0;
  std::string type = referentType(sd,graph,macroAccessor,microAccessor, tagLocalDef, relLocalDef ,definiteCategs, reflexiveReciprocalCategs, undefPronouns,possPronouns, personAccessor, anagraph, language);
  categ(type);
  // anaphora
  if (type=="other"
   || (type =="reflPron")
   || (resolveDefinites && type =="def") 
   || (resolveN3PPronouns && type =="N3Ppron") )
  {
    res += 1;
  }
  // candidate
  if (type!="pleon" && (type!="N3Ppron" || resolveN3PPronouns) && type!="reflPron" && type!="on" && type!="undefPron")
  {
    res += 10;
  }
  return res;
}

  float CoreferentAnnotation::salienceWeighting(
  const std::map<std::string,float>& weights,
  const SyntacticData* sd,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  MediaId language,
  const std::map< std::string,LinguisticCode>& tagLocalDef,
  const std::map< std::string,std::deque<std::string> >& relLocalDef,
  AnalysisContent& ac,
  LinguisticGraphVertex& beginSentence,
  LinguisticGraphVertex& endSentence) const
  {
//   COREFSOLVERLOGINIT;
  float res =0;
  if (weights.find("SentenceRecency")!=weights.end()) 
  {    
     res += (*weights.find("SentenceRecency")).second;
  }
  if (weights.find("SubjEmph")!=weights.end() && isFunctionMasterOf(sd,(*relLocalDef.find("SubjectRelation")).second,language)) 
  {    
     res += weights.find("SubjEmph")->second;
  }
  if (weights.find("ExistEmph")!=weights.end() && isFunctionMasterOf(sd,(*relLocalDef.find("AttributeRelation")).second,language)) 
  {    
     res += weights.find("ExistEmph")->second;
  }
  if (weights.find("CodEmph")!=weights.end() && isFunctionMasterOf(sd,(*relLocalDef.find("CODRelation")).second,language)) 
  {    
     res += weights.find("CodEmph")->second;
  }
  if (weights.find("CoiCoblEmph")!=weights.end() && isFunctionMasterOf(sd,(*relLocalDef.find("COIRelation")).second,language)) 
  {    
     res += weights.find("CoiCoblEmph")->second;
  }
  if (m_morphVertex == npHeadVertex(sd,language, new set<DependencyGraphVertex>()))
  {    
     res += weights.find("HeadEmph")->second;
  }
  if (weights.find("NonAdvEmph")!=weights.end() && !CoreferentAnnotation(0,npHeadVertex(sd,language, new set<DependencyGraphVertex>())).isFunctionMasterOf(sd,(*relLocalDef.find("AdjunctRelation")).second,language)) 
  {    
     res += weights.find("NonAdvEmph")->second;
  }
  if (weights.find("IsInSubordinate")!=weights.end() && weights.find("IsInSubordinate")->second!=0 && isInSubordinate(sd,macroAccessor,tagLocalDef,language,anagraph,ac, new set<LinguisticGraphVertex>())) 
  {
    res += weights.find("IsInSubordinate")->second;
  }
  Token* token = get(vertex_token, *anagraph->getGraph(), m_morphVertex);
// cerr << token->stringForm() <<endl;
  if (weights.find("NoAntecedencyPotential")!=weights.end() && weights.find("NoAntecedencyPotential")->second!=0 && token != 0 && limastring2utf8stdstring(token->stringForm()) == "y")
  {
    res += weights.find("NoAntecedencyPotential")->second;
  }  
  if (weights.find("Appos")!=weights.end() && weights.find("Appos")->second!=0 && isInAppos(sd,anagraph,ac,language,new std::set<DependencyGraphVertex>()))
  {
    res += weights.find("Appos")->second;
  }  
  if (weights.find("BeginWithColon")!=weights.end() && weights.find("BeginWithColon")->second!=0 && beginWithColon(anagraph,beginSentence))
  {
    res += weights.find("BeginWithColon")->second;
  }
  if (weights.find("EndWithColon")!=weights.end() && weights.find("EndWithColon")->second!=0 && endWithColon(anagraph,endSentence))
  {
    res += weights.find("EndWithColon")->second;
  }
  if (weights.find("AntecedencyPotential")!=weights.end() && weights.find("AntecedencyPotential")->second!=0 && isInQuantA(sd,anagraph))
  {
    res += weights.find("AntecedencyPotential")->second;
  }  
    return res;
} 

AnnotationGraphVertex CoreferentAnnotation::writeAnnotation(
  Common::AnnotationGraphs::AnnotationData* ad,
  CoreferentAnnotation& antecedent) const
{
  COREFSOLVERLOGINIT;
  LDEBUG << "CoreferentAnnotation::writeAnnotation " << m_morphVertex << " refering to " << antecedent.m_morphVertex;

  std::set< AnnotationGraphVertex > matches = ad->matches("PosGraph",m_morphVertex,"annot");
  if (matches.empty())
  {
    COREFSOLVERLOGINIT;
    LERROR << "CoreferentAnnotation::writeAnnotation No annotation graph vertex matches PoS graph vertex " << m_morphVertex <<  ". This should not happen.";
    return AnnotationGraphVertex();
  }
  AnnotationGraphVertex av = *matches.begin();
  

  if (!ad->hasAnnotation(av, Common::Misc::utf8stdstring2limastring("Coreferent")))
  {
    /** Creation of an annotation for the object CoreferentAnnotation */
    GenericAnnotation ga(*this); 

    ad->annotate(av, utf8stdstring2limastring("Coreferent"), ga);
  }

  std::set< AnnotationGraphVertex > antecedentMatches = ad->matches("PosGraph",antecedent.m_morphVertex,"annot");
  if (antecedentMatches.empty())
  {
    COREFSOLVERLOGINIT;
    LERROR << "CoreferentAnnotation::writeAnnotation No annotation graph vertex matches PoS graph antecedent vertex " << antecedent.m_morphVertex <<  ". This should not happen.";
    return AnnotationGraphVertex();
  }
  AnnotationGraphVertex antecedentAv = *antecedentMatches.begin();

  if (!ad->hasAnnotation(antecedentAv, Common::Misc::utf8stdstring2limastring("Coreferent")))
  {
    /** Creation of an annotation for the object CoreferentAnnotation */
    GenericAnnotation ga(antecedent); 

    /** Creation of a new vertex (a new annotation anchor) in the annotation graph. */
  //  AnnotationGraphVertex av = ad->createAnnotationVertex();

  ad->annotate(antecedentAv, utf8stdstring2limastring("Coreferent"), ga);
  }

  /** If anaphora, the two concerned vertices are binded */ 
  if (av!=antecedentAv)
  {
    ad->createAnnotationEdge(av, antecedentAv);
  }
  return AnnotationGraphVertex(); //unused;
}


void CoreferentAnnotation::outputXml(std::ostream& xmlStream,const LinguisticGraph& g, const AnnotationData* ad) const
{
  COREFSOLVERLOGINIT;
  LDEBUG << "CoreferentAnnotation::outputXml";
  CoreferentAnnotation antecedent;
  bool hasAntecedent = false;
  std::set< AnnotationGraphVertex > matches = ad->matches("PosGraph",m_morphVertex,"annot");
  if (matches.empty())
  {
    COREFSOLVERLOGINIT;
    LERROR << "CoreferentAnnotation::outputXml No annotation graph vertex matches PoS graph vertex " << m_morphVertex <<  ". This should not happen.";
    return ;
  }
  AnnotationGraphVertex av = *matches.begin();
  AnnotationGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::out_edges(av, ad->getGraph());
  if (it != it_end) 
  {
    for (; it != it_end; it++)
    {
       GenericAnnotation ga = ad->annotation(target(*it, ad->getGraph()), utf8stdstring2limastring("Coreferent"));
       try
      {
        antecedent = ga.value<CoreferentAnnotation>(); 
        hasAntecedent = true;
      }
      catch (const boost::bad_any_cast& )
      {
        continue;
      }
    }
  }
  if (hasAntecedent)
  {
    xmlStream << "<COREF ID=\"" << m_id << "\" TYPE=\"IDENT\" REF=\"" << antecedent.id() <<   "\"";
  }
  else
  {
    xmlStream << "<COREF ID=\"" << m_id<<"\"";
  }
  xmlStream << " CATEG=\"" << categ() << "\">";

  Token* token = get(vertex_token, g, m_morphVertex);
  if (token != 0)
  {
    xmlStream << token->stringForm();
    if (token->status().isAlphaPossessive())
    {
      xmlStream << "'s ";
    }
  }
  xmlStream << "</COREF>"; 
}


} // closing namespace Coreferences
} // closing namespace LinguisticProcessing
} // closing namespace Lima
