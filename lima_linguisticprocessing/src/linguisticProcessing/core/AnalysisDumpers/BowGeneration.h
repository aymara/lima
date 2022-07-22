// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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


  /* create a specific entity (BoWNamedEntity): make this function public so that
   * entities can be created even for vertices that are not in the graph anymore 
   * (to allow BowDumper to dump nested entities)
   */
  boost::shared_ptr< Common::BagOfWords::BoWNamedEntity > createSpecificEntity(
    const LinguisticGraphVertex& vertex,
    const AnnotationGraphVertex& v,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    bool frompos) const;

private:
  BowGeneratorPrivate* m_d;
};

} // Compounds

} // LinguisticProcessing

} // Lima

#endif
