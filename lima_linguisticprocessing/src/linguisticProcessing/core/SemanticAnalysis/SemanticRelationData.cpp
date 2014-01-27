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
  * @version     $Id: SemanticRelationData.cpp $
  *
  */

#include "SemanticRelationData.h"
#include "SemanticRelationAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/Automaton/EntityFeatures.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace boost;

namespace Lima{
namespace LinguisticProcessing{
namespace SemanticAnalysis{

SemanticRelationData::SemanticRelationData() :
    m_relations()
{
  

}


SemanticRelationData::~SemanticRelationData()
{
  m_relations.clear();
}


bool SemanticRelationData::addRelations(AnalysisContent& analysis)
{
//   PROCESSORSLOGINIT;
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));

  if (annotationData->dumpFunction("SemanticRelation") == 0)
  {
    annotationData->dumpFunction("SemanticRelation", new DumpSemanticRelation());
  }
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));

  for (std::set<SemanticRelation>::iterator i=m_relations.begin();
       i != m_relations.end(); i++) 
  {
    LinguisticGraphVertex vertex1 = i->get<0>();
    LinguisticGraphVertex vertex2 = i->get<1>();

    std::set< AnnotationGraphVertex > matchesVtx1 = annotationData->matches(recoData->getGraphId(),vertex1,"annot");
    std::set< AnnotationGraphVertex > matchesVtx2 = annotationData->matches(recoData->getGraphId(),vertex2,"annot");

    if (!annotationData->hasAnnotation(*(matchesVtx1.begin()), *(matchesVtx2.begin()),
         Common::Misc::utf8stdstring2limastring("SemanticRelation")))
    {
      SemanticRelationAnnotation annot(i->get<2>());
      GenericAnnotation ga(annot);
      annotationData->annotate(*(matchesVtx1.begin()), *(matchesVtx2.begin()),
      Common::Misc::utf8stdstring2limastring("SemanticRelation"), ga);
    }
  }
  m_relations.clear();
  return true;
}



}
}
}
