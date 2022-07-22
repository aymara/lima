// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  LDEBUG << "CreateSemanticRelation::operator()" << vertex1 << vertex2
          << m_semanticRelationType;
#endif
  LIMA_UNUSED(anagraph);
  auto semanticData=static_cast<SemanticRelationData*>(analysis.getData("SemanticRelationData"));
  if (semanticData == nullptr)
  {
    semanticData=new SemanticRelationData();
    analysis.setData("SemanticRelationData",semanticData);

  }

  return semanticData->relation(vertex1,vertex2,m_semanticRelationType);

}



} // end namespace
} // end namespace
} // end namespace
