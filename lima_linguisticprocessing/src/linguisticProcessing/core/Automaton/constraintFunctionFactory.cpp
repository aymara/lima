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
