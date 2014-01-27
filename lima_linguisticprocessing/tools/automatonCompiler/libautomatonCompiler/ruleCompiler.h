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
/*************************************************************************
*
* File        : ruleCompiler.h
* Project     : Named Entities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Apr  7 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: ruleCompiler.h 9081 2008-02-25 18:34:51Z de-chalendarg $
*
* Description : a namespace for the compilation of rules
*
*************************************************************************/

#ifndef RULECOMPILER_H
#define RULECOMPILER_H

#include "ruleFormat.h"
#include "gazeteer.h"
#include "subAutomaton.h"
#include "linguisticProcessing/core/Automaton/rule.h"
#include "linguisticProcessing/core/Automaton/constraint.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {
namespace RuleCompiler {

  LimaString initRule(Rule& r,
                       const LimaString&, 
                       MediaId language,
                       const std::vector<Gazeteer>& gazeteers,
                       const std::vector<SubAutomaton>& subAutomatons,
                       const std::vector<LimaString>& activeEntityGroups,
                       const std::string& filename="",
                       uint64_t lineNumber=0);

  LimaString 
    treatConstraints(Rule& r,const LimaString&);
  
  void 
    setType(Rule& r,
            const LimaString& s,
            const std::vector<LimaString>& activeEntityGroups);

} // end namespace
} // end namespace
} // end namespace
} // end namespace


#endif
