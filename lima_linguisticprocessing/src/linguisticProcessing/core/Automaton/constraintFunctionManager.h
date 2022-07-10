// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       constraintFunctionManager.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Mar 16 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     Automaton
 * 
 * @brief      manager for constraint functions (contains all defined constraint functions)
 * 
 * 
 ***********************************************************************/

#ifndef CONSTRAINTFUNCTIONMANAGER_H
#define CONSTRAINTFUNCTIONMANAGER_H

#include "AutomatonExport.h"
#include "common/AbstractFactoryPattern/Singleton.h"
#include "common/Data/LimaString.h"
#include <map>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class ConstraintFunction;

class LIMA_AUTOMATON_EXPORT ConstraintFunctionManager : 
  public Singleton<ConstraintFunctionManager>
{

 friend class Singleton<ConstraintFunctionManager>;

 public:
  ConstraintFunctionManager(); 
  ~ConstraintFunctionManager();

  ConstraintFunction* 
    getConstraintFunction(const std::string& id,
                          MediaId language,
                          const LimaString& complement=LimaString());

  ConstraintFunction* 
    createConstraintFunction(const std::string& id,
                             MediaId language,
                             const LimaString& complement=LimaString());

  bool getFunctionName(const ConstraintFunction* functionPtr,
                       std::string& id,
                       LimaString& complement) const;

  const std::multimap<std::string,ConstraintFunction*>& 
    getRegisteredFunctions() const { return m_constraintMap; }

 private:
  std::multimap<std::string,ConstraintFunction*> m_constraintMap;
};

} // end namespace
} // end namespace
} // end namespace

#endif
