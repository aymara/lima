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
* File        : automatonCompiler.h
* Project     : NamedEntities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Apr  7 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: automatonCompiler.h 9081 2008-02-25 18:34:51Z de-chalendarg $
*
* Description : a class for the parsing of regular expressions and building automata
*
*************************************************************************/

#ifndef AUTOMATONCOMPILER_H
#define AUTOMATONCOMPILER_H

#include "AutomatonCompilerExport.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/core/Automaton/automaton.h"
#include "automatonString.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {
namespace AutomatonCompiler {

  //----------------------------------------------------------------------
  // use directly automaton string
  LIMA_AUTOMATONCOMPILER_EXPORT Automaton buildAutomaton(const AutomatonString& automatonString,
                           MediaId language,
                           const std::vector<Gazeteer>& gazeteers,
                           SearchGraphSense sense,
                           const std::vector<LimaString>& activeEntityGroups,
                         const std::vector<std::pair<LimaString,Constraint> >& actionsWithOneArgument);

  LIMA_AUTOMATONCOMPILER_EXPORT Tstate buildAutomaton(Automaton& a,
                        const AutomatonString& automatonString, 
                        const Tstate& initialState, const std::string& currentId,
                        MediaId language,
                        const std::vector<Gazeteer>& gazeteers,
                        const std::vector<LimaString>& activeEntityGroups);
  
  LIMA_AUTOMATONCOMPILER_EXPORT Tstate buildAutomatonNotOptional(Automaton& a,
                                   const AutomatonString& automatonString, 
                                   const Tstate& initialState, const std::string& currentId,
                                   MediaId language,
                                   const std::vector<Gazeteer>& gazeteers,
                                   const std::vector<LimaString>& activeEntityGroups);
} // end namespace
} // end namespace
} // end namespace
} // end namespace

#endif
