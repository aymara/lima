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
