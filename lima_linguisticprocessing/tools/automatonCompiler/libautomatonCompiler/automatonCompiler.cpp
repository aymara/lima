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
/***********************************************************************
*
* File        : automatonCompiler.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Apr  7 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: automatonCompiler.cpp 9081 2008-02-25 18:34:51Z de-chalendarg $
*
************************************************************************/

#include "automatonCompiler.h"
#include "transitionCompiler.h"
#include "compilerExceptions.h"
#include "linguisticProcessing/core/Automaton/automatonCommon.h"
#include "linguisticProcessing/core/Automaton/epsilonTransition.h"

#include <QtCore/QCoreApplication>

using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {
namespace AutomatonCompiler {


/***********************************************************************/
// build an automaton from the automatonString
/***********************************************************************/
Automaton buildAutomaton(const AutomatonString& automatonString,
                         MediaId language, 
                         SearchGraphSense sense,
                         const std::vector<LimaString>& activeEntityGroups) {
  AUCLOGINIT;
  Automaton a;
  
  // etat initial
  Tstate initialState=a.addState();
  if (automatonString.empty()) { // universal automaton -> accepts everything
    a.makeFinal(initialState);
    a.setDeterministic(true);
  }
  else {
//     LDEBUG << "automatonString is: " << automatonString;
    Tstate finalState=buildAutomaton(a,automatonString,
                                     initialState,language,
                                     activeEntityGroups);
    // LDEBUG << "final state is " << finalState;
    a.makeFinal(finalState);

    // LDEBUG << "automaton=" << a;

    a.setDeterministic(false);

    if (sense==BACKWARDSEARCH) { // reverse automaton
      a=a.reverse();
      // LDEBUG << "reverse automaton=" << a;
    }
    
    // make it deterministic
    a=a.subsets();

    // LDEBUG << "deterministic automaton=" << a;

    // make it minimal
    a=a.brzozowskiMinimize();

    // LDEBUG << "minimal automaton=" << a;

  }

  return a;
}

/***********************************************************************/
// build an automaton from the automatonString
// (structured automaton with transitions as strings)
/***********************************************************************/
Tstate buildAutomaton(Automaton& a,
                      const AutomatonString& automatonString, 
                      const Tstate& initialState,
                      MediaId language,
                      const std::vector<LimaString>& activeEntityGroups) {
  
  AUCLOGINIT;
  LDEBUG << "build automaton from " << automatonString.getString();

  if (automatonString.isOptional()) {

    int min=automatonString.getMinOccurrences();
    int max=automatonString.getMaxOccurrences();

    LDEBUG << "automaton is optional {"<<min<<"-"<<max<< "}";

    Tstate finalState=initialState;

    // if min occurrences not 0 : non-optional part
    while (min > 0) {
      // must be there x times -> insert it as non-optional
      finalState = buildAutomatonNotOptional(a,automatonString,finalState,language,activeEntityGroups);
      min--;
      if (max != AutomatonString::INFINITE_OCC) { 
        max--;
      }
    }

    Tstate optInitialState=finalState; // after possible non-optional part

    // optional part
    if (max == AutomatonString::INFINITE_OCC) {
      // insert it once as non-optional, 
      // add the epsilon-transition from first to last (for minOcurrences=0)
      // and insert again the automaton from last to first 
      // (to avoid epsilon-cycles)
      finalState = buildAutomatonNotOptional(a,automatonString,finalState,language,activeEntityGroups);
      a.addTransition(optInitialState,finalState,new EpsilonTransition());
      
      Tstate tmpFinalState(finalState);
      Tstate tmpReturnState = buildAutomatonNotOptional(a,automatonString,
                                                        tmpFinalState,language,activeEntityGroups);
      //a.addTransition(tmpReturnState,optInitialState,new EpsilonTransition());
      a.addTransition(tmpReturnState,finalState,new EpsilonTransition());

      // add another epsilon transition to new state
      // (in case some epsilon state will point on final 
      // state (optional inside optional))
      tmpFinalState=finalState;
      finalState=a.addState();
      a.addTransition(tmpFinalState,finalState,new EpsilonTransition());
    }
    else {
      // insert it as non-optional as many times as necessary 
      // and add the epsilon-transition
      while (max > 0) {
        finalState = buildAutomatonNotOptional(a,automatonString,finalState,language,activeEntityGroups);
        a.addTransition(optInitialState,finalState,new EpsilonTransition());
        max--;
      }
    }
    return finalState;
  }
  else {
    return buildAutomatonNotOptional(a,automatonString,initialState,language,activeEntityGroups);
  }
}

Tstate buildAutomatonNotOptional(Automaton& a,
                                 const AutomatonString& automatonString, 
                                 const Tstate& initialState,
                                 MediaId language,
                                 const std::vector<LimaString>& activeEntityGroups) 
{
  AUCLOGINIT;
  LDEBUG << "build non-optional automaton from " << automatonString.getString();

  //-------------------------- alternative ------------------------------
  if (automatonString.isAlternative()) {
    LDEBUG << "is alternative ";
    Tstate finalState=a.addState(); // the final state to which all 
                                    // options will converge
   
    std::vector<AutomatonString>::const_iterator
      it=automatonString.getParts().begin(),
      it_end=automatonString.getParts().end();
    for (; it!=it_end; it++) {
      Tstate altFinalState=buildAutomaton(a,*it,initialState,language,activeEntityGroups);
      a.addTransition(altFinalState,finalState,new EpsilonTransition());
    }
    return finalState;
  }
  //----------------------------- sequence ------------------------------
  else if (automatonString.isSequence()) {
    LDEBUG << "is sequence ";
    std::vector<AutomatonString>::const_iterator
      it=automatonString.getParts().begin(),
      it_end=automatonString.getParts().end();

    Tstate seqInitialState=initialState;
    Tstate seqfinalState=initialState;
    for (; it!=it_end; it++) {
      seqfinalState=buildAutomaton(a,*it,seqInitialState,language,activeEntityGroups);
      seqInitialState=seqfinalState;
    }
    return seqfinalState;
  }
  //-------------------------- simple unit ------------------------------
  else if (automatonString.isUnit()) {
    LDEBUG << "is unit ";
    TransitionUnit *t = createTransition(automatonString,language,activeEntityGroups);
    if (t != 0) {
      Tstate finalState = a.addState();
      a.addTransition(initialState, finalState, t);
      return finalState;
    }
    else {
      throw AutomatonErrorException("attempt to insert empty transition\n");
    }
  }
  return initialState;
}

} // end namespace
} // end namespace
} // end namespace
} // end namespace
