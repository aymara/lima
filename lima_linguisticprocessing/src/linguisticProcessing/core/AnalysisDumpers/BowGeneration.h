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
public:
  BowGenerator();
  
  virtual ~BowGenerator();

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
  std::vector< std::pair<Common::BagOfWords::BoWRelation*,Common::BagOfWords::BoWToken*> > buildTermFor(
    const AnnotationGraphVertex& vx,
    const AnnotationGraphVertex& tgt,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    const SyntacticAnalysis::SyntacticData* syntacticData,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    std::set<LinguisticGraphVertex>& visited) const;

  std::vector< std::pair<Common::BagOfWords::BoWRelation*,Common::BagOfWords::BoWToken*> > createBoWTokens(
    const LinguisticGraphVertex v,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offsetBegin,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    std::set<LinguisticGraphVertex>& visited,
    bool keepAnyway = false) const;


private:

  Common::BagOfWords::BoWRelation* createBoWRelationFor(
    const AnnotationGraphVertex& vx, 
    const AnnotationGraphVertex& tgt,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const LinguisticGraph& posgraph,
    const SyntacticAnalysis::SyntacticData* syntacticData) const;

  class NamedEntityPart
  {
    public:
      NamedEntityPart(): inflectedForm(), lemma(), category(0), position(0), 
      length(0) {}
      NamedEntityPart(const LimaString& fl, const LimaString& l, 
                      const uint64_t cat, const uint64_t pos, 
                      const uint64_t len):
          inflectedForm(fl), lemma(l), category(cat), position (pos), 
          length(len) {}

      LimaString inflectedForm;
      LimaString lemma;
      uint64_t category;
      uint64_t position;
      uint64_t length;
  };

  typedef std::set< std::pair<uint64_t,uint64_t> > TokenPositions;

  MediaId m_language;
  AnalysisDumpers::StopList* m_stopList;
  bool m_useStopList;
  bool m_useEmptyMacro;
  bool m_useEmptyMicro;
  LinguisticCode m_properNounCategory;
  LinguisticCode m_commonNounCategory;
  bool m_keepAllNamedEntityParts;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;

  // what to assign to the BoWNamedEntity lemma
  typedef enum {
    NORMALIZE_NE_INFLECTED, // assign inflected form
    NORMALIZE_NE_LEMMA,     // assign lemma
    NORMALIZE_NE_NORMALIZEDFORM, // assign normalized form from NE
    NORMALIZE_NE_NETYPE // assign type of NE
  } NENormalization;
  NENormalization m_NEnormalization;

  Common::BagOfWords::BoWNamedEntity* createSpecificEntity(
    const LinguisticGraphVertex& vertex,
    const AnnotationGraphVertex& v,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    bool frompos = true) const;

  Common::BagOfWords::BoWToken* createCompoundTense(
    const AnnotationGraphVertex& v,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    std::set<LinguisticGraphVertex>& visited) const;

  Common::BagOfWords::BoWPredicate* createPredicate(const Common::MediaticData::EntityType& t, QMultiMap<Common::MediaticData::EntityType, Common::BagOfWords::AbstractBoWElement*> roles,
    const Common::AnnotationGraphs::AnnotationData* annotationData) const;

  bool checkStopWordInCompound(
    Common::BagOfWords::BoWToken*&,
    uint64_t offset,
    std::set< std::string >& alreadyStored,
    Common::BagOfWords::BoWText& bowText) const;

  StringsPoolIndex getNamedEntityNormalization(
      const AnnotationGraphVertex& v,
      const Common::AnnotationGraphs::AnnotationData* annotationData) const;

  bool shouldBeKept(const LinguisticAnalysisStructure::LinguisticElement& elem) const;

  /**
     * create the parts of a named entity (depends on its type,
     * but may be independant from its components, that can be
     * erroneously tagged)
     *
     * @param v the annotation graph vertex handling the named entity
     *
     * @return a list of the parts to consider, composed of an inflected
     * form, a lemma and a category
   */
  std::vector<NamedEntityPart> createNEParts(
      const AnnotationGraphVertex& v,
                                              const Common::AnnotationGraphs::AnnotationData* annotationData,
                                              const LinguisticGraph& anagraph,
                                              const LinguisticGraph& posgraph,
                                            bool frompos = true) const;

  void bowTokenPositions(TokenPositions& res,
                         const Common::BagOfWords::BoWToken* tok) const;

  uint64_t computeCompoundLength(const TokenPositions& headTokPositions,
                                     const TokenPositions& extensionPositions) const;
};

} // Compounds

} // LinguisticProcessing

} // Lima

#endif
