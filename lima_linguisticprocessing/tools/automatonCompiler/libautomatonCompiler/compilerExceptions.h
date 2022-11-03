// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
*
* File        : compilerExceptions.h
* Project     : Named Entities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Tue Apr  8 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: compilerExceptions.h 2199 2005-02-28 18:04:09Z mathieub $
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
    LimaException(mes) {};
  ~AutomatonCompilerException() throw() {};
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
