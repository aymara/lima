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
 * @file       constraintFunctionManager.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Mar 16 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "constraintFunctionManager.h"
#include "constraintFunctionFactory.h"
#include "constraintFunction.h"
#include "boost/tuple/tuple.hpp" // for tie

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

ConstraintFunctionManager::ConstraintFunctionManager():
m_constraintMap()
{
}

ConstraintFunctionManager::~ConstraintFunctionManager() 
{
  std::multimap<std::string,ConstraintFunction*>::iterator
    func=m_constraintMap.begin(),
    func_end=m_constraintMap.end();
  
  for (;func!=func_end;func++) {
    if ((*func).second!=0) {
      delete (*func).second;
      (*func).second=0;
    }
  }
  m_constraintMap.clear();
}

ConstraintFunction* ConstraintFunctionManager::
getConstraintFunction(const std::string& id,
                      MediaId language,
                      const LimaString& complement) {
//   AULOGINIT;
//   LDEBUG << "getConstraintFunction: "<< id
//          << "," << complement;
  
  std::multimap<std::string,ConstraintFunction*>::const_iterator
    it,it_end;
  boost::tie(it,it_end)=m_constraintMap.equal_range(id);
  for (;it!=it_end; it++) {
    if ((*it).second->getComplementString() == complement && 
  (*it).second->getLanguage() == language) {
//       LDEBUG << "getConstraintFunction: function exist: " 
//              << (*it).first << "," << (*it).second->getComplementString() 
//              << "->" << (*it).second;
      return (*it).second;
    }
  }
//   LDEBUG << "getConstraintFunction: function does not exist"<< LENDL;
  return createConstraintFunction(id,language,complement);
}

// not efficient but only for debug
bool ConstraintFunctionManager::
getFunctionName(const ConstraintFunction* functionPtr,
                std::string& id,
                LimaString& complement) const {
  std::multimap<std::string,ConstraintFunction*>::const_iterator
    it=m_constraintMap.begin(),
    it_end=m_constraintMap.end();
  for (;it!=it_end; it++) {
    if ((*it).second==functionPtr) {
      id=(*it).first;
      complement=(*it).second->getComplementString();
      return true;
    }
  }
  return false;
}

ConstraintFunction* ConstraintFunctionManager::
createConstraintFunction(const std::string& id,
                         MediaId language,
                         const LimaString& complement) {


  ConstraintFunction* newConstraint=
    AbstractConstraintFunctionFactory::
    getFactory(id)->create(language,complement);
  m_constraintMap.insert(make_pair(id,newConstraint));

//   AULOGINIT;
//   LDEBUG << "createConstraintFunction: "<< id
//          << "," << complement << "->" << newConstraint
//         ;

  return newConstraint;
}


} // end namespace
} // end namespace
} // end namespace
