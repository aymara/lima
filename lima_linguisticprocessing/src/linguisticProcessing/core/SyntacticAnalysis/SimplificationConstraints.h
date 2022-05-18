// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @brief       this file contains the definitions of several constraint
  *              functions for the detection of subsentences
  *
  * @file        SimplificationConstraints.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2005 by CEA
  * @date        Created on Tue Mar, 15 2005
  *
  *
  */

#ifndef LIMA_SYNTACTICANALYSIS_SIMPLIFICATIONCONSTRAINTS_H
#define LIMA_SYNTACTICANALYSIS_SIMPLIFICATIONCONSTRAINTS_H

#include "SyntacticAnalysisExport.h"
#include "HomoSyntagmaticConstraints.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"

#include <iostream>

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

//**********************************************************************
// ids of constraints defined in this file
#define SubsentenceBoundsId "SubsentenceBounds"
#define SimplifyId "Simplify"
#define ClearStoredSubsentencesId "ClearStoredSubsentences"

//**********************************************************************
class LIMA_SYNTACTICANALYSIS_EXPORT SubsentenceBounds : public ConstraintWithRelationComplement
{
public:
  explicit SubsentenceBounds(MediaId language,
             const LimaString& complement=LimaString());
  ~SubsentenceBounds() {}
  
  bool operator()(const Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v,
                  AnalysisContent& analysis) const override;
  bool operator()(const Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;
private:
  uint64_t m_language;
};



class LIMA_SYNTACTICANALYSIS_EXPORT Simplify : public ConstraintWithRelationComplement
{
public:
  explicit Simplify(MediaId language,
                    const LimaString& complement=LimaString());
  ~Simplify() {}
  
  bool operator()(Automaton::RecognizerMatch& /*unused*/,
                AnalysisContent& analysis) const override;

  bool actionNeedsRecognizedExpression() override { return true; }

private:
  uint64_t m_language;
};

class LIMA_SYNTACTICANALYSIS_EXPORT ClearStoredSubsentences : public ConstraintWithRelationComplement
{
public:
  explicit ClearStoredSubsentences(MediaId language,
           const LimaString& complement=LimaString());
  ~ClearStoredSubsentences() {}
  
  bool operator()(AnalysisContent& analysis) const override;
  
};


} // end namespace SyntacticAnalysis
} // end namespace LinguisticProcessing
} // end namespace Lima

#endif // LIMA_SYNTACTICANALYSIS_SIMPLIFICATIONCONSTRAINTS_H
