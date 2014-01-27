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
* File        : compilerExceptions.h
* Project     : Named Entities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Tue Apr  8 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id$
*
* Description : exceptions than can be thrown during the compilation of rules
*
************************************************************************/

#ifndef COMPILEREXCEPTIONS_H
#define COMPILEREXCEPTIONS_H

#include "common/LimaCommon.h"
#include <stdexcept>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class AutomatonCompilerException : public LimaException {
public:
  AutomatonCompilerException(const std::string& mes):
    LimaException(),m_msg(mes) {};
  ~AutomatonCompilerException() throw() {};
  std::string what() { return m_msg; }
protected:
  std::string m_msg;
};

/***********************************************************************/
// exception thrown by the recognizer compiler
class RecognizerCompilerException : public AutomatonCompilerException {
public: 
  RecognizerCompilerException(const std::string& mes):
    AutomatonCompilerException(mes) {};
  ~RecognizerCompilerException() throw() {};
};

/***********************************************************************/
// exception thrown by automatonRegexp in case of syntax error
class ExpressionSyntaxException : public AutomatonCompilerException {
public:
  ExpressionSyntaxException(const std::string& mes):
    AutomatonCompilerException(mes){};
  ~ExpressionSyntaxException() throw() {};
};

/***********************************************************************/
// exception thrown by automatonCompiler 
class AutomatonErrorException : public AutomatonCompilerException {
public: 
  AutomatonErrorException(const std::string& mes):
    AutomatonCompilerException(mes){};
  ~AutomatonErrorException() throw() {};
};

/***********************************************************************/
// exception thrown by RuleCompiler in case of erroneous constraint syntax
class ConstraintSyntaxException : public AutomatonCompilerException {
public: 
  ConstraintSyntaxException(const std::string& mes):
    AutomatonCompilerException(mes){};
  ~ConstraintSyntaxException() throw() {};
};

/***********************************************************************/
// exception thrown by RecognizerCompiler in case of erroneous type definition
class TypeDefSyntaxException : public AutomatonCompilerException {
public: 
  TypeDefSyntaxException(const std::string& mes):
    AutomatonCompilerException(mes){};
  ~TypeDefSyntaxException() throw() {};
};


} // end namespace
} // end namespace
} // end namespace

#endif
