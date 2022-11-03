// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2006 by Romuald   *
 *   romuald.bourre@cea.fr   *
 *                                                                         *
 ***************************************************************************/

#ifndef PATTERNPROCESSING_CONSTRAINTFUNCTION_H
#define PATTERNPROCESSING_CONSTRAINTFUNCTION_H

#include "SemanticAnalysisExport.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"


namespace Lima
{
namespace LinguisticProcessing 
{
namespace SemanticAnalysis
{

//**********************************************************************
// ids of constraints defined in this file
#define CreateSemanticRelationId "CreateSemanticRelation"
#define ClearSemanticRelationId "ClearSemanticRelation"
#define SaveSemanticRelationId "SaveSemanticRelation"

class LIMA_SEMANTICANALYSIS_EXPORT CreateSemanticRelation : public Automaton::ConstraintFunction
{
public:
  CreateSemanticRelation(MediaId language,
                         const LimaString& complement=LimaString());
  ~CreateSemanticRelation() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;
private:
  std::string m_semanticRelationType;
};

class LIMA_SEMANTICANALYSIS_EXPORT ClearSemanticRelation : public Automaton::ConstraintFunction
{
  public:
    ClearSemanticRelation(MediaId language,
                           const LimaString& complement=LimaString());
    ~ClearSemanticRelation() {}
    bool operator()( AnalysisContent& analysis) const override;
};

class LIMA_SEMANTICANALYSIS_EXPORT SaveSemanticRelation : public Automaton::ConstraintFunction
{
  public:
    SaveSemanticRelation(MediaId language,
                           const LimaString& complement=LimaString());
    ~SaveSemanticRelation() {}
    bool operator()( AnalysisContent& analysis) const override;
};


} // end namespace
} // end namespace
} // end namespace

#endif
