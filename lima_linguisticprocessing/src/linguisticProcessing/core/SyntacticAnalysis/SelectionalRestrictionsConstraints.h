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
/**
  * @brief       this file contains the definitions of several constraint
  *              functions for the disambiguation of dependencies based on
  *              selectional restrictions
  *
  * @file        SelectionalRestrictionsConstraints.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2010 by CEA
  * @date        Created on  Sun May, 16 2010
  * @version     $Id:  $
  *
  *
  */

#ifndef LIMA_SYNTACTICANALYSIS_SELECTIONALRESTRICTIONSCONSTRAINTS_H
#define LIMA_SYNTACTICANALYSIS_SELECTIONALRESTRICTIONSCONSTRAINTS_H

#include "SyntacticAnalysisExport.h"
#include "SyntacticData.h"
#include "HomoSyntagmaticConstraints.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"
#include <iostream>

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

//**********************************************************************
// ids of constraints defined in this file
#define StoreForDisambiguationId "StoreForDisambiguation"
#define DisambiguateWithId "DisambiguateWith"

class SelectionalPreferences;

/** @defgroup SAConstraints Syntactic analysis rules constraints (or action constraints) classes */
///@{

/** @brief This constraint
 */
class LIMA_SYNTACTICANALYSIS_EXPORT StoreForDisambiguation : public ConstraintWithRelationComplement
{
public:
  explicit StoreForDisambiguation(MediaId language,
                     const LimaString& complement=LimaString());
  ~StoreForDisambiguation() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
};


/** @brief This constraint tests if
 */
class LIMA_SYNTACTICANALYSIS_EXPORT DisambiguateWith : public ConstraintWithRelationComplement
{
public:
  explicit DisambiguateWith(MediaId language,
                const LimaString& complement=LimaString());
  ~DisambiguateWith() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
  MediaId m_language;
  SelectionalPreferences* m_preferences;
};


///@}

} // end namespace SyntacticAnalysis
} // end namespace LinguisticProcessing
} // end namespace Lima

#endif // LIMA_SYNTACTICANALYSIS_SELECTIONALRESTRICTIONSCONSTRAINTS_H
