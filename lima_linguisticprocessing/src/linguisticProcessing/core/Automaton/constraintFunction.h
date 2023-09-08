// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       constraintFunction.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Sep  1 2003
 * copyright   Copyright (C) 2003 by CEA LIST
 * Project     Automaton
 *
 * @brief      this class describes the basic constraint visitor
 * (used in the recognizer)
 *
 *
 ***********************************************************************/

#ifndef CONSTRAINTFUNCTION_H
#define CONSTRAINTFUNCTION_H

#include "constraintFunctionManager.h"
#include "recognizerMatch.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include <map>
#include <string>
#include <iostream>
#include <stdexcept>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class ConstraintFunction
{
 public:
  virtual ~ConstraintFunction() {};

  /**
   * binary constraint function : the constraint only applies on two
   * vertices in the graph (the graph is also given as a parameter).
   * Function is not pure virtual because it can be unary or binary.
   *
   * @return true if the constraint is verified, false otherwise
   */
  virtual bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& /*unused*/,
                          const LinguisticGraphVertex& /*unused*/,
                          const LinguisticGraphVertex& /*unused*/,
                          AnalysisContent& /*unused*/) const {
    std::string functionName;
    LimaString complement;
    ConstraintFunctionManager::single().
      getFunctionName(this,functionName,complement);
    std::cerr << "binary constraint function "
              << functionName
              << " not defined" << std::endl;
    return true;
  }

  /**
   * unary constraint function : the constraint only applies on one
   * vertices in the graph (the graph is also given as a parameter).
   * Function is not pure virtual because it can be unary or binary.
   *
   * @return true if the constraint is verified, false otherwise
   */
  virtual bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& /*unused*/,
                          const LinguisticGraphVertex& /*unused*/,
                          AnalysisContent& /*unused*/) const {

    std::string functionName;
    LimaString complement;
    ConstraintFunctionManager::single().
      getFunctionName(this,functionName,complement);
    std::cerr << "unary constraint function "
              << functionName
              << " not defined" << std::endl;
    return true;
  }

  /**
   * zero-ary constraint function :
   * applies the function without a vertex indication (used for actions,
   * not constraints -> applied only at end, when a match is found)
   *
   * @return true if the action of the function succeeded, false otherwise
   */
  virtual bool operator()(AnalysisContent& /*unused*/) const {

    std::string functionName;
    LimaString complement;
    ConstraintFunctionManager::single().
      getFunctionName(this,functionName,complement);
    std::cerr << "action function "
              << functionName
              << " not defined" << std::endl;
    return true;
  }

  /**
   * zero-ary constraint function :
   * applies the function without a vertex indication (used for actions,
   * not constraints -> applied only at end, when a match is found:
   * uses the result of the match
   *
   * @return true if the action of the function succeeded, false otherwise
   */
  virtual bool operator()(RecognizerMatch& /*unused*/,
                          AnalysisContent& /*unused*/) const {

    std::string functionName;
    LimaString complement;
    ConstraintFunctionManager::single().
      getFunctionName(this,functionName,complement);
    std::cerr << "action function (using recognized expression) "
              << functionName
              << " not defined" << std::endl;
    return true;
  }

  virtual bool actionNeedsRecognizedExpression() { return false; }

  const LimaString& getComplementString() { return m_complement; }
  unsigned char getLanguage() { return m_language_id; }

 protected:
  explicit ConstraintFunction(MediaId language,
                     const LimaString& complement=LimaString()):
    m_language_id(language),
    m_complement(complement) {}

  // a unusual name to be different from existing m_language in existing
  // constraints => @todo : remove m_language from existing contraints
  MediaId m_language_id;
  LimaString m_complement;
};

} // end namespace
} // end namespace
} // end namespace

#endif
