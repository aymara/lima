// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       constraintFunctionFactory.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Mar 16 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "constraintFunctionFactory.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

AbstractConstraintFunctionFactory::
AbstractConstraintFunctionFactory(const std::string& factoryId):
RegistrableFactory<AbstractConstraintFunctionFactory>(factoryId)
{
}

} // end namespace
} // end namespace
} // end namespace
