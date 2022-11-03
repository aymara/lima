// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief       Data used for Semantic Relation Annotation
  *
  * @file        SemanticRelationData.h
  * @author      Faiza Gara (Faiza.Gara@cea.fr) 

  *              Copyright (c) 2007 by CEA
  * @date        Created on Feb, 16 2007
  * @version     $Id: 
  *
  */

#ifndef LIMA_ANNOTATION_SEMANTICRELATIONDATA_H
#define LIMA_ANNOTATION_SEMANTICRELATIONDATA_H

#include "SemanticAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/Data/strwstrtools.h"

/// To use tuples
#include "boost/tuple/tuple.hpp"
///Comparison operators can be included with:
#include "boost/tuple/tuple_comparison.hpp"
/// To use tuple input and output operators,
#include "boost/tuple/tuple_io.hpp"


namespace Lima
{
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{

class LIMA_SEMANTICANALYSIS_EXPORT SemanticRelation :public boost::tuple< LinguisticGraphVertex, LinguisticGraphVertex, std::string >
{
  public:
    SemanticRelation(const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  const std::string relationType)
  :boost::tuple<LinguisticGraphVertex,LinguisticGraphVertex,std::string>(v1,v2,relationType){};
};

class LIMA_SEMANTICANALYSIS_EXPORT SemanticRelationData : public Lima::AnalysisData
{
public:

  SemanticRelationData();

  virtual ~SemanticRelationData();

  inline bool clearStoredRelations()
  {
    m_relations.clear();
    return true;
  }
    
  /// Stores SemanticRelation
  bool relation(const LinguisticGraphVertex& v1,
                         const LinguisticGraphVertex& v2,
                         const std::string semanticRelationType);


  ///add SemanticRelation instance in the annotationGraph
  bool addRelations(AnalysisContent& analysis);


private:
 
  ///list of Relations to add in the annotationGraph
  std::set< SemanticRelation > m_relations;
    

};


}
}
}

#endif
