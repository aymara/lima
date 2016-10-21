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
 * @file       automaton.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Oct 04 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      A class for the description of automata
 * 
 * 
 ***********************************************************************/

#ifndef AUTOMATON_H
#define AUTOMATON_H

#include "AutomatonExport.h"
#include "transitionUnit.h"
#include "searchGraph.h"
#include "gazeteerTransition.h"
#include "transition.h"
#include "transitionSearchStructure.h"
#include "recognizerMatch.h"
#include "constraint.h"
#include "automatonReaderWriter.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

#include <fstream>
#include <vector>
#include <set>
#include <string>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/**
 * rief a class for control parameters for the search using the automata
 */
class LIMA_AUTOMATON_EXPORT AutomatonControlParams {
 public:
  AutomatonControlParams();
  ~AutomatonControlParams();

  uint64_t getMaxDepthStack() const
    { return m_maxDepthStack; }
  uint64_t getMaxTransitionsExplored() const
    { return m_maxTransitionsExplored; }
  uint64_t getMaxNbResults() const
    { return m_maxNbResults; }
  uint64_t getMaxResultSize() const
    { return m_maxResultSize; }

  void setMaxDepthStack(const uint64_t val) 
    { m_maxDepthStack=val; }
  void setMaxTransitionsExplored(const uint64_t val) 
    { m_maxTransitionsExplored=val; }
  void setMaxNbResults(const uint64_t val)
    { m_maxNbResults=val; }
  void setMaxResultSize(const uint64_t val) 
    { m_maxResultSize=val; }

 private:
  uint64_t m_maxDepthStack;
  uint64_t m_maxTransitionsExplored;
  uint64_t m_maxNbResults;
  uint64_t m_maxResultSize;
};

/**
 * rief A class for the description of automata
 *
 * this class is used for the definition of the automata implementing
 * local grammars:
 * - The states are simply identified by a number: there is no explicit
 * representation of the states, only a number of states.
 * The number of states is uint64_t.
 * - The transition function is represented by a list of lists, associating
 * to each state the lists of couples (transition, next state)
 */

class LIMA_AUTOMATON_EXPORT Automaton
{
friend class AutomatonReader;
friend class AutomatonWriter;

public:
  /**
   * void constructor initializes an empty automaton
   * (no state, no transition)
   */
  Automaton( const std::string& automId = "" );

  /**
   * copy constructor
   */
  Automaton(const Automaton& a); 

  /** 
   * constructor: build an automaton with a predefined number of states,
   * no indications of final states, no transitions
   * 
   * @param nbStates : the number of states to reserve
   */
  Automaton(const Tstate nbStates); 

  /** 
   * destructor
   */
  ~Automaton();
  
  /**
   * assignment operator
   */  
  Automaton& operator= (const Automaton& a);
  

  //**********************************************************************
  /** 
   * get the number of states of the automaton
   * @return The number of states of the automaton
   */
  Tstate numberOfStates() const;
  
  /** 
   * get the number of transitions in the automaton
   * @return The number of transitions in the automaton
   */
  uint64_t numberOfTransitions() const;

  /** 
   * find the number of transitions leaving from the state indicated
   * in argument
   * @param state the state to watch
   * @return the number of transitions from the state
   */
  uint64_t numberOfTransitions(const Tstate state) const;

  /** 
   * find the nth transition leaving from a particular state
   * @param state : the state 
   * @param n : the index of the transition we search for
   * @return the corresponding transition
   */
  Transition const& nthTransition(const Tstate state,
                  const uint64_t n) const;

  /** 
   * test if a given state is a final state of the automaton
   * @param state : the state to test
   * @return true if the state is a final state, false otherwise
   */
  bool isFinalState(const Tstate state) const;
  /** 
   * get the list of the final states of the automaton
   * @return a vector containing the final states of the automaton
   */
  std::vector<Tstate> finalStates() const;
  /** 
   * get the list of transitions leaving from a given state
   * @param state : the state to consider
   * @return a vector containing the transitions
   */
  std::vector<Transition> const& getTransitionsState(const Tstate state) const;

  /** 
   * test if a state of the automaton has out transitions
   */
  bool hasTransitionsState(const Tstate state) const;

  /** 
   * test if the automaton is deterministic or not
   * @return true if the automaton is deterministic, false otherwise
   */
  bool isDeterministic() const;

  /** 
   * reinitializes the automaton (no states, no transitions)
   * 
   */
  void reinit();

  /** 
   * set a property hashcode to each transition which represent the constraint(s?) of type action attached to it
   * 
   */
   void setActionHash(const std::vector<std::pair<LimaString,Constraint> >& actionsWithOneArgument);
  /** 
   * @{ types defined to store the result (or results of the application
   * of the automaton on a graph
   */
  typedef std::pair<RecognizerMatch,ConstraintCheckList> AutomatonMatch;
  class CompareAutomatonMatch {
  public:
    bool operator()(const AutomatonMatch& r1, 
                    const AutomatonMatch& r2) const;
  };
  typedef std::set<AutomatonMatch,CompareAutomatonMatch> 
    AutomatonMatchSet;
  /** @} */

  /** 
   * test if a text corresponds to the automaton : the text is represented
   * as a LinguisticAnalysisStructure::AnalysisGraph
   * @param text the LinguisticAnalysisStructure::AnalysisGraph corresponding to the beginning
   * of the text to be analyzed
   * @param longestMatch the result
   * @param checkList the current state for constraint checking
   * @param sense the sense of search in the graph (FORWARD or BACKWARD)
   * @param controlParams parameters to control the search 
   * (for robust treatment of complex graphs)
   *
   * @return true is a match was found, false otherwise
   */
  bool getBestMatch(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                    const LinguisticGraphVertex& begin, 
                    const LinguisticGraphVertex& limit,
                    AnalysisContent& analysis,
                    RecognizerMatch& longestMatch, 
                    ConstraintCheckList& checkList,
                    const SearchGraphSense sense,
                    const AutomatonControlParams& controlParams) const;
  
  /** 
   * get all matches found between automaton and graph between two
   * points
   * (WARNING: in case of backward search,
   * matches returned are reversed : to to reverse them if
   * natural sense of the graph is needed)
   * 
   * @param graph the graph
   * @param begin the first vertex
   * @param limit the last possible vertex
   * @param analysis the current content of the analysis
   * @param allMatches the matches found
   * @param checkList the current state for constraint checking
   * @param sense the sense of search in the graph (FORWARD or BACKWARD)
   * @param controlParams parameters to control the search 
   * (for robust treatment of complex graphs)
   * 
   * @return 
   */
  bool getAllMatches(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                     const LinguisticGraphVertex& begin, 
                     const LinguisticGraphVertex& limit,
                     AnalysisContent& analysis,
                     AutomatonMatchSet& allMatches, 
                     ConstraintCheckList& checkList,
                     ForwardSearch& forward,
                     BackwardSearch& backward,
                     const SearchGraphSense sense,
                     const AutomatonControlParams& controlParams) const;
  

  //----------------------------------------------------------------------
  // to build the automaton
  //----------------------------------------------------------------------

  /** 
   * add a state to the automaton
   * @param is_final indicates if the state must be initialized as a final
   * state of the automaton or not
   * @return the identification of the state added
   */
  Tstate addState(bool is_final=false);
  /** 
   * add a transition between two states
   * @param initialState the initial state
   * @param finalState the final state
   * @param transition the transition unit leading from the initial state to 
   * the final state
   * @return true if the transition was added successfully, false otherwise
   */
  bool addTransition(Tstate initialState, Tstate finalState, 
             TransitionUnit* transition);

  void removeState(const Tstate state);
  void removeTransition(const Tstate initialState, 
            const TransitionUnit& transition);
  /** 
   * make a state final
   * @param state the state to make final
   */
  void makeFinal(const Tstate state);
  /** 
   * remove a state from the final states
   * @param state the state to remove
   */
  void unMakeFinal(const Tstate state);
  /** 
   * set the flag indicating if the automaton is deterministic or not
   * (does not make the automaton deterministic, just set the flag)
   * @param det the value to set the flag with (true to say the automaton
   * is deterministic, false to say it is not)
   */
  
  void setDeterministic(const bool det);
  /** 
   * make deterministic automaton with the subsets method
   * (does not change the current instance of the autmomaton)
   * @return the deterministic automaton obtained
   */  
  Automaton subsets() const;

  /** 
   * build the automaton that will accept the reverse strings of the language
   * (does not change the current instance of the autmomaton)
   * @return the reverse automaton obtained
   */ 
  Automaton reverse() const;


  /** 
   * Brzozowski's algorithm for minimization : double reverse 
   * and determinization
   * (does not change the current instance of the autmomaton)
   * @return the minimized automaton
   */  
  Automaton brzozowskiMinimize() const;

  // for the output
  /** 
   * output operator << overloading
   */  
  friend LIMA_AUTOMATON_EXPORT std::ostream& operator << (std::ostream& os, const Automaton& a);
  friend LIMA_AUTOMATON_EXPORT QDebug& operator << (QDebug& os, const Automaton& a);
  
  void initializeSearchStructures(MediaId language);
  bool getMatchingTransitions(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                              const LinguisticGraphVertex& vertex, 
                              AnalysisContent& analysis,
                              SearchGraph* searchGraph,
                              const Tstate& state,
                              std::vector<std::pair<std::deque<LinguisticGraphVertex>,const Transition*> >& matchingTransitions,
                              const LinguisticGraphVertex& limit) const;
  
 protected:
  Tstate m_numberStates;    /**< number of states in the automaton */
  std::vector<bool> m_finalStates;  /**< which states are final states */
  std::vector< std::vector<Transition> > m_transitions; /**< the transitions */
  std::vector< TransitionSearchStructure<Transition>* > m_searchStructures; 
  bool m_deterministic;        /**< a boolean flag indicating if the 
                                  automaton is deterministic or not */
  std::string m_id;         /** profix of identifier of transition */
  
  //private methods
  //**********************************************************************
  // helper functions for constructors and destructors
  void init();
  void copy(const Automaton& a);
  void freeMem();

  class DFSStack;
  
  bool testFromState(const Tstate firstState,
                     const LinguisticAnalysisStructure::AnalysisGraph& graph,
                     const LinguisticGraphVertex& begin,
                     const LinguisticGraphVertex& limit,
                     AnalysisContent& analysis,
                     AutomatonMatchSet& results,
                     ConstraintCheckList& checkList,
                     DFSStack& stack,
                     const AutomatonControlParams& controlParams) const;

  //************************************************************
  // helper functions for automaton construction and 
  // determinization
  typedef std::set<Tstate> SubSet;
  
  bool existsEpsilonPathToFinal(const Tstate state) const;
  std::vector<TransitionUnit*> collectTransitions() const;
  bool isFinalSubset (const SubSet& v) const;
  void reachableStates(const SubSet& states, 
                       const TransitionUnit& t, 
                       SubSet& reachable) const;
  void reachableStates(const Tstate& state, 
                       const TransitionUnit& t, 
                       SubSet& reachable) const;
  std::string subsetString(const SubSet& subset) const; // for debug

};
/***********************************************************************/
// inline access functions
/***********************************************************************/
inline Tstate Automaton::numberOfStates() const {
  return m_numberStates;
}

inline uint64_t Automaton::numberOfTransitions(const Tstate state) const {
  return m_transitions[state].size();
}

inline Transition const&  Automaton::nthTransition(const Tstate state, 
                           const uint64_t n) const {
  return m_transitions[state][n];
}

inline bool Automaton::isFinalState(const Tstate state) const {
  if (state >= m_numberStates) { return false; }
  return m_finalStates[state];
}


inline bool Automaton::isDeterministic() const {
  return m_deterministic;
}

inline std::vector<Transition> const&
Automaton::getTransitionsState(const Tstate state) const {
  return m_transitions[state];
}

} // end namespace
} // end namespace
} // end namespace

#endif
