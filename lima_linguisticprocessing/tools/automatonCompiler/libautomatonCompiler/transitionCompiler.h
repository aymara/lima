// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

/**
 * Lima::LinguisticProcessing::Automaton::TransitionUnit*
  createGazeteerTransition(const AutomatonString& automatonString,
                 MediaId language, const std::string& id,
                 const std::vector<LimaString>& activeEntityGroups,
                 const std::vector<LimaString>& gazeteerAsVectorOfString,
                 const bool keepTrigger);
*/
Lima::LinguisticProcessing::Automaton::TransitionUnit*
  createGazeteerTransition(const LimaString& gazeteerName,
                 MediaId language, const std::string& id,
                 const std::vector<LimaString>& activeEntityGroups,
                 const std::vector<Gazeteer>& gazeteers,
                 const bool keep=true,
                 const bool head=false);

Lima::LinguisticProcessing::Automaton::TransitionUnit* 
  createTransition(const LimaString,
                   MediaId language, const std::string& id,
                   const std::vector<LimaString>& activeEntityGroups,
                   const bool keep=true,
                   const bool neg=false,
                   const std::vector<Constraint>& constraints=
                   std::vector<Constraint>(0),
                   const std::vector<LimaString>& gazeteerAsVectorOfString = std::vector<LimaString>(0) );

Common::MediaticData::EntityType
  resolveEntityName(const LimaString str,
                    const std::vector<LimaString>& activeEntityGroups);
Common::MediaticData::EntityGroupId
  resolveGroupName(const LimaString s,
                   const std::vector<LimaString>& activeEntityGroups);
 
} // end namespace
} // end namespace
} // end namespace
} // end namespace

#endif
