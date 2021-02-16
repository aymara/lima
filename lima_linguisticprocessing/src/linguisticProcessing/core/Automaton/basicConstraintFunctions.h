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
 * @file       basicConstraintFunctions.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed Mar 16 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     Automaton
 *
 * @brief this file contains the definitions of several basic
 * constraint functions (agreement etc)
 *
 *
 ***********************************************************************/

#ifndef BASICCONSTRAINTFUNCTIONS_H
#define BASICCONSTRAINTFUNCTIONS_H

#include "AutomatonExport.h"
#include "constraintFunction.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

//**********************************************************************
// ids of constraints defined in this file
#define AgreementConstraintId "AgreementConstraint"
#define GenderAgreementId "GenderAgreement"
#define NumberAgreementId "NumberAgreement"
#define LinguisticPropertyIsId "LinguisticPropertyIs"
#define LengthInIntervalId "LengthInInterval"
#define NumericValueInIntervalId "NumericValueInIntervalId"
#define NoSpaceWithId "NoSpaceWith"

/**
 * generic agreement constraint function: complement
 * must contain the element on which the agreement must stand:
 * GENDER,NUMBER or PERSON
 */
class LIMA_AUTOMATON_EXPORT AgreementConstraint : public ConstraintFunction
{
public:
  AgreementConstraint(MediaId language,
                      const LimaString& complement=LimaString());
  ~AgreementConstraint() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;
private:
  const Common::PropertyCode::PropertyAccessor* m_categoryForAgreementAccessor;
};

// specializations of agreement constraints
class LIMA_AUTOMATON_EXPORT GenderAgreement : public AgreementConstraint
{
public:
  GenderAgreement(MediaId language,
                  const LimaString& complement=LimaString());
  ~GenderAgreement() {}
private:
};

class LIMA_AUTOMATON_EXPORT NumberAgreement : public AgreementConstraint
{
public:
  NumberAgreement(MediaId language,
                  const LimaString& complement=LimaString());
  ~NumberAgreement() {}
private:
};

/**
 * test the value of a linguistic property
 * @param complement the complement contains the name of the
 * property and the value to test, separated by a comma
 */
class LIMA_AUTOMATON_EXPORT LinguisticPropertyIs : public ConstraintFunction
{
public:
  LinguisticPropertyIs(MediaId language,
                       const LimaString& complement=LimaString());
  ~LinguisticPropertyIs() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v,
                  AnalysisContent& analysis) const override;

private:
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
  std::vector<std::pair<LinguisticCode,LinguisticProcessing::LinguisticAnalysisStructure::MorphoSyntacticType> > m_values;
};

/**
 * test the length of the inflected form of the word
 */
class LIMA_AUTOMATON_EXPORT LengthInInterval : public ConstraintFunction
{
public:
  LengthInInterval(MediaId language,
                   const LimaString& complement=LimaString());
  ~LengthInInterval() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v,
                  AnalysisContent& analysis) const override;

private:
  uint64_t m_min;
  uint64_t m_max;
};

/**
 * test the length of the inflected form of the word
 */
class LIMA_AUTOMATON_EXPORT NumericValueInInterval : public ConstraintFunction
{
public:
  NumericValueInInterval(MediaId language,
                   const LimaString& complement=LimaString());
  ~NumericValueInInterval() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v,
                  AnalysisContent& analysis) const override;

private:
  MediaId m_language;
  uint64_t m_min;
  uint64_t m_max;
};

/* check if the considered token has no whitespace separator with the other token
 * (e.g. '-' is a minus sign if not separated from following number)
 */
class LIMA_AUTOMATON_EXPORT NoSpaceWith : public ConstraintFunction
{
public:
  NoSpaceWith(MediaId language,
                      const LimaString& complement=LimaString());
  ~NoSpaceWith() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;
private:
};


} // end namespace
} // end namespace
} // end namespace

#endif
