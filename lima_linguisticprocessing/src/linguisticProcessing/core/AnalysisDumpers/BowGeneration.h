/*
    Copyright 2002-2020 CEA LIST

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
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_COMPOUNDSBOWGENERATION_H
#define LIMA_LINGUISTICPROCESSING_COMPOUNDSBOWGENERATION_H

#include "AnalysisDumpersExport.h"
#include "linguisticProcessing/core/AnalysisDumpers/StopList.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"
#include "linguisticProcessing/common/BagOfWords/BoWPredicate.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"


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
  class BoWNamedEntity;
  class BoWPredicate;
}
}
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{
  class SemanticRelationAnnotation;
}
namespace SyntacticAnalysis
{
  class SyntacticData;
}
namespace AnalysisDumpers
{
  class StopList;
}
namespace Compounds
{

class BowGeneratorPrivate;
/**
  * Parameters retrived in the configuration file:
  * - useStopList
  * - stopList
  * - useEmptyMacro
  * - useEmptyMicro
  * - NEmacroCategories : the list of categories associated to the entities types
  * - properNounCategory
  * - commonNounCategory
  * - keepAllNamedEntityParts
  * - NEnormalization
  * - group : the entities group to use to find the enities names (optional,
  *   default is SpecificEntities)
  * @author Gael de Chalendar
  */
class LIMA_ANALYSISDUMPERS_EXPORT BowGenerator
{
  friend class BowGeneratorPrivate;
public:
  BowGenerator();

  virtual ~BowGenerator();
  BowGenerator(const BowGenerator&) = delete;
  BowGenerator& operator=(const BowGenerator&) = delete;

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    MediaId language)
  ;

  /**
    * @brief Creates the terms reachable from the given annotation vertex.
    * Supposes that all edges pointing to vx are edges to be used in the term
    * Builds terms for vertex vx, possibly extension of vertex tgt. If this is
    * the case, the edge between vx and tgt is used to build the BoWRelations
    * of the returned vector
    */
  std::vector< std::pair< boost::shared_ptr< Common::BagOfWords::BoWRelation >,
                          boost::shared_ptr< Common::BagOfWords::BoWToken > > > buildTermFor(
    const AnnotationGraphVertex& vx,
    const AnnotationGraphVertex& tgt,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    const SyntacticAnalysis::SyntacticData* syntacticData,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    std::set<LinguisticGraphVertex>& visited) const;

  std::vector< std::pair< boost::shared_ptr< Common::BagOfWords::BoWRelation >,
                          boost::shared_ptr< Common::BagOfWords::AbstractBoWElement > > > createAbstractBoWElement(
    const LinguisticGraphVertex v,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offsetBegin,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    std::set<LinguisticGraphVertex>& visited,
    bool keepAnyway = false) const;

  /**
   * Builds a BoWPredicate corresponding to a  semantic relation (an edge in the
   * annotation graph holding a SemanticRelation annotation. Note that if several
   * semantic relations hold between the source and target vertices, then the
   * annotation type value is the comma-concatenated list of the semantic relations
   * ids
   *
   * @param lgvs source linguistic graph vertex
   * @param agvs source annotation graph vertex matching lgvs
   * @param agvt target annotation graph vertex of the relation
   * @param annot the semantic relation itself (holding its type)
   */
  QList< boost::shared_ptr< Common::BagOfWords::BoWPredicate > > createSemanticRelationPredicate(
    const LinguisticGraphVertex& lgvs,
    const AnnotationGraphVertex& agvs,
    const AnnotationGraphVertex& agvt,
    const SemanticAnalysis::SemanticRelationAnnotation& annot,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    uint64_t offset,
    std::set< LinguisticGraphVertex >& visited,
    bool keepAnyway) const;


private:
  BowGeneratorPrivate* m_d;
};

} // Compounds

} // LinguisticProcessing

} // Lima

#endif
