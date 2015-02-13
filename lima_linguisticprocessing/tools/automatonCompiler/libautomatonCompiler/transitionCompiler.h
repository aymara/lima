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
* File        : transitionCompiler.h
* Project     : Named Entities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Tue Apr  8 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: transitionCompiler.h 9081 2008-02-25 18:34:51Z de-chalendarg $
*
* Description : a namespace for functions to build transitions from a
* string
*
************************************************************************/

#ifndef TRANSITIONCOMPILER_H
#define TRANSITIONCOMPILER_H

#include "common/MediaticData/EntityType.h"
#include "linguisticProcessing/core/Automaton/transitionUnit.h"
#include "automatonString.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {
namespace AutomatonCompiler {

Lima::LinguisticProcessing::Automaton::TransitionUnit* 
  createTransition(const AutomatonString& automatonString,
                   MediaId language, const std::string& id,
                   const std::vector<LimaString>& activeEntityGroups);

Lima::LinguisticProcessing::Automaton::TransitionUnit* 
  createTransition(const LimaString,
                   MediaId language, const std::string& id,
                   const std::vector<LimaString>& activeEntityGroups,
                   const bool keep=true,
                   const bool neg=false,
                   const std::vector<Constraint>& constraints=
                   std::vector<Constraint>(0));

Common::MediaticData::EntityType
  resolveEntityName(const LimaString str,
                    const std::vector<LimaString>& activeEntityGroups);
 
} // end namespace
} // end namespace
} // end namespace
} // end namespace

#endif
