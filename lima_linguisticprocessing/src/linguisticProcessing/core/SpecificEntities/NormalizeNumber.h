// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       NormalizeNumber.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Jun 13 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     s2lp
 * 
 * @brief      action for normalization of number expressions
 * 
 * 
 ***********************************************************************/

#ifndef NORMALIZENUMBER_H
#define NORMALIZENUMBER_H

#include "SpecificEntitiesExport.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

//**********************************************************************
// ids of actions defined in this file
#define NormalizeNumberId "NormalizeNumber"

class LIMA_SPECIFICENTITIES_NUMBER_EXPORT NormalizeNumber : public Automaton::ConstraintFunction
{
public:
  NormalizeNumber(MediaId language,
                  const LimaString& complement=LimaString());
  ~NormalizeNumber() {}

  bool operator()(Automaton::RecognizerMatch& m,
                  AnalysisContent& analysis) const override;

  bool actionNeedsRecognizedExpression() override { return true; }
private:
  MediaId m_language;
  const std::set<LinguisticCode>* m_microsForNumber;
  const std::set<LinguisticCode>* m_microsForUnit;
  const std::set<LinguisticCode>* m_microsForConjunction;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;

  // private member functions
  double getNumberValue(LinguisticAnalysisStructure::Token* t,
                        LinguisticAnalysisStructure::MorphoSyntacticData* data) const;
};

} // end namespace
} // end namespace
} // end namespace

#endif
