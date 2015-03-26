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
* File        : automaton.cpp
* Author      : Romaric Besan�n (besanconr@zoe.cea.fr)
* Created on  : Fri Oct 04 2002
* Copyright   : (c) 2002 by CEA
*
************************************************************************/


#include "automaton.h"
#include "epsilonTransition.h"
#include "common/Data/readwritetools.h"
#include <boost/tuple/tuple.hpp> // for tie
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// a class for control parameters for the search using the automata

#define DEFAULT_MAXDEPTHSTACK 100
#define DEFAULT_MAXTRANSITIONSEXPLORED 1000
#define DEFAULT_MAXNBRESULTS 50
#define DEFAULT_MAXRESULTSIZE 200

AutomatonControlParams::AutomatonControlParams():
m_maxDepthStack(DEFAULT_MAXDEPTHSTACK),
m_maxTransitionsExplored(DEFAULT_MAXTRANSITIONSEXPLORED),
m_maxNbResults(DEFAULT_MAXNBRESULTS),
m_maxResultSize(DEFAULT_MAXRESULTSIZE) 
{
}

AutomatonControlParams::~AutomatonControlParams() 
{
}

/***********************************************************************/
// constructors
/***********************************************************************/
Automaton::Automaton():
  m_numberStates(0),
  m_finalStates(0),
  m_transitions(0),
  m_searchStructures(0),
  m_deterministic(false)
{
}

Automaton::Automaton(const Tstate nbStates):
  m_numberStates(nbStates),
  m_finalStates(nbStates,false),
  m_transitions(nbStates), 
  m_searchStructures(nbStates,0),
  m_deterministic(false)
{
}

Automaton::Automaton(const Automaton& a) {
  init();
  copy(a);
}

/***********************************************************************/
// destructor
/***********************************************************************/
Automaton::~Automaton() {
  freeMem();
}

/***********************************************************************/
// assignment operator
/***********************************************************************/
Automaton& Automaton::operator= (const Automaton& a) {
  if (this != &a) {
    freeMem();
    init();
    copy(a);
  }
  return *this;
}

//**********************************************************************
// helper functions for constructors and destructors
void Automaton::init() 
{
  m_numberStates=0;
  m_finalStates.clear();
  m_transitions.clear();
  m_searchStructures.clear();
  m_deterministic=false;
}

void Automaton::copy(const Automaton& a) 
{
  m_numberStates=a.numberOfStates();
  m_finalStates=a.m_finalStates;
  m_transitions=a.m_transitions;
  m_deterministic=a.m_deterministic;

  // clone search structures if necessary
  m_searchStructures.clear();
  std::vector<TransitionSearchStructure<Transition>*>::const_iterator
    it=a.m_searchStructures.begin(),
    it_end=a.m_searchStructures.end();
  for (; it!=it_end;it++) {
    if ((*it)!=0) {
      m_searchStructures.
        push_back(new TransitionSearchStructure<Transition>(**it));
    }
    else {
      m_searchStructures.push_back(static_cast<TransitionSearchStructure<Transition>*>(0));
    }
  }
}

void Automaton::freeMem() 
{
  m_finalStates.clear();
  m_transitions.clear();
  std::vector<TransitionSearchStructure<Transition>*>::iterator
    it=m_searchStructures.begin(),
    it_end=m_searchStructures.end();
  for (; it!=it_end;it++) {
    if ((*it)!=0) {
      delete (*it);
      *it=0;
    }
  }
  m_searchStructures.clear();
}

void Automaton::reinit() {
  m_numberStates=0;
  freeMem();
}

/***********************************************************************/
// access functions
/***********************************************************************/

uint64_t Automaton::numberOfTransitions() const {
  uint64_t nbTrans(0);
  for (uint64_t i(0); i<m_numberStates; i++) {
    nbTrans+=m_transitions[i].size();
  }
  return nbTrans;
}

vector<Tstate> Automaton::finalStates() const {
  vector<Tstate> finals(0);
  for (uint64_t i(0); i<m_numberStates; i++) {
    if (m_finalStates[i]) {
      finals.push_back(i); 
    }
  }
  return finals;
}

bool Automaton::hasTransitionsState(const Tstate state) const {
  if (m_searchStructures[state]==0) {
//    AULOGINIT;
//    LDEBUG << "search structure not initialized";
    return (! m_transitions[state].empty());
  }
  else {
    return (! m_searchStructures[state]->empty());
  }
}

/***********************************************************************/
// to test the automaton
/***********************************************************************/

//***************************************************************************
// indicates if it exists at least one path leading to a final state 
// that contains only epsilon transitions
bool Automaton::existsEpsilonPathToFinal(const Tstate state) const {
  Tstate currentState(state);
  if (isFinalState(currentState)) { return true; }
  for (uint64_t i(0); i<m_transitions[currentState].size(); i++) {
    if (m_transitions[currentState][i].transitionUnit()->isEpsilonTransition()) {
      if (existsEpsilonPathToFinal(m_transitions[currentState][i].nextState())) {
        return true;
      }
    }
  }
  return false;
}

void Automaton::initializeSearchStructures(MediaId language) {
  const Common::PropertyCode::PropertyAccessor* macro=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  const Common::PropertyCode::PropertyAccessor* micro=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  for (uint64_t i(0); i<m_numberStates; i++) {
    if (m_searchStructures[i]==0) {
      m_searchStructures[i]=new TransitionSearchStructure<Transition>();
    }
    else {
      m_searchStructures[i]->clear();
    }
    m_searchStructures[i]->init(m_transitions[i],macro,micro);
  }
  m_transitions.clear();
}

bool Automaton::
getMatchingTransitions(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                       const LinguisticGraphVertex& vertex, 
                       AnalysisContent& analysis,
                       const Tstate& state,
                       std::vector<const Transition*>& 
                       matchingTransitions) const {
  Token* token = get(vertex_token, *(graph.getGraph()), vertex);
  MorphoSyntacticData* data = get(vertex_data, *(graph.getGraph()), vertex);

  if (m_searchStructures[state]==0) {
//    AULOGINIT;
//    LDEBUG << "search structure not initialized: linear search";
    //linear search on the transitions
    matchingTransitions.clear();
    vector<Transition>::const_iterator
      trans=m_transitions[state].begin(),
      trans_end=m_transitions[state].end();

    for (; trans!=trans_end; trans++) {
//       LDEBUG << "Automaton::getMatchingTransitions vertex: " << vertex;
      bool match=(*trans).transitionUnit()->compare(graph,vertex,analysis,token,data);
      if ((*trans).transitionUnit()->negative()) {
        match = (!match);
      }
      if (match) {
        matchingTransitions.push_back(&(*trans));
      }
    }
    return (!matchingTransitions.empty());
  }
  else {
    return m_searchStructures[state]->
      findMatchingTransitions(graph,vertex,analysis,token,data,matchingTransitions);
  }
}

//**********************************************************************
// main function: test the automaton on a morphological graph
//***********************************************************************
// comparison operator for elements of AutomatonMatchSet
bool Automaton::CompareAutomatonMatch::
operator()(const AutomatonMatch& r1, 
           const AutomatonMatch& r2) const {
  // operator > : first result is result with more coverage
  const RecognizerMatch& m1=r1.first;
  const RecognizerMatch& m2=r2.first;
  
  // number of kept elements
  uint64_t nbElt1=m1.numberOfElements();
  uint64_t nbElt2=m2.numberOfElements();
  
  if (nbElt1 > nbElt2) {
    return true;
  }
  if (nbElt1 == nbElt2) {
    // use size as second criteria
    uint64_t size1=m1.size();
    uint64_t size2=m2.size();
    if (size1 > size2) {
      return true;
    }
    else if (size1 == size2) {
      // use vertex numbers as last criteria
      // take greater vertex first (should be the last tested)
      for (uint64_t i(0); i<size1; i++) {
        if (m1[i].getVertex() > m2[i].getVertex()) {
          return true;
        }
        if (m1[i].getVertex() < m2[i].getVertex()) {
          return false;
        }
      }
    }
  }
  return false;
}

// internal definition of a utility class: 
// stack for DFS test function 
class Automaton::DFSStack {
public:
  DFSStack(const Automaton& a,
           const LinguisticAnalysisStructure::AnalysisGraph& graph,
           SearchGraph* searchGraph,
           const LinguisticGraphVertex& limit):
    m_stack(),
    m_automaton(a),
    m_graph(graph),
    m_searchGraph(searchGraph),
    m_limit(limit) {}

  ~DFSStack() {}
  
  uint64_t size() const { return m_stack.size(); }
  bool empty() const { return m_stack.empty(); }
  bool isLimitVertex(const LinguisticGraphVertex& v) const 
  { return (v==m_limit);}

  bool isEndVertex(const LinguisticGraphVertex& v) const 
  { return (v==m_searchGraph->endOfGraph(m_graph)); }

  std::pair<LinguisticGraphVertex,const Transition*> top();
  void popVertex();
  bool pop();
  bool push(const LinguisticGraphVertex& vertex,
            const Tstate& state,
            AnalysisContent& analysis);
private:
  struct DFSStackElement {
    DFSStackElement(LinguisticGraphVertex v,
                    const std::vector<const Transition*>& t):
      m_vertex(v),
      m_transitions(t),
      m_transition(t.begin()) 
    {
    }

    DFSStackElement(const DFSStackElement& elt):
      m_vertex(elt.m_vertex),
      m_transitions(elt.m_transitions),
      m_transition(m_transitions.begin()) 
    {
    }

    ~DFSStackElement() {}

    LinguisticGraphVertex m_vertex;
    std::vector<const Transition*> m_transitions;
    std::vector<const Transition*>::const_iterator m_transition;
  };
  std::vector<DFSStackElement> m_stack;
  const Automaton& m_automaton;
  const LinguisticAnalysisStructure::AnalysisGraph& m_graph;
  SearchGraph* m_searchGraph;
  LinguisticGraphVertex m_limit;
};

std::pair<LinguisticGraphVertex,const Transition*> 
Automaton::DFSStack::top() {
//   AULOGINIT;
//   LDEBUG << "Automaton:DFSSTack: top "
//          << "transition=" << *(m_stack.back().m_transition)
//          << ";transitionUnit=" 
//          << (*(m_stack.back().m_transition))->transitionUnit()
//         ;
  return make_pair(m_stack.back().m_vertex,
                   *(m_stack.back().m_transition));
}

bool Automaton::DFSStack::pop() {
//   AULOGINIT;
//   LDEBUG << "Automaton:DFSSTack: poping ";
  m_stack.back().m_transition++;
  if (m_stack.back().m_transition==
      m_stack.back().m_transitions.end()) {
//     LDEBUG << "Automaton:DFSSTack: end of transitions: poping vertex "
//           ;
    m_stack.pop_back();
    return true;
  }
  return false;
}

void Automaton::DFSStack::popVertex() {
  m_stack.pop_back();
}

bool Automaton::DFSStack::
push(const LinguisticGraphVertex& vertex,
     const Tstate& state,
     AnalysisContent& analysis) {

/*  AULOGINIT;
  LDEBUG << "Automaton:DFSSTack: pushing " << vertex
          << ";" << state;*/
  
  if (isLimitVertex(vertex)) { 
    return false;
  }

  if (! m_automaton.hasTransitionsState(state)) {
    return false;
  }

  // use temporary stack to reverse elements
  // (not efficient but to test similariry with previous version)
  std::vector<DFSStackElement> tmpStack;

  // look at next vertices (defined by the searchGraph strategy)
  m_searchGraph->findNextVertices(m_graph.getGraph(),vertex);
  LinguisticGraphVertex nextVertex;
  while (m_searchGraph->getNextVertex(m_graph.getGraph(),nextVertex)) {
    if (! isEndVertex(nextVertex)) {
      std::vector<const Transition*> matchingTransitions(0);
//       LDEBUG << "Automaton:get matching transitions from state "
//              << state << " for vertex " << nextVertex;
      if (m_automaton.
          getMatchingTransitions(m_graph,nextVertex,analysis,
                                 state,matchingTransitions)) {
/*        if (logger.isDebugEnabled()) {
          ostringstream oss;
          std::vector<const Transition*>::const_iterator 
            it=matchingTransitions.begin(),
            it_end=matchingTransitions.end();
          oss << "Automaton:DFSSTack: matching transitions = ";
          for (;it!=it_end;it++) {
            oss << **it << ";";
          }
          LDEBUG << oss.str();
        }*/
        tmpStack.push_back(DFSStackElement(nextVertex,matchingTransitions));
      }
/*      else {
        LDEBUG << "Automaton:DFSSTack: => no matching transitions" 
              ;
      }*/
    }
  }
  // clear search structure for this vertex
  m_searchGraph->clear();

  if (tmpStack.empty()) {
    return false;
  } else {
    m_stack.insert(m_stack.end(),tmpStack.rbegin(),tmpStack.rend());
  }
  // reverse stacked elements
//  while (!tmpStack.empty()) {
//    m_stack.push_back(tmpStack.top());
//    tmpStack.pop();
//  }

  return true;
}


bool Automaton::
getBestMatch(const LinguisticAnalysisStructure::AnalysisGraph& graph,
             const LinguisticGraphVertex& begin, 
             const LinguisticGraphVertex& limit,
             AnalysisContent& analysis,
             RecognizerMatch& longestMatch, 
             ConstraintCheckList& checkList,
             const SearchGraphSense sense,
             const AutomatonControlParams& controlParams) const {
//   AULOGINIT;
//   LDEBUG << "testing automaton from " << begin << " to " << limit;
  
 
  AutomatonMatchSet results;
  ForwardSearch forward;
  BackwardSearch backward;
  bool success=getAllMatches(graph,begin,limit,analysis,
                             results,checkList,forward,
                             backward,sense,controlParams);
  if (success) {
    // results are sorted so that first is best
    longestMatch=results.begin()->first;
    checkList=results.begin()->second;
    if (sense == BACKWARDSEARCH) {
      // reverse found match
      std::reverse(longestMatch.begin(),longestMatch.end());
    }
  }

//   LDEBUG << "return success=" << success 
//          << ",match=" << longestMatch;
  
  return success;
}

bool Automaton::
getAllMatches(const LinguisticAnalysisStructure::AnalysisGraph& graph,
              const LinguisticGraphVertex& begin, 
              const LinguisticGraphVertex& limit,
              AnalysisContent& analysis,
              AutomatonMatchSet& results, 
              ConstraintCheckList& checkList,
              ForwardSearch& forward,
              BackwardSearch& backward,
              const SearchGraphSense sense,
              const AutomatonControlParams& controlParams) const {

  Tstate initialState(0);
  bool success(false);

  switch(sense) {
  case FORWARDSEARCH: {
    //SearchGraph* searchGraph=new ForwardSearch();
    forward.reinit();
    DFSStack forwardSearchStack(*this,graph,
                                &forward,
                                limit);
    success = testFromState(initialState, graph, 
                            begin, analysis,
                            results, 
                            checkList, 
                            forwardSearchStack,
                            controlParams);
    //delete searchGraph; 
    break;
  }
  case BACKWARDSEARCH: {
    //SearchGraph* searchGraph=new BackwardSearch();
    backward.reinit();
    DFSStack backwardSearchStack(*this,graph,
                                 &backward,
                                 limit);
    success = testFromState(initialState, graph, 
                            begin, analysis,
                            results, 
                            checkList, 
                            backwardSearchStack,
                            controlParams);
    //delete searchGraph; 
    break;
  }
  }
  
  return success;
}

bool Automaton::testFromState(const Tstate firstState,
                              const LinguisticAnalysisStructure::AnalysisGraph& graph,
                              const LinguisticGraphVertex& beginVertex,
                              AnalysisContent& analysis,
                              AutomatonMatchSet& results,
                              ConstraintCheckList& checkList,
                              DFSStack& S,
                              const AutomatonControlParams& controlParams) const {

//    AULOGINIT;
//    LDEBUG << "Automaton: testing from state " << firstState;

  // store in stack pairs of (automaton transition/graph vertex)
  // (store combinatory of all possible pairs, but if store only
  // matching pairs, problems with ConstraintCheckList

  RecognizerMatch currentMatch(&graph);

  // check initial state
  if (isFinalState(firstState)) { 
    results.insert(make_pair(currentMatch,checkList)); 
  }

  if (S.isEndVertex(beginVertex)) {
//     LDEBUG << beginVertex << "is end vertex. testing returns " << !results.empty();
    return (!results.empty());
  }

  // begin is the vertex that matched the trigger: 
  // push following vertices 
//   LDEBUG << "pushing";
  S.push(beginVertex,firstState,analysis);
  
  LinguisticGraphVertex vertex;
  const Transition* transition(0);
  uint64_t nbIter(0);
  bool backtrack(false);

  vector<uint64_t> backtrackDepth;
  backtrackDepth.push_back(0);

//                               LDEBUG << "before while (S size: " << S.size() << ")";
  while (! S.empty()) {
    
    nbIter++;
//     LDEBUG << "in iteration " << nbIter;
    if (S.size() > controlParams.getMaxDepthStack()) {
      AULOGINIT;
      LWARN << "MaxDepthStack exceeded in automaton search: ignore rest of search" 
           ;
      return (!results.empty());
    }
    if (nbIter > controlParams.getMaxTransitionsExplored()) {
      AULOGINIT;
      LWARN << "MaxTransitionsExplored exceeded in automaton search: ignore rest of search" 
           ;
      return (!results.empty());
    }
    
    boost::tie(vertex,transition)=S.top();
    if (backtrack) {
      // in backtrack : pop_back current match until the vertex 
      // for which we are testing a new matching transition

//       LDEBUG << "Automaton: backtrack: currentMatch="
//              << currentMatch << ", next matching for vertex " 
//              << vertex;

      if (backtrackDepth.empty()) {
        AULOGINIT;
        LWARN << "Automaton: should not be here! "
              << "backtrack stack empty: abort search";
        return (!results.empty());
      }

      uint64_t depth=backtrackDepth.back();
      if (currentMatch.size() < depth) {
        AULOGINIT;
        LWARN << "Automaton: should not be here! "
              << "backtrack depth larger than current match size: abort search";
        return (!results.empty());
      }
      for (uint64_t i(0); i<depth; i++) {
        currentMatch.popBackVertex();
      }
      backtrackDepth.pop_back();
      if (backtrackDepth.empty()) { // came back to start point
        backtrackDepth.push_back(0);
      }
      backtrack=false;
    }

    bool lastTransitionWithThisVertex=S.pop();
    
    // compare transition with vertex
    TransitionUnit* trans=transition->transitionUnit();

/*    LDEBUG << "Automaton: testing vertex " << vertex 
           << " with transition " << *trans;*/
//     if (lastTransitionWithThisVertex) {
//       LDEBUG << "=> is last transition for vertex " << vertex;
//     }

    //if (trans->match(graph,vertex,analysis,checkList)) {
    if (trans->checkConstraints(graph,vertex,analysis,checkList)) {
      
//       LDEBUG << "Automaton: -> match found";
      // update current match
      currentMatch.addBackVertex(vertex,trans->keep());
/*      LDEBUG << "Automaton: -> vertex (" << vertex 
             << ",keep=" << trans->keep() 
             << ") added in result, currentMatch="
             << currentMatch;*/
      // test if it is the head
      if (trans->head()) {
        // get token associated to next vertex
        currentMatch.setHead(vertex);
      }

      if (! lastTransitionWithThisVertex) {
        // not the last transition to test for this vertex
        // will have to come back to this branching point
        backtrackDepth.push_back(1);
      }
      else {
        backtrackDepth.back()++;
      }

      Tstate nextState=transition->nextState();
      if (isFinalState(nextState)) {
//         LDEBUG << "Automaton: saving result of size "<< currentMatch.size();
        if (currentMatch.size() > controlParams.getMaxResultSize()) {
          AULOGINIT;
          LWARN << "maxResultSize exceeded in automaton search: ignore result" 
               ;
        }
        else {
          results.insert(make_pair(currentMatch,checkList)); 
          if (results.size() > controlParams.getMaxNbResults()) {
            AULOGINIT;
            LWARN << "maxNbResults exceeded in automaton search: ignore rest of search" 
                 ;
          return (!results.empty());
          }
        }

/*        if (logger.isDebugEnabled()) {
          ostringstream oss;
          AutomatonMatchSet::const_iterator 
            it=results.begin(),
            it_end=results.end();
          for (;it!=it_end;it++) {
            oss << (*it).first << ";";
          }
           LDEBUG << "results are (" << oss.str() << ")";
      }*/
        if (lastTransitionWithThisVertex && ! hasTransitionsState(nextState)) {
          backtrack=true;
        }
      }

      // push next vertices
      if (!S.push(vertex,nextState,analysis)) {
        backtrack=true;
      }
    }
    else if (lastTransitionWithThisVertex) {
      backtrack=true;
    }
  }

  return (!results.empty());
}

/***********************************************************************/
// to build the automaton
/***********************************************************************/
Tstate Automaton::addState(bool is_final) {
  m_numberStates++;
  m_finalStates.push_back(is_final);
  m_transitions.push_back(vector<Transition>());
  m_searchStructures.push_back(static_cast<TransitionSearchStructure<Transition>*>(0));
  return m_numberStates-1; // first state is 0
}

// copy the content of the pointer (insert function of the 
bool Automaton::addTransition(Tstate initialState, 
                  Tstate finalState,    
                  TransitionUnit* transition) {

  vector<Transition>& transitions=m_transitions[initialState];

  // put negative transition at the end, so that positive transitions
  // are tested before : if not(a) and (b) are possible transitions, 
  // token "b" matches both, so transition (b) has to be checked before
  // this way, we can advance in the automaton
  // being sure that we do not need to go back eventually 
  if (transition->negative()) { 
    transitions.push_back(Transition(transition,finalState));
  }
  else { // put int front
    // putting epsilon transitions at first helps minimizing automaton
    vector<Transition>::iterator 
      it=transitions.begin(),
      it_end=transitions.end();
    for (; it!=it_end; it++) {
      if (!(*it).transitionUnit()->isEpsilonTransition()) {
        break;
      }
    }
    transitions.insert(it,Transition(transition,finalState));
//     transitions.insert(transitions.begin(),Transition(transition,finalState));
  }
  
//   std::cerr << Common::Misc::utf8stdstring2limastring("add transition ") 
//         << *transition << Common::Misc::utf8stdstring2limastring(" from ") << initialState 
//         << Common::Misc::utf8stdstring2limastring(" to ") << finalState << endl;
  return true;
}

void Automaton::removeState(const Tstate) {
  // on est oblige de tout renumeroter...
  std::cerr << "Warning: removeState not yet implemented..." << endl;
}

void Automaton::removeTransition(const Tstate initialState, 
                 const TransitionUnit& transition) {
  vector<Transition>::iterator i;
  for (i=m_transitions[initialState].begin(); i<m_transitions[initialState].end(); i++) {
    if (*(i->transitionUnit()) == transition) {
      m_transitions[initialState].erase(i);
      return;
    }
  }
}

void Automaton::makeFinal(const Tstate state) {
  m_finalStates[state]=true;
}

void Automaton::unMakeFinal(const Tstate state) {
  m_finalStates[state]=false;
}

void Automaton::setDeterministic(bool val) {
  m_deterministic=val;
}

/***********************************************************************/
// makes a deterministic automaton from a non-deterministic one
// using a simple subset construction
/***********************************************************************/
// some operations on SubSets (could be in a separate class)
std::string Automaton::subsetString(const Automaton::SubSet& subset) const {
  ostringstream oss;
  oss << "[";
  if (!subset.empty()) {
    SubSet::const_iterator 
      state=subset.begin(),
      state_end=subset.end();
    oss << *state;
    state++;
    for (; state!=state_end; state++) {
      oss << "," << *state;
    }
  }
  oss << "]";
  return oss.str();
}

// bool operator== (const vector<Tstate>& v1, const vector<Tstate>& v2) {
//   if (v1.size() != v2.size()) { return false; }
//   for (uint64_t i(0); i<v1.size(); i++) {
//     if (v1[i] != v2[i]) { return false; }
//   }
//   return true;
// }

// test if a set of states contains at least one final state
bool Automaton::isFinalSubset (const SubSet& v) const {
  SubSet::const_iterator 
    state=v.begin(),
    state_end=v.end();
  for (; state!=state_end; state++) {
    if (isFinalState(*state)) {
      return true; 
    }
  }
  return false;
}

Automaton Automaton::subsets() const {
  vector<TransitionUnit*> alphabet;
  vector< Automaton::SubSet > subsets;
  Automaton::SubSet currentSubset;
  Automaton detFA;

//   AULOGINIT;
  
  alphabet=collectTransitions();
//   if (logger.isDebugEnabled()) {
//     ostringstream oss;
//     oss << "alphabet=";
//     for (uint64_t i(0); i<alphabet.size(); i++) {
//       oss << *(alphabet[i]) << " ";
//     }
//     LDEBUG << oss.str();
//   }

  detFA.addState();
  //initial state is possibly final
  if ((! isDeterministic()) && 
      (existsEpsilonPathToFinal(0))) { 
    detFA.makeFinal(0); 
  }

  SubSet firstSubSet;
  firstSubSet.insert(0);
  subsets.push_back(firstSubSet);
  
  for (uint64_t i(0); i<detFA.numberOfStates(); i++) {
    for (uint64_t j(0); j<alphabet.size(); j++) {
      currentSubset.clear();
      reachableStates(subsets[i],*(alphabet[j]),currentSubset);
//       LDEBUG << "reachables from " << subsetString(subsets[i]) 
//              << " with " << *(alphabet[j]) << ":" 
//              << subsetString(currentSubset);

      if (currentSubset.size()) {
        // if a subset already corresponds to the current subset
        // do not add state, just add transition
        bool existingSubset(false);
        for (uint64_t k(0); k<subsets.size(); k++) {
          if (currentSubset == subsets[k]) {
            TransitionUnit *t =(*(alphabet[j])).clone();
            //TransitionUnit *t =alphabet[j];
            detFA.addTransition(i,k,t);
            existingSubset=true;
//             LDEBUG << "adding transition [" << i << "+" 
//                    << *(alphabet[j]) << "->" << k << "]";
            break;
          }
        }
        if (! existingSubset) { // add the state
          Tstate lastState=detFA.addState();
          if (isFinalSubset(currentSubset)) { detFA.makeFinal(lastState); }
          subsets.push_back(currentSubset);
          TransitionUnit *t =(*(alphabet[j])).clone();
          //TransitionUnit *t =alphabet[j];
          detFA.addTransition(i,lastState,t);
//           LDEBUG << "adding new state " << lastState 
//                  << " and transition [" << i << "+" 
//                  << *(alphabet[j]) << "->" << lastState << "]";
        }
      }
    }
  }

  // clear alphabet
  for (uint64_t i(0); i< alphabet.size(); i++) {
    delete alphabet[i];
    alphabet[i]=0;
  }
  alphabet.clear();
  
  detFA.setDeterministic(true);
  return detFA;
}

// get all the transitions that appear in the automaton
vector<TransitionUnit*> Automaton::collectTransitions() const {
  vector<TransitionUnit*> alphabet(0);
  bool alreadyCollected;
  
  for (uint64_t i(0); i<m_numberStates; i++) {
    for (uint64_t j(0); j<m_transitions[i].size(); j++) {
      if (m_transitions[i][j].transitionUnit()->isEpsilonTransition()) { continue; }
      // tests if it is already collected
      alreadyCollected=false;
      for (uint64_t k(0); k<alphabet.size(); k++) {
        if (*(m_transitions[i][j].transitionUnit()) == *(alphabet[k])) { 
          alreadyCollected=true; 
          break; 
        }
      }
      if (! alreadyCollected) {
        TransitionUnit *t=(*(m_transitions[i][j].transitionUnit())).clone();
        alphabet.push_back(t); 
      }
    }
  }
  return alphabet;
}

// get all the states that can be reached from a set of states with one
// particular transition
void Automaton::reachableStates(const SubSet& states, 
                                const TransitionUnit& t,
                                SubSet& reachable) const {
  SubSet::const_iterator 
    state=states.begin(),
    state_end=states.end();
  
  for (; state!=state_end; state++) {
    reachableStates(*state,t,reachable);
  }
}

void Automaton::reachableStates(const Tstate& state, 
                                const TransitionUnit& t,
                                SubSet& reachable) const {

  std::vector<Transition>::const_iterator
    transition=m_transitions[state].begin(),
    transition_end=m_transitions[state].end();
  
  //for (uint64_t l(0); l<m_transitions[*state].size(); l++) {
  for (; transition!=transition_end; transition++) {
    Tstate nextState=transition->nextState();
    if (*(transition->transitionUnit()) == t) {
      reachable.insert(nextState);
      reachableStates(nextState, EpsilonTransition(), reachable);
    }
    else if (transition->transitionUnit()->isEpsilonTransition()) {
      reachableStates(nextState, t, reachable);
    }
  }
}

/***********************************************************************/
// build the reverse automaton
/***********************************************************************/
Automaton Automaton::reverse() const {
  Automaton reverseAutomaton(numberOfStates()+1); // one more state (see below)
  Tstate newValueInitialState;
  
  // the reverse automaton will not be deterministic (because in the 
  // first deterministic automaton, several identical transitions can 
  // lead to one state), hence we do not try to be subtle and always
  // add epsilon transitions for the new initial state (even if there 
  // is only one final state in the original automaton)
  
  // the initial state becomes the last state of the reverse automaton
  newValueInitialState=numberOfStates();
  reverseAutomaton.makeFinal(newValueInitialState);

  vector<Tstate> finals(finalStates());
  // add one initial state and epsilon transitions
  for (uint64_t i(0); i<finals.size(); i++) {
    if (finals[i]==0) { // the initial state was also final
      reverseAutomaton.addTransition(0,newValueInitialState,new EpsilonTransition());
    } 
    else {
      reverseAutomaton.addTransition(0,finals[i],new EpsilonTransition());
    }
  }
  
  for (uint64_t i(0); i<m_transitions.size(); i++) {
    for (uint64_t j(0); j<m_transitions[i].size(); j++) {
      Tstate initial = m_transitions[i][j].nextState();
      Tstate final = i;
      if (initial == 0) { initial = newValueInitialState; }
      if (final   == 0) { final   = newValueInitialState; }
      reverseAutomaton.addTransition(initial, final, 
                     //m_transitions[i][j].transitionUnit());
                           m_transitions[i][j].transitionUnit()->clone());
    }
  }
  
  if (isDeterministic()) { // make the new one deterministic also
    reverseAutomaton = reverseAutomaton.subsets();
  }
  
  return reverseAutomaton;
}

/***********************************************************************/
// simple Brzozowski's algorithm for minimization : just reverse 
// and determinize twice
/***********************************************************************/
Automaton Automaton::brzozowskiMinimize() const {
  Automaton a;
  if (! isDeterministic()) {
    a=subsets();
  }
  else {
    a=*this;
  }
  a=a.reverse(); // determinization is done in function reverse if 
  a=a.reverse(); // the automaton was already deterministic
  return a;
}

/***********************************************************************/
// output
/***********************************************************************/


ostream& operator << (ostream& os, const Automaton& a) {
  
  //   os << "deterministic=" << a.isDeterministic() << endl;
  
  for (uint64_t i(0); i<a.numberOfStates(); i++) {
    if (a.isFinalState(i)) { os << i << " [final]" << endl; }
    for (uint64_t j(0); j<a.m_transitions[i].size(); j++) {
      os << i << " -> " << a.m_transitions[i][j].nextState()
      << " [label=\""
      << *(a.m_transitions[i][j].transitionUnit())
      << "\"]" << endl;
    }
  }
  
  //os << "}" << endl;
  
  return os;
}

QDebug& operator << (QDebug& os, const Automaton& a) {
  
  //   os << "deterministic=" << a.isDeterministic() << endl;
  
  for (uint64_t i(0); i<a.numberOfStates(); i++) {
    if (a.isFinalState(i)) { os << i << " [final]" << endl; }
    for (uint64_t j(0); j<a.m_transitions[i].size(); j++) {
      os << i << " -> " << a.m_transitions[i][j].nextState()
      << " [label=\""
      << *(a.m_transitions[i][j].transitionUnit())
      << "\"]" << endl;
    }
  }
  
  //os << "}" << endl;
  
  return os;
}

} // namespace end
} // namespace end
} // namespace end
