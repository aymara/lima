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
