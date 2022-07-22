// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       constraintFunctionFactory.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Mar 16 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     Automaton
 * 
 * @brief      abstract factory for constraint functions
 * 
 * 
 ***********************************************************************/

#ifndef CONSTRAINTFUNCTIONFACTORY_H
#define CONSTRAINTFUNCTIONFACTORY_H

#include "AutomatonExport.h"
#include "constraintFunction.h"
#include "common/AbstractFactoryPattern/RegistrableFactory.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT AbstractConstraintFunctionFactory : 
  public RegistrableFactory<AbstractConstraintFunctionFactory>
{
 public:
  AbstractConstraintFunctionFactory(const std::string& factoryId); 
  virtual ConstraintFunction* create(MediaId language,
                                     const LimaString& param)=0;
 private:
};

template<typename Constraint> 
class ConstraintFunctionFactory : 
  public AbstractConstraintFunctionFactory
{
 public:
  ConstraintFunctionFactory(const std::string& factoryId):
    AbstractConstraintFunctionFactory(factoryId) {}
  ConstraintFunction* create(MediaId language,
                             const LimaString& param) override {
    ConstraintFunction* newFunction=static_cast<ConstraintFunction*>(new Constraint(language,param));
    return newFunction;
  }
 private:
};

} // end namespace
} // end namespace
} // end namespace

#endif
