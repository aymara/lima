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
 * @file       NormalizePersonName.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Jun 13 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     s2lp
 * 
 * @brief      action for the normalization of person names
 * 
 * 
 ***********************************************************************/

#ifndef NORMALIZEPERSONNAME_H
#define NORMALIZEPERSONNAME_H

#include "SpecificEntitiesExport.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

//**********************************************************************
// ids of actions defined in this file
#define NormalizePersonNameId "NormalizePersonName"

class LIMA_SPECIFICENTITIES_PERSON_EXPORT NormalizePersonName : public Automaton::ConstraintFunction
{
public:
  NormalizePersonName(MediaId language,
                      const LimaString& complement=LimaString());
  ~NormalizePersonName() {}

  // function to normalize person names specifying firstname and lastname
/*   bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph, */
/*                   const LinguisticGraphVertex& v1, */
/*                   const LinguisticGraphVertex& v2, */
/*                   AnalysisContent& analysis) const; */

  // function to normalize person names using a simple heuristic on result
  // to separate firstname from lastname
  bool operator()(Automaton::RecognizerMatch& m,
                  AnalysisContent& analysis) const;

  bool actionNeedsRecognizedExpression() { return true; }
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
