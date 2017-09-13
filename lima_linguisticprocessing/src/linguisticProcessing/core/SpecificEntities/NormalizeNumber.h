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
