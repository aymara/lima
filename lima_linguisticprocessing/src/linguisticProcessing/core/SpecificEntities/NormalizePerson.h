// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       NormalizePerson.h
 * @author     Olivier Mesnarrd (olivier.mesnard@cea.fr)
 * @date       Wed Jan 13 2015
 * copyright   Copyright (C) 2006-2015 by CEA LIST
 * Project     lima_linguisticprocessing
 * 
 * @brief      action for the normalization of person (replace normalization of person name)
 * 
 * 
 ***********************************************************************/

#ifndef NORMALIZEPERSON_H
#define NORMALIZEPERSON_H

#include "SpecificEntitiesExport.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

//**********************************************************************
// ids of actions defined in this file
#define NormalizePersonId "NormalizePerson"

class LIMA_SPECIFICENTITIES_PERSON_EXPORT NormalizePerson : public Automaton::ConstraintFunction
{
public:
  NormalizePerson(MediaId language,
                      const LimaString& complement=LimaString());
  ~NormalizePerson() {}

  // function to normalize person names specifying firstname and lastname
/*   bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph, */
/*                   const LinguisticGraphVertex& v1, */
/*                   const LinguisticGraphVertex& v2, */
/*                   AnalysisContent& analysis) const; */

  // function to normalize person names using a simple heuristic on result
  // to separate firstname from lastname
  bool operator()(Automaton::RecognizerMatch& m,
                  AnalysisContent& analysis) const override;

  bool actionNeedsRecognizedExpression() override { return true; }

private:
  MediaId m_language;
  LimaString m_firstname; // values of firstname and lastname
  LimaString m_lastname;  // possibly given as arguments of the action
  const std::set<LinguisticCode>* m_microsForFirstname;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
};

} // end namespace
} // end namespace
} // end namespace

#endif
