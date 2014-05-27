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
  *
  * @file        annotationGraphTestProcessUnit.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2004 by CEA
  * @date        Created on Nov, 8 2004
  *
  */
 
#include "corefSolver.h"
#include "coreferentAnnotation.h"
 
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/Automaton/basicConstraintFunctions.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SimplificationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/HomoSyntagmaticConstraints.h"
#include <stack>
#include <iostream>
#include <math.h>

using namespace std;
using namespace boost;
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
SimpleFactory<MediaProcessUnit,CorefSolver> coreferencesSolvingProcessUnit(COREFSOLVINGPU_CLASSID);



CorefSolver::CorefSolver() :
    m_language()
{}


void CorefSolver::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  COREFSOLVERLOGINIT;
  m_language=manager->getInitializationParameters().media;
  try {
    m_scope=atoi(unitConfiguration.getParamsValueAtKey("scope").c_str());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No 'scope' defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    m_scope = 3;
    LERROR << "Scope is set to 3 by default.";
  }
  try {
    m_threshold=atoi(unitConfiguration.getParamsValueAtKey("threshold").c_str());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No 'threshold' defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    m_threshold = 70;
    LERROR << "Threshold is set to 130 by default.";
  }
  try {
    m_resolveDefinites=atoi(unitConfiguration.getParamsValueAtKey("Resolve Definites").c_str());
    m_resolveN3PPronouns=atoi(unitConfiguration.getParamsValueAtKey("Resolve non third person pronouns").c_str());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "Please define 'Resolve Definites' and 'Resolve non third person pronouns' in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    m_resolveDefinites = 1;
    m_resolveN3PPronouns = 0;
    LERROR << "Resolve Definites is set to true (1) by default.";
    LERROR << "Resolve non third person pronouns is set to false (0) by default.";
  }
  cerr << m_language << endl;
  const Common::PropertyCode::PropertyManager& macroManager=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO");
  const Common::PropertyCode::PropertyManager& microManager=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO");
  m_macroAccessor=&macroManager.getPropertyAccessor();
  m_microAccessor=&static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO");
  m_genderAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("GENDER"));
  m_personAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("PERSON"));
  m_numberAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("NUMBER"));
  m_inNpCategs = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getNounPhraseHeadMicroCategories();
  m_definiteCategs = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getDefiniteMicroCategories();

  std::map<string, string> tmpMap = unitConfiguration.getMapAtKey("MacroCategories");
  for (map<string, string>::const_iterator it = tmpMap.begin(); it!=tmpMap.end(); it++) 
    m_tagLocalDef.insert(make_pair((*it).first,macroManager.getPropertyValue((*it).second)));
  try {
    deque<string> tmpDeque = unitConfiguration.getListsValueAtKey("LexicalAnaphora");
    for (deque<string>::const_iterator it = tmpDeque.begin(); it!=tmpDeque.end(); it++) 
    {
      m_reflexiveReciprocal.insert(microManager.getPropertyValue((*it)));
    }
    tmpDeque.clear();
    tmpDeque =
    unitConfiguration.getListsValueAtKey("UndefinitePronouns");
    for (deque<string>::const_iterator it = tmpDeque.begin(); it!=tmpDeque.end(); it++) 
    {
      m_undefPronouns.insert(microManager.getPropertyValue((*it)));
    }
    tmpDeque.clear();
    tmpDeque =
    unitConfiguration.getListsValueAtKey("PossessivePronouns");
    for (deque<string>::const_iterator it = tmpDeque.begin(); it!=tmpDeque.end(); it++) 
    {
      m_undefPronouns.insert(microManager.getPropertyValue((*it)));
    }
    m_conjCoord = microManager.getPropertyValue("L_CONJ_COORD");
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& )
  {
    LERROR << "One of the tags list is not defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    LERROR << "Please check all of the following categories:";
    LERROR << "'LexicalAnaphora', 'Undefinite Pronouns'.";
  }

  try {
    m_relLocalDef.insert(make_pair("PrepRelation",unitConfiguration.getListsValueAtKey("PrepRelation")));
    m_relLocalDef.insert(make_pair("PleonasticRelation",unitConfiguration.getListsValueAtKey("PleonasticRelation"))); 
    m_relLocalDef.insert(make_pair("DefiniteRelation",unitConfiguration.getListsValueAtKey("DefiniteRelation")));
    m_relLocalDef.insert(make_pair("SubjectRelation",unitConfiguration.getListsValueAtKey("SubjectRelation")));
    m_relLocalDef.insert(make_pair("AttributeRelation",unitConfiguration.getListsValueAtKey("AttributeRelation")));
    m_relLocalDef.insert(make_pair("CODRelation",unitConfiguration.getListsValueAtKey("CODRelation")));
    m_relLocalDef.insert(make_pair("COIRelation",unitConfiguration.getListsValueAtKey("COIRelation")));
    m_relLocalDef.insert(make_pair("AdjunctRelation",unitConfiguration.getListsValueAtKey("AdjunctRelation")));
    m_relLocalDef.insert(make_pair("AgentRelation",unitConfiguration.getListsValueAtKey("AgentRelation")));
    m_relLocalDef.insert(make_pair("NPDeterminerRelation",unitConfiguration.getListsValueAtKey("NPDeterminerRelation")));
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& )
  {
    LERROR << "One of the macro relation is not defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    LERROR << "Please check all of the following relations:";
    LERROR << "'PrepRelation', 'PleonasticRelation', 'DefiniteRelation', 'SubjectRelation', 'AttributeRelation', 'CODRelation', 'COIRelation', 'AdjunctRelation', 'AgentRelation', 'NPDeterminerRelation'.";
  }


  // Salience factors
  try
  {
    const std::map<std::string,std::string>& salience=unitConfiguration.getMapAtKey("SalienceFactors");
    for (std::map<std::string,std::string>::const_iterator it=salience.begin();
         it!=salience.end();
         it++)
    { 
       m_salienceWeights[it->first]=atof((it->second).c_str());
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchMap& )
 {
    LERROR << "No map 'SalienceFactors' in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
//     throw InvalidConfiguration();
  }

 // Slot Values
  try
  {
    const std::map<std::string,std::string>& slotValuesStr=unitConfiguration.getMapAtKey("SlotValues");
    for (std::map<std::string,std::string>::const_iterator it=slotValuesStr.begin();
         it!=slotValuesStr.end();
         it++)
    { 
       m_slotValues[it->first]=atoi((it->second).c_str());
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchMap& )
 {
    LERROR << "No map 'SlotValues' in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
//     throw InvalidConfiguration();
  }

}

/**
 * 
 * @param analysis 
 * @return 
 */
LimaStatusCode CorefSolver::process(
  AnalysisContent& analysis) const
{
  COREFSOLVERLOGINIT;
  TimeUtils::updateCurrentTime();
  LINFO << "start CorefSolver";
  // create syntacticData  
  AnalysisGraph* posgraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph==0)
  {
    LERROR << "no PosGraph ! abort";
    return MISSING_DATA;
  }
  SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    LERROR << "no sentence bounds ! abort";
    return MISSING_DATA;
  }
  if (sb->getGraphId() != "PosGraph") {
    LERROR << "SentenceBounds computed on graph '" << sb->getGraphId() << "'. CorefSolver needs " <<
    "sentence bounds on PosGraph";
    return INVALID_CONFIGURATION;
  }
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  if (sb==0)
  {
    LERROR << "no syntactic data ! abort";
    return MISSING_DATA;
  }

  
  /** Access to or creation of an annotation graph */
  AnnotationData* annotationData=static_cast<AnnotationData*>(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData=new AnnotationData();
    /** Creates a node in the annotation graph for each node of the 
      * morphosyntactic graph. Each new node is annotated with the name mrphv and
      * associated to the morphosyntactic vertex number */
    if (static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph")) != 0)
    {
      static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(annotationData, "AnalysisGraph");
    } 
    if (static_cast<AnalysisGraph*>(analysis.getData("PosGraph")) != 0)
    {
      
      static_cast<AnalysisGraph*>(analysis.getData("PosGraph"))->populateAnnotationGraph(annotationData, "PosGraph");
    }
   
    analysis.setData("AnnotationData",annotationData);
  }


  /** Write words on the annotation vertex */
// trop long !!
//       LinguisticGraphVertexIt itg, itg_end;
//       boost::tie(itg, itg_end) = vertices(*anagraph->getGraph());
//       for (; itg != itg_end; itg++)
//       {
//         LinguisticGraph graph =*anagraph->getGraph(); 
//         LinguisticAnalysisStructure::Token* token=get(vertex_token, graph, *itg);
//         if (token != 0)
//         {
//           LimaString mot = token->stringForm();
//           annotationData->annotate(*itg, Common::Misc::utf8stdstring2limastring("Mot"),mot);
//         }
//       } 



  /** To be able to dump the content of an annotation, a function pointer with
    * a precise signature has to be givent to the annotation graph. See
    * @ref{annotationGraphTestProcessUnit.h} for the details of the dumpPoint 
    * function */
  if (annotationData->dumpFunction("Coreferent") == 0)
  {
    annotationData->dumpFunction("Coreferent", new DumpCoreferent());
  }


  /* npVertices and npCandidates are respectively deques of vectors and sets.
      The number of elements of these deques matches the size of the scope (ie. number of sentences in which the antecedents can be found) entered as a parameter.
      Thus each of the internal vectors (resp. sets) corresponds to one and only one sentence. */
  std::deque<std::vector<CoreferentAnnotation> > npVertices;
  std::deque<Vertices>* npCandidates =  new std::deque<Vertices>();


  LinguisticGraph* graph=posgraph->getGraph();
  uint64_t nextReferentId = 1;
  std::set< LinguisticGraphVertex > alreadyProcessedVertices;

  //LinguisticGraphVertex beginSentence=sb->getStartVertex();
  // for each sentence
  // ??OME2 for (SegmentationData::const_iterator boundItr=sb->begin();
  //     boundItr!=sb->end();
  for (std::vector<Segment>::const_iterator boundItr=(sb->getSegments()).begin();
       boundItr!=(sb->getSegments()).end();
       boundItr++)
  {
    LinguisticGraphVertex beginSentence=boundItr->getFirstVertex();
    LinguisticGraphVertex endSentence=boundItr->getLastVertex();
    
    // non pleonastic pronouns (or definite NP)
    Vertices* npAnaphora = new Vertices();
    // Ruled Out Binding by syntacticFilter
    VerticesRelation* roBinding = new VerticesRelation();
    // lexical Anaphora Binding
    WeightedVerticesRelation* lexAnaBinding = new WeightedVerticesRelation();
    // potentialBinding
    WeightedVerticesRelation* pBinding = new WeightedVerticesRelation();
    
    // add entry in npVertices and npCandidates
    npVertices.push_front(std::vector<CoreferentAnnotation>());
    npCandidates->push_front(Vertices());
    // if size>scope, remove last 
    if (npVertices.size()>m_scope)
      npVertices.pop_back();
    if (npCandidates->size()>m_scope)
      npCandidates->pop_back();
    //degradate the existing salience factors
    for (std::deque<Vertices>::iterator itm = npCandidates->begin();
         itm != npCandidates->end( ); 
         itm++)
    {
      for (Vertices::iterator candidateItr = (*itm).begin( );
           candidateItr != (*itm).end( ); 
           candidateItr++ )
      {
        (*candidateItr)->salience((*candidateItr)->salience()/2);
      }
    }

    // ces lignes servaient en fait avant que je change le code sur la césure des phrases
    // ie : avant L_PONCTU-L_PONCTU_MISE_EN_EVIDENCE était un token troncateur de phrase mais il ne l'est plus.
//     LinguisticAnalysisStructure::Token* token=get(vertex_token, *anagraph->getGraph(), *boundItr);
//     while (token != 0 && (limastring2utf8stdstring(token->stringForm()) == "\"" || limastring2utf8stdstring(token->stringForm()) == "'"))
//     {
//       boundItr++;
//       token=get(vertex_token, *anagraph->getGraph(), *boundItr);
//     } 
    //////////
    //LinguisticGraphVertex endSentence=*boundItr;


    LDEBUG << "analyze sentence from vertex " << beginSentence << " to vertex " << endSentence;


    // for each word in the sentence
    LinguisticGraphVertex v=beginSentence;
    LinguisticGraphVertex savedSb=beginSentence;
    while (v!=endSentence)
    {
      if (alreadyProcessedVertices.find(v) != alreadyProcessedVertices.end())
        continue;
      //else alreadyProcessedVertices.insert(v); 
      CoreferentAnnotation ca(nextReferentId,v);
      if (ca.isIncludedInNounPhrase(graph, m_language, posgraph, analysis, m_inNpCategs, m_microAccessor) )
      {
        (*npVertices.begin()).push_back(ca);
        alreadyProcessedVertices.insert(v);
        nextReferentId++;
      }
      beginSentence=endSentence;
      LinguisticGraphOutEdgeIt ite, ite_end;
      boost::tie(ite, ite_end)=boost::out_edges(v, *graph);
      v=target(*ite, *graph);
      
    }/*
TimeUtils::logElapsedTime("retrieve nps");
TimeUtils::updateCurrentTime();*/

    LDEBUG << "classify NPs and calculate salience weights";
    for(vector<CoreferentAnnotation>::iterator itca = (*npVertices.begin()).begin();
        itca!=(*npVertices.begin()).end();
        itca++)
    {
      (*itca).newerRef(&(*itca));
      int classif = (*itca).classify(graph, syntacticData, m_macroAccessor, m_microAccessor, m_tagLocalDef, m_relLocalDef, m_definiteCategs, m_reflexiveReciprocal, m_undefPronouns, m_possPronouns, m_resolveDefinites, m_resolveN3PPronouns, m_personAccessor, posgraph, m_language);
      if (classif == 1 || classif == 11)
      {
        npAnaphora->insert(&(*itca));
      }
      if (classif == 10 || classif == 11)
      {
        (*npCandidates->begin()).insert(&(*itca));
      }

      (*itca).salience((*itca).salienceWeighting(m_salienceWeights, syntacticData, m_macroAccessor,posgraph,m_language,m_tagLocalDef, m_relLocalDef,analysis,savedSb,endSentence));
    }

// TimeUtils::logElapsedTime("classify");
// TimeUtils::updateCurrentTime();

    LDEBUG<< "Anaphora:";
    for (Vertices::iterator anaphItr=npAnaphora->begin();
         anaphItr!=npAnaphora->end();
         anaphItr++)
      LDEBUG<<(*anaphItr)->morphVertex()<< " - " << limastring2utf8stdstring(get(vertex_token, *graph, (*anaphItr)->morphVertex())->stringForm()) <<(*anaphItr)->newerRef()->morphVertex()<<LENDL;
    LDEBUG<< " - ";

    LDEBUG<< "Candidates:";
    for (std::deque<Vertices>::iterator itm = npCandidates->begin( );
         itm != npCandidates->end( ); 
         itm++ )
      for (Vertices::iterator itc = (*itm).begin( );
           itc != (*itm).end( ); 
           itc++ )
        LDEBUG<< " - " << limastring2utf8stdstring(get(vertex_token, *graph, (*itc)->morphVertex())->stringForm()) << ":" << (*itc)->salience();
    LDEBUG<< " - ";

    LDEBUG << "initialize syntactic filter";
    initSyntacticFilter(analysis, posgraph, syntacticData, npAnaphora, npCandidates, roBinding);
    
// TimeUtils::logElapsedTime("initialize syntactic filter");
// TimeUtils::updateCurrentTime();

    LDEBUG<< "RuledOutBinding:";
    for (VerticesRelation::iterator itro = roBinding->begin( );
         itro != roBinding->end( ); 
         itro++ )
    {
      LDEBUG<< " Anaphora: " << limastring2utf8stdstring(get(vertex_token, *graph, (*itro).first->morphVertex())->stringForm())<< LENDL;
      for (std::set<CoreferentAnnotation*>::iterator its = (*itro).second.begin( );
           its != (*itro).second.end( ); 
           its++ )
        LDEBUG << " - " << limastring2utf8stdstring(get(vertex_token, *graph, (*its)->morphVertex())->stringForm()) << " - ";
    }
    LDEBUG<< LENDL;

    LDEBUG << "initialize lexical anaphora binding";
    bindingLexicalAnaphora(analysis,posgraph, syntacticData, npAnaphora, npCandidates, lexAnaBinding);

// TimeUtils::logElapsedTime("initialize lex ana binding");
// TimeUtils::updateCurrentTime();

    LDEBUG<< "Lexical Anaphora Binding:";
    for (WeightedVerticesRelation::iterator itp = lexAnaBinding->begin( );
         itp != lexAnaBinding->end( ); 
         itp++ )
    {
      LDEBUG<< "Anaphora: " << limastring2utf8stdstring(get(vertex_token, *graph, (*itp).first->morphVertex())->stringForm())<< LENDL;
      for (std::map<CoreferentAnnotation*, float>::iterator its = (*itp).second.begin( );
           its != (*itp).second.end( ); 
           its++ )  
        LDEBUG <<(*its).first->morphVertex()<< " - " << limastring2utf8stdstring(get(vertex_token, *graph, (*its).first->morphVertex())->stringForm()) <<  " : " <<  (*its).second;
    }

    LDEBUG << "resolve lexical anaphora";
    getBest(syntacticData, posgraph, lexAnaBinding, /*results,*/ npCandidates, annotationData);


// TimeUtils::logElapsedTime("resolve lexical anaphora");
// TimeUtils::updateCurrentTime();

    LDEBUG << "initialize potential binding";
    bindingPotentialCandidates(posgraph, npAnaphora, npCandidates, pBinding);

    LDEBUG << "adjust local saliences";
    adjustSaliences(syntacticData,npCandidates,pBinding, endSentence,posgraph,analysis);

    LDEBUG<< "Potential Binding:";
    for (WeightedVerticesRelation::iterator itp = pBinding->begin( );
         itp != pBinding->end( ); 
         itp++ )
    {
      LDEBUG<< "Anaphora: " << limastring2utf8stdstring(get(vertex_token, *graph, (*itp).first->morphVertex())->stringForm())<< LENDL;
      for (std::map<CoreferentAnnotation*, float>::iterator its = (*itp).second.begin( );
           its != (*itp).second.end( ); 
           its++ ) 
      { 
        LDEBUG <<(*its).first->morphVertex()<< " - " << limastring2utf8stdstring(get(vertex_token, *graph, (*its).first->morphVertex())->stringForm()) <<  " : " <<  (*its).second;
      }
    }
    LDEBUG<< LENDL;



    LDEBUG << "apply threshold filter";
    applyThresholdFilter(pBinding);
    
    LDEBUG << "apply circular filter";
    applyCircularFilter(pBinding);
    
    LDEBUG << "apply morphosyntactic filter";
    applyMorphoSyntacticFilter(pBinding,roBinding);

/*TimeUtils::logElapsedTime("apply filters");
TimeUtils::updateCurrentTime();*/
    LDEBUG << "resolve binding";
    getBest(syntacticData,posgraph,pBinding,/*results,*/npCandidates,annotationData);

/*TimeUtils::logElapsedTime("resolve binding");
TimeUtils::updateCurrentTime();*/
    delete npAnaphora;  
    delete roBinding;
    delete pBinding;
    delete lexAnaBinding;
  }

  LDEBUG << "write coreferent annotations on graph";

  delete npCandidates;
//   delete results;
  TimeUtils::logElapsedTime("Coreferences");
  return SUCCESS_ID;
}


  void CorefSolver::initSyntacticFilter(
  AnalysisContent& ac,
  AnalysisGraph* anagraph,
  SyntacticData* syntacticData,
  Vertices* npAnaphora,
  std::deque<CoreferentAnnotation::Vertices>* npCandidates,
  VerticesRelation* roBinding) const
{  
//   COREFSOLVERLOGINIT;
/*TimeUtils::logElapsedTime("init synt filter");
TimeUtils::updateCurrentTime();*/
  LinguisticGraph* graph = anagraph->getGraph();
  // for every anaphora
  for (Vertices::iterator anaphItr=npAnaphora->begin();
       anaphItr!=npAnaphora->end();
       anaphItr++)
  {
    // for every candidate
    std::set<CoreferentAnnotation*> tmpCandidate;
    for (std::deque<Vertices>::iterator itm = npCandidates->begin();
      itm != npCandidates->end( ); 
      itm++)
    for (Vertices::iterator candidateItr = (*itm).begin( );
         candidateItr != (*itm).end( ); 
         candidateItr++ )
    { 
      // if to be ruled out, insert in roBinding
      if (
      // P is diferent from N.
      /*(*anaphItr)->morphVertex() != (*candidateItr)->morphVertex() && */
      // P and N have incompatible agreement features.
      !(*anaphItr)->isAgreementCompatibleWith(**candidateItr, syntacticData, m_genderAccessor, m_personAccessor, m_numberAccessor, m_language, anagraph, ac) ||
      // P & N are in the same sentence AND ...
      (
        (*npCandidates->begin()).find(*candidateItr)!=(*npCandidates->begin()).end()
        &&
        // P is in the argument domain of N. 
        // do not rule out if P is SujInv et N est SUJ_V
        (
           (*anaphItr)->isInTheArgumentDomainOf(**candidateItr, syntacticData, m_language, anagraph, ac, m_macroAccessor, m_microAccessor, m_tagLocalDef,m_conjCoord, new set<LinguisticGraphVertex>())
          &&
          !(
            GovernorOf(m_language,utf8stdstring2limastring("SujInv"))(*anagraph,(*anaphItr)->morphVertex(),ac)
            &&
            GovernorOf(m_language,utf8stdstring2limastring("SUJ_V"))(*anagraph,(*candidateItr)->morphVertex(),ac)
           )
//         &&  limastring2utf8stdstring(get(vertex_token, *graph, (*anaphItr)->morphVertex())->stringForm())!="en"
        )  
      )||
      // P is in the adjunct domain of N.
        ((*anaphItr)->isInTheAdjunctDomainOf(**candidateItr, syntacticData, graph, m_macroAccessor, m_tagLocalDef, m_relLocalDef, m_language) /*&&  limastring2utf8stdstring(get(vertex_token, *graph, (*anaphItr)->morphVertex())->stringForm())!="en" no change*/)||
      // P is an argument of a head H, N is not a pronoun, and N is contained in H.
//        (*anaphItr)->sf4(**candidateItr, syntacticData, m_macroAccessor, L_PRON(), m_language, anagraph, ac) ||
      // P is in the NP domain of N.
      (*anaphItr)->isInTheNpDomainOf(**candidateItr, syntacticData, m_macroAccessor, m_tagLocalDef, m_relLocalDef, m_language, anagraph, ac) ||
      // P is a determiner of a noun Q, and N is contained in Q.
      (*anaphItr)->sf6(**candidateItr, syntacticData, m_relLocalDef, m_language, anagraph, ac)
      )
      {
        tmpCandidate.insert(*candidateItr);
      }
    }
    roBinding->insert(make_pair(*anaphItr, tmpCandidate));
    tmpCandidate.clear();
  }
}
  
  void CorefSolver::bindingLexicalAnaphora(
  AnalysisContent& ac,
  AnalysisGraph* anagraph,
  SyntacticData* syntacticData,
  Vertices* npAnaphora,
  std::deque<CoreferentAnnotation::Vertices>* npCandidates,
  WeightedVerticesRelation* lexAnaBinding) const
{ 
//   COREFSOLVERLOGINIT;
      /*TimeUtils::logElapsedTime("init blA");
      TimeUtils::updateCurrentTime();
  */LinguisticGraph* graph = anagraph->getGraph();
  // for every anaphora
  for (Vertices::iterator anaphItr=npAnaphora->begin();
       anaphItr!=npAnaphora->end();
       anaphItr++)
  {
    // if not reciprocal or reflexive, ignore
    if (!(*anaphItr)->isTaggedAsOneOfThese(graph,m_reflexiveReciprocal,m_microAccessor))
      continue;
    // for every candidate
    std::map<CoreferentAnnotation*, float> tmpCandidate;
    for (std::deque<Vertices>::iterator itm = npCandidates->begin();
         itm != npCandidates->end( ); 
         itm++)
      for (Vertices::iterator candidateItr = (*itm).begin( );
           candidateItr != (*itm).end( ); 
           candidateItr++ )
      {
        // if to be binded, insert in lexAnaBinding
        if ( 
          // A is diferent from N.
           (*anaphItr)->morphVertex() != (*candidateItr)->morphVertex() &&
           // A and N do not have any incompatible agreement features
          (*anaphItr)->isAgreementCompatibleWith(**candidateItr, syntacticData, m_genderAccessor, m_personAccessor, m_numberAccessor, m_language, anagraph, ac) && (
           // A is SUBSUBJUX of N
          !SecondUngovernedBy(m_language, utf8stdstring2limastring("SUBSUBJUX"))(*anagraph,(*anaphItr)->morphVertex(),(*candidateItr)->morphVertex(),ac) ||
           // A is in the argument domain of N,
           // and N fills a higher argument slot than A.
           (*anaphItr)->isInTheArgumentDomainOf2(**candidateItr, syntacticData,m_language,anagraph,ac,m_macroAccessor,m_microAccessor,m_tagLocalDef,m_conjCoord, new set<LinguisticGraphVertex>()) ||
           // A is in the adjunct domain of N.
           (*anaphItr)->isInTheAdjunctDomainOf(**candidateItr,syntacticData, anagraph->getGraph(), m_macroAccessor, m_tagLocalDef, m_relLocalDef, m_language) ||  
           // A is in the NP domain of N.
           (*anaphItr)->isInTheNpDomainOf(**candidateItr, syntacticData,  m_macroAccessor, m_tagLocalDef, m_relLocalDef, m_language, anagraph, ac) ||  
           // N is an argument of a verb V,
           // there is an NP Q in the argument domain of N such that Q has no noun determiner,
           // and
           //    (i) A is an argument of Q.
           // or (ii) A is an argument of a preposition PREP and PREP is an adjunct of Q.
           (*anaphItr)->aba4(**candidateItr,syntacticData,m_tagLocalDef,m_relLocalDef,m_macroAccessor,m_microAccessor,m_language,m_inNpCategs,anagraph,ac,m_conjCoord,npCandidates) ||
           // A is a determiner of a noun Q, 
           // and
           //    (i) Q is in the argument domain of N,
           //        and N fills a higher argument slot than Q.
           // or (ii) Q is in the adjunct domain of N.
           (*anaphItr)->aba5(**candidateItr,syntacticData,m_tagLocalDef,m_relLocalDef,m_macroAccessor,m_microAccessor,m_language,anagraph,ac,m_slotValues,m_conjCoord) 
        ))
        {  
           tmpCandidate.insert(make_pair(*candidateItr, (*candidateItr)->salience()));
        }
      }
    lexAnaBinding->insert(make_pair(*anaphItr, tmpCandidate));
  }
}


  void CorefSolver::bindingPotentialCandidates(
  AnalysisGraph* anagraph,
  Vertices* npAnaphora,
  std::deque<Vertices>* npCandidates,
  WeightedVerticesRelation* pBinding) const
{  
//   COREFSOLVERLOGINIT;
  LinguisticGraph* graph = anagraph->getGraph();
  applyEquivalentClassFilter(graph,npCandidates);
  // for every anaphora
  for (Vertices::iterator anaphItr=npAnaphora->begin();
       anaphItr!=npAnaphora->end();
       anaphItr++)
  {
    // if reciprocal or reflexive, ignore
    if ((*anaphItr)->isTaggedAsOneOfThese(graph,m_reflexiveReciprocal,m_microAccessor))
      continue;
    // for every candidate
    for (std::deque<Vertices>::iterator itm = npCandidates->begin();
         itm != npCandidates->end( ); 
         itm++)
    {
      for (Vertices::iterator candidateItr = (*itm).begin( );
           candidateItr != (*itm).end( ); 
           candidateItr++ )
      {
        WeightedVerticesRelation::iterator tmpBinding = pBinding->find(*anaphItr);
        if (tmpBinding==pBinding->end())
        {
          std::map <CoreferentAnnotation*,float> tmpMap;
          tmpMap.insert(make_pair(*candidateItr,(*candidateItr)->salience()));
          pBinding->insert(make_pair(*anaphItr,tmpMap));        
        }
        else
        {
            (*tmpBinding).second.insert(make_pair(*candidateItr,(*candidateItr)->salience()));
        }
      }
    }
  }
}

void CorefSolver::getBest(
  const SyntacticAnalysis::SyntacticData* sd,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  WeightedVerticesRelation* binding,
//   std::vector<std::pair<CoreferentAnnotation, CoreferentAnnotation> >* results,
  std::deque<CoreferentAnnotation::Vertices>* npCandidates,
 Common::AnnotationGraphs::AnnotationData* ad) const
{
  COREFSOLVERLOGINIT;
  LDEBUG << "CorefSolver::getBest binding size = " << binding->size();
  const LinguisticGraph* graph = anagraph->getGraph();
  for (WeightedVerticesRelation::iterator itp = binding->begin( );
         itp != binding->end( ); 
         itp++ )
  {
    LDEBUG << "  outer for loop on " << (*itp).first->morphVertex();
    bool result = false; 
    bool erase = false;
    CoreferentAnnotation* ca2erase = 0;
    std::pair<CoreferentAnnotation, CoreferentAnnotation> tmpPair;
    for (std::map<CoreferentAnnotation*, float>::iterator its = (*itp).second.begin( );
         its != (*itp).second.end( ); 
         its++ )  
    {
      LDEBUG << "  looking at " << (*itp).first->morphVertex() << " ("<<(*itp).first->bindingSalience()<<") -> " << (*its).first->morphVertex() << " ("<<(*its).second<<")";
      if ((*its).second>=(*itp).first->bindingSalience())
      {
        (*itp).first->bindingSalience((*its).second);
        tmpPair = make_pair(*(*itp).first, *(*its).first);
        ca2erase = (*its).first;
        // si id!=ref
        // et si id n'est pas anaphore lexicale
        // et si id n'est pas un défini
        // et si id != "y"
        // effacer ancienne référence

        if ((*itp).first!=(*its).first &&  (!((*its).first)->isTaggedAsOneOfThese(graph,m_reflexiveReciprocal,m_microAccessor)) && (*itp).first->referentType(sd,anagraph->getGraph(),m_macroAccessor,m_microAccessor,m_tagLocalDef,m_relLocalDef,m_definiteCategs,m_reflexiveReciprocal,m_undefPronouns,m_possPronouns,m_personAccessor,anagraph, m_language)!="def" &&
        limastring2utf8stdstring(get(vertex_token, *graph, ((*itp).first)->morphVertex())->stringForm())!="y" )
          erase = true;
        result = true;
      }
// used by First_NP baseline
// tmpPair = make_pair(*(*itp).first, *(*its).first);
// break;
    }
    if (result)
    {
//       results->push_back(tmpPair);
      tmpPair.first.writeAnnotation(ad, tmpPair.second);
      if (erase)
      {
        // mark newer Ref in npCandidates list so that former ref will be removed at next sentence
        for (std::deque<Vertices>::iterator itm = npCandidates->begin();
            itm != npCandidates->end( ); 
            itm++)
        {
          if ((*itm).find(ca2erase)!=(*itm).end())
          {
            (*(*itm).find(ca2erase))->newerRef((*itp).first);
          }
        }
        // erase the winning candidate from the current list of candidates. last equivalent will be this pronoun from now on.
        for (WeightedVerticesRelation::iterator itp2 = binding->begin( );
            itp2 != binding->end( ); 
            itp2++ )
        {
          LDEBUG << "  erasing " << (*itp2).first->morphVertex() << " -> " << ca2erase->morphVertex();
          (*itp2).second.erase(ca2erase);
        }
      }
    }
  }
}


  void CorefSolver::applyEquivalentClassFilter(  
  const LinguisticGraph* graph,
  std::deque<CoreferentAnnotation::Vertices>* npCandidates) const
{
//   COREFSOLVERLOGINIT;
  std::set<CoreferentAnnotation*> candidates2erase;
  for (std::deque<Vertices>::iterator itm = npCandidates->begin();
       itm != npCandidates->end( ); 
       itm++)
  {
    for (Vertices::iterator candidateItr = (*itm).begin( );
         candidateItr != (*itm).end( ); 
         candidateItr++ )
    {  
      if (!(*candidateItr)->newerRef()->isTaggedAsOneOfThese(graph,m_reflexiveReciprocal,m_microAccessor) && (*candidateItr)->hasNewerRef())
      {
//         cerr << (*candidateItr)->morphVertex() << " will be erased"<< endl;
        candidates2erase.insert(*candidateItr);
      }
    }
  }
  for (std::set<CoreferentAnnotation*>::iterator itE = candidates2erase.begin();
       itE != candidates2erase.end( ); 
       itE++)  
    for (std::deque<Vertices>::iterator itm = npCandidates->begin();
       itm != npCandidates->end( ); 
       itm++)
    {
      // keep best salience
      for (std::set<CoreferentAnnotation*>::iterator itm2 = (*itm).begin();
       itm2 != (*itm).end( ); 
       itm2++)
      {
        if ((*itm2)==(*itE)->newerRef() && (*itE)->salience()>(*itm2)->salience())
        {
          (*itm2)->salience((*itE)->salience());
          continue;
        }
      }
      // remove **itE from the candidates list
      (*itm).erase(*itE);
    }
}

void CorefSolver::adjustSaliences(
  LinguisticProcessing::SyntacticAnalysis::SyntacticData* sd,
  std::deque<Vertices>* npCandidates,
  CoreferentAnnotation::WeightedVerticesRelation* pBinding,
  LinguisticGraphVertex /*endSentence*/,
  const LinguisticAnalysisStructure::AnalysisGraph* /*anagraph*/,
  AnalysisContent& /*ac*/) const
{
//   COREFSOLVERLOGINIT;
  float cataphoraWeight = m_salienceWeights.find("Cataphora")->second;
  float sameSlotWeight = m_salienceWeights.find("SameSlot")->second;
  float itselfWeight = m_salienceWeights.find("Itself")->second;
//   float slightDiff = m_salienceWeights.find("SlightDifference")->second;

  for (WeightedVerticesRelation::iterator itp = pBinding->begin( );
       itp != pBinding->end( ); 
       itp++ )
  {
    for (std::map<CoreferentAnnotation*, float>::iterator its = (*itp).second.begin( );
         its != (*itp).second.end( ); 
         its++ )  
    {

      // if (NP_CANDIDATE in the same sentence as PRON or after)
      // && NP_CANDIDATE is after PRON 
      // <=>
      // NP_candidate is in last part of npCandidates
      // && vertexOf(PRON) >= vertexOf(NP_Candidate)
      if ((*npCandidates->begin()).find((*its).first) != (*npCandidates->begin()).end() && (*itp).first->morphVertex()>=(*its).first->morphVertex())
      {
        (*its).second += cataphoraWeight;
        cataphoraWeight-=10;
      }
//       bool res = false;
      // for each "macro" dependency relation
      for (std::map< std::string,std::deque<std::string> >::const_iterator itDr = m_relLocalDef.begin();
           itDr != m_relLocalDef.end();
           itDr++)
      {
        const std::deque<std::string> macroDependencyRel = (*itDr).second;
        // if PRON and NP fills the same "macro" dependency relation
        if ((*itp).first->isFunctionMasterOf(sd,macroDependencyRel,m_language)
          && (*its).first->isFunctionMasterOf(sd,macroDependencyRel,m_language))
        {
          (*its).second += sameSlotWeight;
        }
      }
//       // if PRON and NP fills each a different one of these slots : COI && COMPDUNOM
//       if(m_relLocalDef.find("COIRelation")!=m_relLocalDef.end()
//          &&
//          (
//            (*itp).first->isFunctionMasterOf(sd,(*m_relLocalDef.find("COIRelation")).second)
//             &&
//             (GovernorOf(m_language,utf8stdstring2limastring("COMPDUNOM"))(*anagraph,(*its).first->morphVertex(),ac)/*||GovernorOf(m_language,utf8stdstring2limastring("COD_V"))(*anagraph,(*its).first->morphVertex(),ac)*/)
//          ))
//       {
// //         (*its).second += sameSlotWeight+slightDiff;      
//       }   
      // decrease potential if CANDIDATE=PRON
      if ((*itp).first==(*its).first)
      {
        (*its).second += itselfWeight;
      }
    }
  }
}

/** @brief Ensure that there is no circular cross-references.
 * Computing limited to direct reverse reference because otherwise, the cost is hard to specify.
 */
void CorefSolver::applyCircularFilter(CoreferentAnnotation::WeightedVerticesRelation* pBinding) const
{
  COREFSOLVERLOGINIT;
  LDEBUG << "CorefSolver::applyCircularFilter binding size = " << pBinding->size();
  bool crossReferenceFound = false;
  do
  {
    crossReferenceFound = false;
    for (WeightedVerticesRelation::iterator itp = pBinding->begin( ); itp != pBinding->end( );
    itp++ )
    {
      CoreferentAnnotation* source = (*itp).first;
      source->morphVertex();
      for (std::map<CoreferentAnnotation*, float>::iterator its = (*itp).second.begin( ); its != (*itp).second.end( ); its++ )
      {
        CoreferentAnnotation* target = (*its).first;
        // current target of current source points also to references
        if (source != target && pBinding->find(target) != pBinding->end())
        {
          LDEBUG << "Check cross reference between " << source->morphVertex() <<" and " << target->morphVertex();
          WeightedVerticesRelation::iterator targetIt = pBinding->find(target);
          // current source and target are effectively cross-referencing each other
          if ( (*targetIt).second.find(source) != (*targetIt).second.end() )
          {
            crossReferenceFound = true;
            float sourceToTargetWeight = (*its).second;
            float targetToSourceWeight = (*(*targetIt).second.find(source)).second;
            LDEBUG << "Cross reference found between " << source->morphVertex() << " ("<<sourceToTargetWeight<<") and " << target->morphVertex() << " ("<<targetToSourceWeight<<")";
            // source to target is better
            if (sourceToTargetWeight >= targetToSourceWeight)
            {
              LDEBUG << "  erasing " << (*targetIt).first->morphVertex() << " -> " << (*(*targetIt).second.find(source)).first->morphVertex();
              (*targetIt).second.erase( (*targetIt).second.find(source) ) ;
            }
            // target to source is better
            else
            {
              LDEBUG << "  erasing " << (*itp).first->morphVertex() << " -> " << (*its).first->morphVertex();
              (*itp).second.erase(its);
            }
            break;
          }
        }
      }
      // go out of outer for loop if cross reference found as iterators are invalidated
      if (crossReferenceFound) break;
    }
  } while (crossReferenceFound);
  LDEBUG << "No more cross references";
}

  void CorefSolver::applyThresholdFilter(CoreferentAnnotation::WeightedVerticesRelation* pBinding) const
{
  COREFSOLVERLOGINIT;
  LDEBUG << "CorefSolver::applyThresholdFilter " << m_threshold;
  for (WeightedVerticesRelation::iterator itp = pBinding->begin( );
         itp != pBinding->end( ); 
         itp++ )
    {
      for (std::map<CoreferentAnnotation*, float>::iterator its = (*itp).second.begin( );
           its != (*itp).second.end( ); 
           its++ )
      {
        LDEBUG << "  threshold bewteen " << (*itp).first->morphVertex() << " and " << (*its).first->morphVertex() << " ; value: " << (*its).second;
        if ((*its).second<m_threshold)
        {
          LDEBUG << "  REMOVING ";
          (*itp).second.erase(its);
          // its is invalidated; reinitialize it
          its = (*itp).second.begin( );
        }
      }
    }
}

void CorefSolver::applyMorphoSyntacticFilter(  
  CoreferentAnnotation::WeightedVerticesRelation* pBinding,
  CoreferentAnnotation::VerticesRelation* roBinding) const
{
  COREFSOLVERLOGINIT;
  LDEBUG << "CorefSolver::applyMorphoSyntacticFilter binding size = " << pBinding->size();
  for (WeightedVerticesRelation::iterator itp = pBinding->begin( );
         itp != pBinding->end( ); 
         itp++ )
    {
      std::set<CoreferentAnnotation*> roSet = (*roBinding->find((*itp).first)).second;
      for (std::set<CoreferentAnnotation*>::iterator itro = roSet.begin( );
         itro != roSet.end( ); 
         itro++ )
      {
        LDEBUG << "  erasing " << (*itp).first->morphVertex() << " -> " << (*itro)->morphVertex();
        (*itp).second.erase(*itro);
      }
    }
}


} // closing namespace Coreferences
} // closing namespace LinguisticProcessing
} // closing namespace Lima
