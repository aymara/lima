/*
    Copyright 2002-2020 CEA LIST

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
* File        : recognizer.cpp
* Author      : Romaric Besancon (besanconr@zoe.cea.fr)
* Created on  : Tue Oct 15 2002
* Copyright   : (c) 2002 by CEA
*
************************************************************************/

#include "recognizer.h"

#include "transitionSearchStructure.h"
#include "automatonCommon.h"
#include "transitionUnit.h"
#include "recognizerData.h"
#include "common/tools/FileUtils.h"
#include "common/Data/LimaString.h"
#include "common/MediaticData/EntityType.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <stack>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

// a comparison operator on Rule pointer:
// to sort SetOfRules on decreasing rule weights
class CompareRulePtr {
public:
  bool operator()(Rule* r1,Rule* r2) {
    return (r1->getWeight() > r2->getWeight());
  }
};


// a comparison operator on TriggerRule
class Recognizer::CompareTriggerRule {
public:
  bool operator()(const Recognizer::TriggerRule* r1,
                  const Recognizer::TriggerRule* r2) {
    return (r1->setOfRules().front()->getWeight() >
            r2->setOfRules().front()->getWeight());
  }
};


/** recognizer factory */
SimpleFactory<AbstractResource,Recognizer> recognizerFactory(RECOGNIZER_CLASSID);

//**********************************************************************
// constructors
//**********************************************************************
Recognizer::Recognizer():
  AbstractResource(),
  m_rules(0),
  m_ruleStorage(0),
  m_language(),
  m_automatonControlParams(),
  m_filename(),
  m_searchStructure()
{ }

// copy is complex because of the pointers
Recognizer::Recognizer(const Recognizer& r):
AbstractResource(r)
{
  init();
  copy(r);

  // have to initialize the search structure of the new recognizer
  initializeSearchStructure();
}

//**********************************************************************
// destructor
//**********************************************************************
Recognizer::~Recognizer()
{
  freeMem();
  clearSearchStructure();
}

//**********************************************************************
// copy
//**********************************************************************
Recognizer& Recognizer::operator = (const Recognizer& r)
{
  if (this != &r)
  {
    freeMem();
    init();
    copy(r);
  }

  //  do not copy the search structure : recompute it the new recognizer
  // (not sure the copy is less complex than recomputing it)
  initializeSearchStructure();

  return (*this);
}

void Recognizer::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{

  /** @addtogroup ResourceConfiguration
   * - <b>&lt;group name="..." class="AutomatonRecognizer"&gt;</b>
   *    -  rules : file containing the compiled rules of the recognizer
   *    -  maxDepthStack : maximum size of stack in depth-first-search
   *        when testing a rule (default is 100)
   *    -  maxTransitionsExplored : max number of transitions explored
   *        when testing a rule (default is 1000)
   *    -  maxNbResults : max number of results temporarily stored
   *        when testing a rule (default is 50)
   *    -  maxResultSize : max size of a result for a rule match
   *        (this parameter can be seen as the effective size of
   *        "n" when using {0-n} in a rule) (default is 200)
   */

  m_language=manager->getInitializationParameters().language;
  string resourcesPath=Common::MediaticData::MediaticData::single().getResourcesPath();
  try
  {
    QString rulesFile = unitConfiguration.getParamsValueAtKey("rules").c_str();
    if (!rulesFile.isEmpty())
    {
      m_filename=rulesFile.toUtf8().constData();
      rulesFile = Common::Misc::findFileInPaths(resourcesPath.c_str(), rulesFile);
//       LDEBUG << "read recognizer from file : " << rulesFile;
      //readFromFile(rulesFile);
      AutomatonReader reader;
      reader.readRecognizer(rulesFile.toUtf8().constData(),*this);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    AULOGINIT;
    LERROR << "No param 'rules' in recognizer group for language " << (int)m_language;
    throw InvalidConfiguration();
  }

  try
  {
    string str=unitConfiguration.getParamsValueAtKey("maxDepthStack");
    uint64_t val=atol(str.c_str());
    if (val==0) {
      AULOGINIT;
      LWARN << "maxDepthStack is 0: keep default value";
    }
    else {
      m_automatonControlParams.setMaxDepthStack(val);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    //  keep default value
  }

  try
  {
    string str=unitConfiguration.getParamsValueAtKey("maxTransitionsExplored");
    uint64_t val=atol(str.c_str());
    if (val==0) {
      AULOGINIT;
    LWARN << "maxTransitionsExplored is 0: keep default value";
    }
    else {
      m_automatonControlParams.setMaxTransitionsExplored(val);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    //  keep default value
  }

  try
  {
    string str=unitConfiguration.getParamsValueAtKey("maxNbResults");
    uint64_t val=atol(str.c_str());
    if (val==0) {
      AULOGINIT;
    LWARN << "maxNbResults is 0: keep default value";
    }
    else {
      m_automatonControlParams.setMaxNbResults(val);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    //  keep default value
  }

  try
  {
    string str=unitConfiguration.getParamsValueAtKey("maxResultSize");
    uint64_t val=atol(str.c_str());
    if (val==0) {
      AULOGINIT;
      LWARN << "maxResultSize is 0: keep default value";
    }
    else {
      m_automatonControlParams.setMaxResultSize(val);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    //  keep default value
  }

  Common::MediaticData::MediaticData::changeable().stringsPool(m_language).endResourcesPool();
}

//**********************************************************************
// helper functions for constructors and destructors
//**********************************************************************
void Recognizer::init()
{
  m_rules.clear();
  m_ruleStorage.clear();
  m_language=UNDEFLANG;
  m_automatonControlParams=AutomatonControlParams();
}

void Recognizer::copy(const Recognizer& r)
{
  map<Rule*,Rule*> pointersMap;

  for (uint64_t i(0); i<r.m_ruleStorage.size(); i++)
  {
    Rule *rule = new Rule(*(r.m_ruleStorage[i]));
    m_ruleStorage.push_back(rule);
    pointersMap[r.m_ruleStorage[i]]=rule;
  }

  for (uint64_t i(0); i<r.m_rules.size(); i++)
  {
    TransitionUnit* t = r.m_rules[i].first->clone();
    m_rules.push_back(TriggerRule(t,SetOfRules(0)));
    for (uint64_t j(0); j<r.m_rules[i].second.size(); j++)
    {
      m_rules[i].second.push_back(pointersMap[r.m_rules[i].second[j]]);
    }
  }
  pointersMap.clear();

  m_language=r.m_language;
  m_automatonControlParams=r.m_automatonControlParams;
}

void Recognizer::freeMem()
{
  for (uint64_t i(0); i<m_ruleStorage.size(); i++)
  {
    delete m_ruleStorage[i];
    m_ruleStorage[i]=0;
  }
  m_ruleStorage.clear();
  for (uint64_t i(0); i<m_rules.size(); i++)
  {
    delete m_rules[i].first;
    m_rules[i].first=0;
    m_rules[i].second.clear();
  }
  m_rules.clear();
}

//**********************************************************************
// test the rules corresponding to one trigger on a text,
// at a given position
// stop at the first rule recognized
//**********************************************************************
/* TODO: tobe deleted?
bool Recognizer::parse(const LinguisticAnalysisStructure::Token&,
                       const LinguisticAnalysisStructure::AnalysisGraph& graph,
                       const LinguisticGraphVertex& current,
                       uint64_t offset,
                       AnalysisContent& analysis,
                       RecognizerMatch& result) const {

  vector<RecognizerMatch> results;
  if (testSetOfRules(*(m_rules[offset].first),
                     m_rules[offset].second,
                     graph,
                     current,
                     graph.firstVertex(),
                     graph.lastVertex(),
                     analysis,
                     results))
  {
    result=results.front(); // only one result because stopAtFirstSuccess=true
    return true;
  }
  return false;
}
*/

//**********************************************************************
// test a set of rules for a trigger
uint64_t Recognizer::testSetOfRules(const TransitionUnit& trigger,
                                        const SetOfRules& rules,
                                        const LinguisticAnalysisStructure::AnalysisGraph& graph,
                                        const LinguisticGraphVertex& position,
                                        const LinguisticGraphVertex& begin,
                                        const LinguisticGraphVertex& end,
                                        AnalysisContent& analysis,
                                        vector<RecognizerMatch>& matches,
                                        std::set<Common::MediaticData::EntityType>* forbiddenTypes,
                                        bool stopAtFirstSuccess,
                                        bool onlyOneSuccessPerType,
                                        bool applySameRuleWhileSuccess) const {
  AULOGINIT;
  // If the trigger is defined with a gazeteer, we must check the case of multi-term elements in the gazeteer
  const GazeteerTransition* gazeteerTrigger = dynamic_cast<const GazeteerTransition*>(&trigger);
  RecognizerMatch triggermatch(&graph);
  LinguisticGraphVertex right=position;
  if( gazeteerTrigger != 0 ) {
    Token* token = get(vertex_token, *(graph.getGraph()), position);
    MorphoSyntacticData* data = get(vertex_data, *(graph.getGraph()), position);
    deque<LinguisticGraphVertex> vertices;
    ForwardSearch searchGraph;
    bool match = gazeteerTrigger->matchPath(graph, position, end, &searchGraph, analysis, token, vertices, data);
    if( match ) {
      for( std::deque<LinguisticGraphVertex>::const_iterator vIt = vertices.begin(); vIt != vertices.end() ; vIt++ ) {
        triggermatch.addBackVertex(*vIt,trigger.keep(),"trigger");
      }
    }
  }
  else {
    triggermatch.addBackVertex(position,trigger.keep(),"trigger");
    right=position;
  }

  RecognizerMatch leftmatch(&graph);
  RecognizerMatch rightmatch(&graph);

  if (onlyOneSuccessPerType && forbiddenTypes==0) {
    LERROR << "Recognizer::testSetOfRules: cannot use onlyOneSuccessPerType "
           << "when forbidden types are not allowed";
    onlyOneSuccessPerType=false;
  }


  uint64_t nbSuccess(0);

  // left context is same LinguisticAnalysisStructure::AnalysisGraph as current (current is in fact
  // between the current token and the previous one)
  LinguisticGraphVertex left=position;

#ifdef DEBUG_LP
  LDEBUG << "Recognizer::testSetOfRules: testing set of rules triggered by " << trigger << " on vertex " << position;
    LDEBUG << "onlyOneSuccessPerType=" << onlyOneSuccessPerType;
  if (logger.isDebugEnabled()) {
   std::ostringstream oss;
   for (SetOfRules::const_iterator it=rules.begin(),it_end=rules.end();it!=it_end;it++) {
     oss << " - " << (*it)->getWeight();
   }
   LDEBUG << "Rule weights" << oss.str();
  }
#endif

  bool reapplySameRule(false);

  SetOfRules::const_iterator
    rule=rules.begin(),
    rule_end=rules.end();
  for (; rule!=rule_end; rule++) {
    Rule* currentRule=*rule;

#ifdef DEBUG_LP
    if (logger.isDebugEnabled()) {
      LDEBUG << "Recognizer::testSetOfRules: testing rule "<<*currentRule << "," << currentRule->getRuleId() <<" of type "
             << currentRule->getType() << ",reapply="
             << reapplySameRule << " from " << position;
    }
#endif

    if (forbiddenTypes &&
        forbiddenTypes->find(currentRule->getType())
        != forbiddenTypes->end()) {
      // type previously forbidden by a negative rule
/*      LDEBUG << "type " << currentRule->getType()
             << " is forbidden: continue";*/
      continue;
    }

    // initializes the constraint checklist
    ConstraintCheckList
      constraintCheckList(currentRule->numberOfConstraints(),
                          ConstraintCheckListElement(graph));

    // treat the constraints for the trigger with the constraint
    // checklist corresponding to this rule
    //Token* token=get(vertex_token,*(graph.getGraph()),position);
//     LDEBUG << "Recognizer: checking trigger constraints: ";

    if (!trigger.checkConstraints(graph,position,analysis,
                                  constraintCheckList)) {
      // one unary constraint was not verified
//       LDEBUG << "one unary constraint on trigger not verified";

    // apply actions (for actions triggered by failure)
      if (!currentRule->negative()) {
        currentRule->executeActions(graph, analysis,
                                    constraintCheckList,
                                    false,
                                    0); // match is not used
//     LDEBUG << "actionSuccess=" << actionSuccess;
      }
      continue;
    }

    leftmatch.reinit();
    rightmatch.reinit();
    ForwardSearch forward;
    BackwardSearch backward;
    bool success = currentRule->test(graph, left, right,
                                     begin, end, analysis,
                                     leftmatch, rightmatch,
                                     constraintCheckList,forward,backward,
                                     m_automatonControlParams);
    //LDEBUG << "success=" << success;

    RecognizerMatch* match=0;

    if (success) {
      // build complete match

      match=new RecognizerMatch(leftmatch);
      if (leftmatch.getHead() != 0) {
        match->setHead(leftmatch.getHead());
      }

      // TODO: add node of gazeteerTrigger
      //match->addBackVertex(position,trigger.keep(), "trigger");
      /*
      RecognizerMatch::const_iterator triggerMatchIt = triggermatch.begin();
      for( ; triggerMatchIt != triggermatch.end(); triggerMatchIt++) {
        match->addBackVertex(*triggerMatchIt,trigger.keep(), "trigger");
      }
      */
      match->addBack(triggermatch);
      match->addBack(rightmatch);
      // remove elements not kept at begin and end of the expression
      match->removeUnkeptAtExtremity();

      // check if trigger is head
      match->setType(currentRule->getType());
      match->setLinguisticProperties(currentRule->getLinguisticProperties());
      match->setContextual(currentRule->contextual());
      setNormalizedForm(currentRule->getNormalizedForm(),*match);
    }

    // execute possible actions associated to the rule iff current rule is
    // positive
    //LDEBUG << "Recognizer: executing actions: ";
    bool actionSuccess = true;
    if (!currentRule->negative()) {
      actionSuccess = currentRule->executeActions(graph, analysis,
                                                  constraintCheckList,
                                                  success,
                                                  match);

      //LDEBUG << "actionSuccess=" << actionSuccess;
    }

#ifdef DEBUG_LP
    if (logger.isDebugEnabled()) {
      LinguisticGraphVertex v=position;
      LimaString str("");
      Token* token=get(vertex_token,*(graph.getGraph()),position);
      if (token!=0) {
        str = token->stringForm();
      }
      if (success) {
        LDEBUG << "Recognizer::testSetOfRules: trigger " << v << "[" << str << "]:rule "
               << currentRule->getRuleId() << "-> success=" << success
               << ",actionSuccess=" << actionSuccess;
        LDEBUG << "        matched:" << match->getNormalizedString(Common::MediaticData::MediaticData::single().stringsPool(m_language));
      }
      else {
        LDEBUG << "Recognizer::testSetOfRules: vertex " << v << "[" << str << "]:rule "
               << currentRule->getRuleId() << "-> success= false";
      }
    }
#endif

    if (success && actionSuccess) {
      if (forbiddenTypes && currentRule->negative()) {
        forbiddenTypes->insert(currentRule->getType());
        success = false;
        delete match;
        match=0;
        continue;
      }
      LINFO << "Recognizer::testSetOfRules: execute rule " << currentRule->getRuleId()
            << " of type "<< currentRule->getType()
            << "(" <<  Lima::Common::MediaticData::MediaticData::single().getEntityName(currentRule->getType())
            << ") on vertex " << position;
      RecognizerData* recoData = static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
      if (stopAtFirstSuccess||(recoData != nullptr && !recoData->getNextVertices().empty())) {
        matches.push_back(*match);
        delete match; // a copy has been made
        match=0;
#ifdef DEBUG_LP
        if (logger.isDebugEnabled() && recoData != nullptr) {
          LDEBUG << "Recognizer::testSetOfRules: Returning from testSetOfRules cause stopAtFirstSuccess ("
            << stopAtFirstSuccess << ") or next vertices empty ("
            << (recoData->getNextVertices().empty())
            << ")";
        }
#endif
        return 1;
      }
      else {
        if (applySameRuleWhileSuccess) {
          if (reapplySameRule) {
            if (*match==matches.back()) {
//               AULOGINIT;
//               LDEBUG << "Reapplication of same rule gives same result: "
//                     << "abort to avoid inifinite loop: "
//                     << *match << ";" << matches.back();
              delete match; // a copy has been made
              match=0;
              reapplySameRule=false;
              continue;
            }
/*            else {
              LDEBUG << "Reapplication of same rule gives new result";
            }*/
          }
          // reapply same rule
          rule--;
          reapplySameRule=true;
        }

//         LDEBUG << "add match to results " << *match;
        matches.push_back(*match);
        delete match; // a copy has been made
        match=0;

        if (onlyOneSuccessPerType) {
/*          LDEBUG << "add " << currentRule->getType()
                 << " in forbiddenTypes";*/
          forbiddenTypes->insert(currentRule->getType());
        }
        nbSuccess++;
      }
    }
    else {
//      LDEBUG << "-> no success";
      reapplySameRule=false;
    }

    if (match !=0) {
      delete match;
    }
  }

  return nbSuccess;
}

//**********************************************************************
// normalization function
//**********************************************************************
void Recognizer::
setNormalizedForm(const LimaString& norm,
                  RecognizerMatch& match) const
{
  match.features().clear();

  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);
  if (norm.isEmpty()) {
    // use surface form of the expression as normalized form
    match.features().setFeature(DEFAULT_ATTRIBUTE,match.getNormalizedString(sp));
  }
  else {
    match.features().setFeature(DEFAULT_ATTRIBUTE,norm);
  }
}

//**********************************************************************
// main functions that applies the recognizer on a graph
//**********************************************************************

// Apply between two nodes and search between the same ones
uint64_t Recognizer::
  apply(const LinguisticAnalysisStructure::AnalysisGraph& graph,
        const LinguisticGraphVertex& begin,
        const LinguisticGraphVertex& end,
        AnalysisContent& analysis,
        std::vector<RecognizerMatch>& result,
        bool testAllVertices,
        bool stopAtFirstSuccess,
        bool onlyOneSuccessPerType,
        bool returnAtFirstSuccess,
        bool applySameRuleWhileSuccess) const
{
  return apply(graph,
               begin,
               end,
               begin,
               end,
               analysis,
               result,
               testAllVertices,
               stopAtFirstSuccess,
               onlyOneSuccessPerType,
               returnAtFirstSuccess,
               applySameRuleWhileSuccess);
}

// Apply between two nodes and search between two others.
// precondition [begin, end] included in [upstreamBound,downstreamBound]
uint64_t Recognizer::
  apply(const LinguisticAnalysisStructure::AnalysisGraph& graph,
        const LinguisticGraphVertex& begin,
        const LinguisticGraphVertex& end,
        const LinguisticGraphVertex& upstreamBound,
        const LinguisticGraphVertex& downstreamBound,
        AnalysisContent& analysis,
        std::vector<RecognizerMatch>& result,
        bool testAllVertices,
        bool stopAtFirstSuccess,
        bool onlyOneSuccessPerType,
        bool returnAtFirstSuccess,
        bool applySameRuleWhileSuccess) const
{
  SetOfLinguisticGraphVertices nodesAfterEnd = getFollowingNodes<SetOfLinguisticGraphVertices>(graph, end);
  if (begin == end)
    return 0;

  if (returnAtFirstSuccess) {
    stopAtFirstSuccess=true; // implied by the other
  }

#ifdef DEBUG_LP
  AULOGINIT;
  LDEBUG << "apply recognizer " << m_filename << " from vertex "
         << begin << " to vertex " << end;
  LDEBUG << "  up bound: " << upstreamBound << "; down bound: " << downstreamBound << "; testAllVertices: " << testAllVertices;
  LDEBUG << "  stopAtFirstSuccess: " << stopAtFirstSuccess << "; onlyOneSuccessPerType: " << onlyOneSuccessPerType;
  LDEBUG << "  returnAtFirstSuccess: " << returnAtFirstSuccess << "; applySameRuleWhileSuccess: " << applySameRuleWhileSuccess;
#endif

  uint64_t numberOfRecognized(0);
  bool success(false);

  // use deque instead of queue to be able to clear()
  std::deque<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;

  for (auto v : getFollowingNodes<SetOfLinguisticGraphVertices>(graph, begin))
  {
    if (v != end)
      toVisit.push_back(v);
  }

  // patch for inifinite loop : avoid begin stopped at first step
  //visited.insert(begin);

  bool lastReached = false;
  while (!toVisit.empty())
  {
    LinguisticGraphVertex currentVertex=toVisit.front();
    toVisit.pop_front();
    // patch for inifinite loop : check if we already seen this node
    if (visited.find(currentVertex) != visited.end())
    {
      continue;
    }

    visited.insert(currentVertex);
#ifdef DEBUG_LP
    LDEBUG << "to visit size=" << toVisit.size() << " ; currentVertex=" << currentVertex;
#endif

    if (lastReached ||                // limit given by argument
        currentVertex == graph.lastVertex()) { // end of the graph
      // LDEBUG << "vertex " << currentVertex << " is last vertex";
      continue;  // may be other nodes to test in queue
    }
    if (currentVertex == end ) {                // limit given by argument
      lastReached = true;
    }

    if (currentVertex != graph.firstVertex()) {
#ifdef DEBUG_LP
      LDEBUG << "Recognizer: test on vertex " << currentVertex;
#endif
      success = testOnVertex(graph,currentVertex,
                             upstreamBound,downstreamBound,
                             analysis,result,
                             stopAtFirstSuccess,
                             onlyOneSuccessPerType,
                             applySameRuleWhileSuccess);
      if (success) {
        numberOfRecognized++;
        if (returnAtFirstSuccess)
          return numberOfRecognized;
        if (! testAllVertices) { // restart from end of recognized expression
#ifdef DEBUG_LP
          LDEBUG << "success: continue from vertex " << currentVertex;
#endif
          // GC on 20110803: the clearing below was problematic in case of rules like that:
          // [<Location.LOCATION>]:(t_capital_1st|t_capital){1-3} [,]::LOCATION:N_LOCATION
          // which matches text before (left) the trigger which is not included in the match.
          // thus the next vertex explored was the newly created one ; the vertex following
          // it is already visited (this is in this case the comma) and the content of
          // toVisit (the vertex after the trigger) was removed. Thus the search stopped after
          // the new vertex.
          // Warning: what is the inpact on the use of the testAllVertices parameter ? And is there
          // any other side effect ?
//           toVisit.clear();

        }

        // TODO: update downstreamBound if end node changed
      }
    }

    // store following nodes to test
    LinguisticGraphOutEdgeIt outEdge,outEdge_end;
    boost::tie (outEdge,outEdge_end)=out_edges(currentVertex,*(graph.getGraph()));
    for (; outEdge!=outEdge_end; outEdge++) {
      LinguisticGraphVertex next=target(*outEdge,*(graph.getGraph()));
      if (visited.find(next)==visited.end()) {
#ifdef DEBUG_LP
        LDEBUG << "Recognizer: adding out edge target vertex to the 'to visit' list: " << next;
#endif
        if (nodesAfterEnd.end() == nodesAfterEnd.find(next))
          toVisit.push_back(next);
        // do not put in visited unless it is really visited
        // (otherwise, may be suppressed when testAllVertices is false
        // and never visited)
        //visited.insert(next);
      }
      else {
#ifdef DEBUG_LP
        LDEBUG << "Recognizer: already visited:" << next;
#endif
      }
    }
    RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
    if (nullptr != recoData)
    {
      std::set<LinguisticGraphVertex>& nextVertices = recoData->getNextVertices();
      if (!nextVertices.empty())
      {
  #ifdef DEBUG_LP
        LDEBUG << "Recognizer: adding next vertices to the 'to visit' list";
  #endif
        std::set< LinguisticGraphVertex >::const_iterator nvit, nvit_end;
        nvit = nextVertices.begin();
        nvit_end = nextVertices.end();
        for (; nvit != nvit_end; nvit++)
        {
  #ifdef DEBUG_LP
          LDEBUG << "           - " << *nvit;
  #endif
          if (nodesAfterEnd.end() == nodesAfterEnd.find(*nvit))
            toVisit.push_front(*nvit);
        }
        nextVertices.clear();
      }
    }
#ifdef DEBUG_LP
    LDEBUG << "Recognizer: 'to visit' list size is now: " << toVisit.size();
#endif
  }
  return numberOfRecognized;
}


//**********************************************************************
// test the recognizer on a vertex : test
//**********************************************************************
uint64_t Recognizer::
testOnVertex(const LinguisticAnalysisStructure::AnalysisGraph& graph,
             LinguisticGraphVertex& current,
             const LinguisticGraphVertex& begin,
             const LinguisticGraphVertex& end,
             AnalysisContent& analysis,
             std::vector<RecognizerMatch>& result,
             bool stopAtFirstSuccess,
             bool onlyOneSuccessPerType,
             bool applySameRuleWhileSuccess) const
{
  //AULOGINIT;
  Token* token = get(vertex_token, *(graph.getGraph()), current);
  MorphoSyntacticData* data = get(vertex_data, *(graph.getGraph()), current);

  if (token==0) {
    AULOGINIT;
    LERROR << "no token for vertex " << current;
    return 0;
  }

  if (data==0) {
    AULOGINIT;
    LERROR << "no data for vertex " << current;
    return 0;
  }

  vector<TriggerRule*> matchingRules;
  set<Common::MediaticData::EntityType> forbiddenTypes;
  uint64_t nbSuccess=0;

  findNextSetOfRules(graph, current, analysis, token, data, matchingRules);

  if (! matchingRules.empty()) {
    std::vector<TriggerRule*>::const_iterator
      ruleSet=matchingRules.begin(),
      ruleSet_end=matchingRules.end();
    for (; ruleSet!=ruleSet_end; ruleSet++) {
      uint64_t nbSuccessForTheseRules=
        testSetOfRules(*((*ruleSet)->transitionUnit()),
                       (*ruleSet)->setOfRules(),
                       graph, current, begin, end, analysis,
                       result, &forbiddenTypes,
                       stopAtFirstSuccess,
                       onlyOneSuccessPerType,
                       applySameRuleWhileSuccess);
      if (nbSuccessForTheseRules>0) {
        nbSuccess+=nbSuccessForTheseRules;
        // skip recognized part (if the end of the recognized part is after
        // current token)
        RecognizerMatch& lastSuccess=result.back();
        Token* t=get(vertex_token,*(graph.getGraph()),current);
        uint64_t currentTokenEnd=t->position()+t->length();
        RecognizerData* recoData = static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
        if (stopAtFirstSuccess||(recoData != 0 && !recoData->getNextVertices().empty())) {
          if (lastSuccess.positionEnd() >= currentTokenEnd) {
            current=lastSuccess.getEnd();
          }
          break;
        }
      }
    }
    for(std::vector<TriggerRule*>::iterator it=matchingRules.begin(),
      it_end=matchingRules.end(); it!=it_end; it++) {
       if (*it!=0) {
        delete (*it);
       }
    }
  }
  forbiddenTypes.clear();

  // LDEBUG << "testOnVertex nb successes: " << nbSuccess;
  return nbSuccess;
}

//**********************************************************************
//resolve the problem of overlapping entities in the list of entities :
// when two entities are overlaping, only one is kept
//**********************************************************************
uint64_t Recognizer::
resolveOverlappingEntities(std::vector<RecognizerMatch>& listEntities,
                           const OverlapResolutionStrategy& strategy) const
{
  typedef std::vector<RecognizerMatch>::iterator vectorRecognizerMatchIterator;

  uint64_t numberOfOverlappingEntities(0);

  if (listEntities.empty()) {
    return numberOfOverlappingEntities;
  }

  switch (strategy) {
  case IGNORE_FIRST:  {
    vectorRecognizerMatchIterator currentEntity(listEntities.begin());
    vectorRecognizerMatchIterator nextEntity(currentEntity);
    nextEntity++;
    while (nextEntity != listEntities.end()) {
      if (currentEntity->isOverlapping(*nextEntity)) {
    numberOfOverlappingEntities++;
    currentEntity=listEntities.erase(currentEntity);
    nextEntity=currentEntity;
    nextEntity++;
      }
      else {
    currentEntity++;
    nextEntity++;
      }
    }
    break;
  }
  case IGNORE_SECOND:  {
    vectorRecognizerMatchIterator currentEntity(listEntities.begin());
    vectorRecognizerMatchIterator previousEntity(currentEntity);
    currentEntity++;
    while (currentEntity != listEntities.end()) {
      if (currentEntity->isOverlapping(*previousEntity)) {
    numberOfOverlappingEntities++;
    currentEntity=listEntities.erase(currentEntity);
      }
      else {
    previousEntity++;
    currentEntity++;
      }
    }
    break;
  }
  case IGNORE_SMALLEST:  {
    vectorRecognizerMatchIterator currentEntity(listEntities.begin());
    vectorRecognizerMatchIterator previousEntity(currentEntity);
    currentEntity++;
    while (currentEntity != listEntities.end()) {
      if (currentEntity->isOverlapping(*previousEntity)) {
    numberOfOverlappingEntities++;
    if (currentEntity->numberOfElements()
        < previousEntity->numberOfElements()) { // keep previous entity
      currentEntity=listEntities.erase(currentEntity);
    }
    else { // keep current entity
      previousEntity=listEntities.erase(previousEntity);
      currentEntity=previousEntity;
      currentEntity++;
    }
      }
      else {
    previousEntity++;
    currentEntity++;
      }
    }
    break;
  }
  default:
    break;
  }

  return numberOfOverlappingEntities;
}

//**********************************************************************
// find the set of rules in the recognizer that accept
// a particular token as trigger
//**********************************************************************
void Recognizer::
findNextSetOfRules(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                   LinguisticGraphVertex& vertex,
                   AnalysisContent& analysis,
                   const LinguisticAnalysisStructure::Token* token,
                   const LinguisticAnalysisStructure::MorphoSyntacticData* data,
                   std::vector<TriggerRule*>& matchingSetOfRules) const
{
  matchingSetOfRules.clear();

  // find matching rules
  std::vector<const TriggerRule*> matchingRules;
  m_searchStructure.findMatchingTransitions(graph,vertex,analysis,token,data,matchingRules);

  // matching rules are gathered by common trigger (transition unit)
  // we have to re-sort the rules by their weight at a global level, independently of the trigger
  // create a vector of TriggerRule where each contains only one rule, then sort it
  for (std::vector<const TriggerRule*>::const_iterator it=matchingRules.begin(),it_end=matchingRules.end();it!=it_end;it++) {
    for (SetOfRules::const_iterator r=(*it)->setOfRules().begin(),r_end=(*it)->setOfRules().end(); r!=r_end;r++) {
      matchingSetOfRules.push_back(new TriggerRule((*it)->transitionUnit(),SetOfRules(1,*r)));
    }
  }
  sort(matchingSetOfRules.begin(),matchingSetOfRules.end(),CompareTriggerRule());

  // then, gather rules with the same trigger that are consecutive in this new list
  // (may save some constraint checking on trigger)
  if (! matchingSetOfRules.empty()) {
    std::vector<TriggerRule*>::iterator it=matchingSetOfRules.begin();
    TransitionUnit* currentTrigger=(*it)->transitionUnit();
    std::vector<TriggerRule*>::iterator next=it;
    next++;
    while (next!=matchingSetOfRules.end()) {
      if ((*next)->transitionUnit() == currentTrigger) {
        (*it)->second.push_back((*next)->setOfRules().front());
        delete *next;
        next=matchingSetOfRules.erase(next);
      }
      else {
        it++;
        currentTrigger=(*it)->transitionUnit();
        next++;
      }
    }
  }
}

void Recognizer::initializeSearchStructure() {
  const Common::PropertyCode::PropertyAccessor* macro=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  const Common::PropertyCode::PropertyAccessor* micro=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  m_searchStructure.init(m_rules,macro,micro);
}

void Recognizer::clearSearchStructure() {
  m_searchStructure.clear();
}

//**********************************************************************
// adding a rule
//**********************************************************************
uint64_t Recognizer::addRuleInStorage(Rule* rule)
{
  // add the rule in the storage
  m_ruleStorage.push_back(rule);
  // return the index of the rule in the storage
  return (m_ruleStorage.size() - 1);
}

uint64_t Recognizer::addRule(TransitionUnit* trigger, Rule* rule)
{
  uint64_t indexRule=addRuleInStorage(rule);

  // find if the trigger already exists in the set of triggers
  for (uint64_t i(0); i<m_rules.size(); i++)
  {
    if (*(m_rules[i].first) == *trigger)
    {
      m_rules[i].second.push_back(rule);
      return indexRule;
    }
  }
  m_rules.push_back(TriggerRule(trigger->clone(),
                                SetOfRules(1,rule)));

  return indexRule;
}

void Recognizer::addRule(TransitionUnit* trigger,
                         const uint64_t index)
{
  // find if the trigger already exists in the set of triggers
  for (uint64_t i(0); i<m_rules.size(); i++)
  {
    if (*(m_rules[i].first) == *trigger)
    {
      m_rules[i].second.push_back(m_ruleStorage[index]);
      return;
    }
  }
  m_rules.push_back(TriggerRule(trigger->clone(),
                                SetOfRules(1,m_ruleStorage[index])));
}

//**********************************************************************
// input/output in a binary format
//**********************************************************************
// void Recognizer::readFromTextFile(std::string filename) {
//   RecognizerCompiler::buildRecognizer(*this,filename);
// }

// simple linear search (called only with write function -> not optimized)
uint64_t Recognizer::findRuleIndex(Rule* r) const
{
  for (uint64_t i(0); i<m_ruleStorage.size(); i++)
  {
    if (m_ruleStorage[i] == r)
    {
      return i;
    }
  }
  return m_ruleStorage.size()+1;
}

//**********************************************************************
// output the list of triggers with their associated index
//**********************************************************************
void Recognizer::listTriggers() const
{
  for (uint64_t i(0); i<m_rules.size(); i++)
  {
    cout << "<k>" << m_rules[i].first->printValue() << "</k>"
         << "<o>" << i << "</o>" << endl;
  }
}

//***************************************************************************
// output
//***************************************************************************
ostream& operator << (ostream& os, const Recognizer& r)
{
  for (uint64_t i(0); i<r.m_rules.size(); i++)
  {
    os << "trigger "<< i << " = "
      << *(r.m_rules[i].first) << endl;
    for (uint64_t j(0); j<r.m_rules[i].second.size(); j++)
    {
      os << "rule " << j << ":"
      << *(r.m_rules[i].second[j]);
    }
  }
  return os;
}

} // namespace end
} // namespace end
} // namespace end
