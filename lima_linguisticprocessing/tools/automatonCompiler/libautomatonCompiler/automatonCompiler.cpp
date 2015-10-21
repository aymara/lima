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
#include "gazeteer.h"
#include "transitionCompiler.h"
#include "compilerExceptions.h"
#include "linguisticProcessing/core/Automaton/automatonCommon.h"
#include "linguisticProcessing/core/Automaton/epsilonTransition.h"

#include <QtCore/QCoreApplication>

#include "common/time/timeUtilsController.h"

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
                         const std::vector<Gazeteer>& gazeteers,
                         SearchGraphSense sense,
                         const std::vector<LimaString>& activeEntityGroups) {
  AUCLOGINIT;
  // Lima::TimeUtilsController* ctrlA  = new Lima::TimeUtilsController("buildAutomaton", true);
  std::string currentId(automatonString.getId());
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
                                     initialState,currentId,language,gazeteers,
                                     activeEntityGroups);
    // Lima::TimeUtilsController* ctrlAF  = new Lima::TimeUtilsController("make final", true);
    // LDEBUG << "final state is " << finalState;
    a.makeFinal(finalState);
    // delete ctrlAF;

    // LDEBUG << "automaton=" << a;

    // Lima::TimeUtilsController* ctrlAD  = new Lima::TimeUtilsController("setDeterministic", true);
    a.setDeterministic(false);
    /// delete ctrlAD;

    if (sense==BACKWARDSEARCH) { // reverse automaton
      a=a.reverse();
      // LDEBUG << "reverse automaton=" << a;
    }
    
    // make it deterministic
    // Lima::TimeUtilsController* ctrlASubset  = new Lima::TimeUtilsController("subsets", true);
    a=a.subsets();
    // delete ctrlASubset;

    // LDEBUG << "deterministic automaton=" << a;

    // make it minimal
    // Lima::TimeUtilsController* ctrlAMin  = new Lima::TimeUtilsController("brzozowskiMinimize", true);
    a=a.brzozowskiMinimize();
    // delete ctrlAMin;

    // LDEBUG << "minimal automaton=" << a;

  }

  // delete ctrlA;
  return a;
}

/***********************************************************************/
// build an automaton from the automatonString
// (structured automaton with transitions as strings)
/***********************************************************************/
Tstate buildAutomaton(Automaton& a,
                      const AutomatonString& automatonString, 
                      const Tstate& initialState, const std::string& currentId,
                      MediaId language,
                      const std::vector<Gazeteer>& gazeteers,
                      const std::vector<LimaString>& activeEntityGroups) {
  
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "build automaton from " << automatonString.getString() << " with id " << currentId;
#endif

  if (automatonString.isOptional()) {

    int min=automatonString.getMinOccurrences();
    int max=automatonString.getMaxOccurrences();

#ifdef DEBUG_LP
    LDEBUG << "automaton is optional {"<<min<<"-"<<max<< "}";
#endif

    Tstate finalState=initialState;

    // if min occurrences not 0 : non-optional part
    // TODO: check if we have to handle modifiers of numbering like first, next and last in currentId
    while (min > 0) {
      // must be there x times -> insert it as non-optional
      finalState = buildAutomatonNotOptional(a,automatonString,finalState,currentId,language,gazeteers,activeEntityGroups);
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
      finalState = buildAutomatonNotOptional(a,automatonString,finalState,currentId,language,gazeteers,activeEntityGroups);
      a.addTransition(optInitialState,finalState,new EpsilonTransition());
      
      Tstate tmpFinalState(finalState);
      Tstate tmpReturnState = buildAutomatonNotOptional(a,automatonString,
                                                        tmpFinalState,currentId,language,gazeteers,activeEntityGroups);
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
        finalState = buildAutomatonNotOptional(a,automatonString,finalState,currentId,language,gazeteers,activeEntityGroups);
        a.addTransition(optInitialState,finalState,new EpsilonTransition());
        max--;
      }
    }
    return finalState;
  }
  else {
    return buildAutomatonNotOptional(a,automatonString,initialState,currentId,language,gazeteers,activeEntityGroups);
  }
}

Tstate buildAutomatonNotOptional(Automaton& a,
                                 const AutomatonString& automatonString, 
                                 const Tstate& initialState, const std::string& initialId,
                                 MediaId language,
                                 const std::vector<Gazeteer>& gazeteers,
                                 const std::vector<LimaString>& activeEntityGroups) 
{
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "build non-optional automaton from " << automatonString.getString() << " with id " << initialId;
  #endif

  //-------------------------- alternative ------------------------------
  if (automatonString.isAlternative()) {
#ifdef DEBUG_LP
    LDEBUG << "is alternative ";
#endif
    Tstate finalState=a.addState(); // the final state to which all 
                                    // options will converge
    std::string currentId(initialId);
    //currentId.append(".1");
    std::vector<AutomatonString>::const_iterator
      it=automatonString.getParts().begin(),
      it_end=automatonString.getParts().end();
    for (; it!=it_end; it++) {
      Tstate altFinalState=buildAutomaton(a,*it,initialState,currentId,language,gazeteers,activeEntityGroups);
      a.addTransition(altFinalState,finalState,new EpsilonTransition()); // id???
    }
    return finalState;
  }
  //----------------------------- sequence ------------------------------
  else if (automatonString.isSequence()) {
#ifdef DEBUG_LP
    LDEBUG << "is sequence ";
#endif
    std::vector<AutomatonString>::const_iterator
      it=automatonString.getParts().begin(),
      it_end=automatonString.getParts().end();

    Tstate seqInitialState=initialState;
    Tstate seqfinalState=initialState;
    int subCount = 1;
    for (; it!=it_end; it++, subCount++) {
      std::string currentId(initialId);
      currentId.append(".").append(std::to_string(static_cast<long long>(subCount)));
      seqfinalState=buildAutomaton(a,*it,seqInitialState,currentId,language,gazeteers,activeEntityGroups);
      seqInitialState=seqfinalState;
    }
    return seqfinalState;
  }
  //-------------------------- simple unit ------------------------------
  else if (automatonString.isUnit()) {
#ifdef DEBUG_LP
    LDEBUG << "is unit ";
#endif
    TransitionUnit* t;
#ifdef DEBUG_LP
    LDEBUG << "buildAutomatonNotOptional: createSimpleTransition from " << automatonString.getString();
#endif
    t = createTransition(automatonString,language,initialId,activeEntityGroups);
    if (t != 0) {
      Tstate finalState = a.addState();
      a.addTransition(initialState, finalState, t);
      return finalState;
    }
    else {
      throw AutomatonErrorException("attempt to insert empty transition\n");
    }
  }
  // We do not yet know how to use gazetteer with any element defined with a category or with space chrecter
  else if (automatonString.isSimpleGazeteer()) {
#ifdef DEBUG_LP
    LDEBUG << "is simpleGazeteer ";
#endif
     const LimaString& unitString = automatonString.getUnitString();
     const LimaString& gazeteerName = unitString.mid(1,unitString.size()-1);
// OME    LimaString gazeteerName = automatonString.getUnitString().mid(1,automatonString.getString().size()-1);
// OME     int i;
// OME     for (i=0; i<gazeteers.size(); i++) {
// OME       if (gazeteers[i].alias() == gazeteerName) {
// OME         break;
// OME       }
// OME     }
// OME     const Gazeteer& gazeteer = gazeteers[i];
// OME    const std::vector<LimaString> gazeteerAsVectorOfString = gazeteer;
// OME #ifdef DEBUG_LP
// OME     LDEBUG << "buildAutomatonNotOptional: new GazeteerTransition from " << gazeteer.alias();
// OME #endif
    // t = createGazeteerTransition(automatonString,language,initialId,activeEntityGroups,gazeteerAsVectorOfString,true);
    // DONE?: replace new GazeteerTransition by createTransition....
    // t = new GazeteerTransition(gazeteerAsVectorOfString,gazeteer.alias(),true);
    // TransitionUnit* trigger = new GazeteerTransition(gazeteerAsVectorOfString,gazeteerName,keepTrigger); */
    // TODO, vérifier que 
    //     - que trigger = true, est le bon paramétrage
    //     - que head = false est le bon paramétrage
    //     - gérer aussi les "constraints"
    TransitionUnit* t = createGazeteerTransition(gazeteerName,
                 language, initialId, activeEntityGroups,
                 gazeteers,true,false);

    if (t != 0) {
      const std::vector<Constraint>& constraints = automatonString.getConstraints();
      for (std::size_t i(0); i<constraints.size(); i++) {
        t->addConstraint(constraints[i]);
      }
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
