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
