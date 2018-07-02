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
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_SEMANTICANALYSISBOWGENERATION_H
#define LIMA_LINGUISTICPROCESSING_SEMANTICANALYSISBOWGENERATION_H

#include "SemanticAnalysisExport.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"




namespace Lima
{
namespace Common
{
namespace AnnotationGraphs
{
  class AnnotationData;
}
namespace BagOfWords
{
  class BoWPredicate;
}
}
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{
  class SemanticRelationAnnotation;
}
namespace Compounds
{
  class BowGenerator;

/** 
 * @NOTE These functions are the beginning of an adaptation to make semantic 
 * analysis a real plugin, this with no build time dependencies from lima core 
 * to it
**/
  

/**
  * Builds a BoWPredicate corresponding to a  semantic relation (an edge in the
  * annotation graph holding a SemanticRelation annotation
  *
  * @param lgvs source linguistic graph vertex
  * @param agvs source annotation graph vertex matching lgvs
  * @param lgvt target linguistic graph vertex
  * @param agvt target annotation graph vertex matching lgvt
  * @param age annotation graph edge porting the   semantic relation
  */
boost::shared_ptr< Common::BagOfWords::BoWPredicate > createPredicate(
  BowGenerator* generator,
  const LinguisticGraphVertex& lgvs,
  const AnnotationGraphVertex& agvs,
  const AnnotationGraphVertex& agvt,
  const SemanticAnalysis::SemanticRelationAnnotation& annot,
  const Common::AnnotationGraphs::AnnotationData* annotationData,
  const LinguisticGraph& anagraph,
  const LinguisticGraph& posgraph,
  uint64_t offset,
  std::set< LinguisticGraphVertex >& visited,
  bool keepAnyway);


} // Compounds

} // LinguisticProcessing

} // Lima

#endif
