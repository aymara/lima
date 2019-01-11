/*
    Copyright 2002-2019 CEA LIST

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
 *
 ***************************************************************************/

#include "ConstraintFunction.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "SemanticRelationAnnotation.h"
#include "SemanticRelationData.h"

#include <iostream>

#include <algorithm>

using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;

#define SEMLOGINIT  LOGINIT("LP::SemanticAnalysis")

namespace Lima
{
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{

//**********************************************************************
// factories for constraints defined in this file
Automaton::ConstraintFunctionFactory<CreateSemanticRelation>
CreateSemanticRelationFactory(CreateSemanticRelationId);


Automaton::ConstraintFunctionFactory<ClearSemanticRelation>
ClearSemanticRelationFactory(ClearSemanticRelationId);

Automaton::ConstraintFunctionFactory<SaveSemanticRelation>
SaveSemanticRelationFactory(SaveSemanticRelationId);

//**********************************************************************
ClearSemanticRelation::ClearSemanticRelation(MediaId language,
                       const LimaString& complement):
ConstraintFunction(language,complement)
{
}

bool ClearSemanticRelation::operator()(AnalysisContent& analysis ) const
{
#ifdef DEBUG_LP
  SEMLOGINIT;
  LDEBUG << "ClearSemanticRelation::operator()";
#endif
  SemanticRelationData * semanticData=static_cast<SemanticRelationData*>(analysis.getData("SemanticRelationData"));
  if (semanticData==0)
  {
    return false;
  }
  else
  {
    return semanticData->clearStoredRelations();
  }
}

SaveSemanticRelation::SaveSemanticRelation(MediaId language,
                       const LimaString& complement):
    ConstraintFunction(language,complement)
{
}

bool SaveSemanticRelation::operator()(AnalysisContent& analysis ) const
{
#ifdef DEBUG_LP
  SEMLOGINIT;
  LDEBUG << "SaveSemanticRelation::operator()";
#endif
  SemanticRelationData * semanticData=static_cast<SemanticRelationData*>(analysis.getData("SemanticRelationData"));
  if (semanticData==0)
  {
    return false;
  }
  else
  {
    return semanticData->addRelations(analysis);
  }
}

CreateSemanticRelation::CreateSemanticRelation(MediaId language,
                       const LimaString& complement):
    ConstraintFunction(language,complement)
{
  m_semanticRelationType = Common::Misc::limastring2utf8stdstring(complement);
}

bool CreateSemanticRelation::
operator()(const LinguisticAnalysisStructure::AnalysisGraph& anagraph,
           const LinguisticGraphVertex& vertex1,
           const LinguisticGraphVertex& vertex2,
           AnalysisContent& analysis ) const
{
#ifdef DEBUG_LP
  SEMLOGINIT;
  LDEBUG << "CreateSemanticRelation::operator()" << vertex1 << vertex2 << m_semanticRelationType;
#endif
  LIMA_UNUSED(anagraph);
  SemanticRelationData * semanticData=static_cast<SemanticRelationData*>(analysis.getData("SemanticRelationData"));
  if (semanticData==0)
  {
    semanticData=new SemanticRelationData();
    analysis.setData("SemanticRelationData",semanticData);

  }

  return semanticData->relation(vertex1,vertex2,m_semanticRelationType);

}



} // end namespace
} // end namespace
} // end namespace
