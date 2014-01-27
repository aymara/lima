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
#ifndef LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESSPECIFICENTITIESCONSTRAINTS_H
#define LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESSPECIFICENTITIESCONSTRAINTS_H

#include "SpecificEntitiesExport.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"
#include "common/MediaticData/EntityType.h"
#include "common/misc/fsaStringsPool.h"

namespace Lima {

namespace LinguisticProcessing {

namespace SpecificEntities {

// ids for constraints in this file
#define isASpecificEntityId "isASpecificEntity"
#define isInSameSpecificEntityId "isInSameSpecificEntity"
#define CreateSpecificEntityId "CreateSpecificEntity"

/**
@author Benoit Mathieu
*/
class LIMA_SPECIFICENTITIES_EXPORT isASpecificEntity : public Automaton::ConstraintFunction
{
public:
  isASpecificEntity(MediaId language,
                    const LimaString& complement=LimaString());
  ~isASpecificEntity() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v,
                  AnalysisContent& analysis) const;

private:
  Common::MediaticData::EntityType m_type;
};

class LIMA_SPECIFICENTITIES_EXPORT isInSameSpecificEntity : public Automaton::ConstraintFunction
{
public:
  isInSameSpecificEntity(MediaId language,
                         const LimaString& complement=LimaString());
  ~isInSameSpecificEntity() {}

  /** @brief Tests if the two given vertices are in the same specific entity
   *
   * There is several cases:
   *   - va1 and va2 are SE vertices : true iff va1 == va2
   *   - va1 and va2 are standard vertices : true iff there is an outgoing edge in
   *     the annotation graph annotated with "belongstose" from each of them and
   *     toward the same vertex
   *   - va1 (va2) is a SE vertex and there is an outgoing edge in the annotation
   *     graph annotated with "belongstose" from va2 (va1) to va1 (va2).
   *
   * In all the cases, va1 and va2 are the uniq "morphannot" matches of v1 and v2
  */
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const;
  
private:
  Common::MediaticData::EntityType m_type;
};

/** 
 * @brief This action creates a new vertex for given match containing a 
 * specific entity's data
 *
 * Algorithm to set the new vertex linguistic properties:
 *   - if a list of micro-categories is given in the complement and the entity
 *     head have one (or more) of these micro categories, then the head micro
 *     categories present in the complement list are kept
 *   - elsewhere, the mapping present in the configuration files between the 
 *     entity type and a micro-category is used
 */
class LIMA_SPECIFICENTITIES_EXPORT CreateSpecificEntity : public Automaton::ConstraintFunction
{
public:
  CreateSpecificEntity(MediaId language,
                       const LimaString& complement=LimaString());
  ~CreateSpecificEntity() {}
  bool operator()(Automaton::RecognizerMatch& match,
                  AnalysisContent& analysis) const;

  bool actionNeedsRecognizedExpression() { return true; }

private:
  bool shouldRemoveInitial(
    LinguisticGraphVertex src, 
    LinguisticGraphVertex tgt, 
    const Automaton::RecognizerMatch& match) const;

  bool shouldRemoveFinal(
    LinguisticGraphVertex src, 
    LinguisticGraphVertex tgt, 
    const Automaton::RecognizerMatch& match) const;

  void addMicrosToMorphoSyntacticData(
     LinguisticAnalysisStructure::MorphoSyntacticData* newMorphData,
     const LinguisticAnalysisStructure::MorphoSyntacticData* oldMorphData,
     const std::set<LinguisticCode>& micros,
     LinguisticAnalysisStructure::LinguisticElement& elem) const;
   
  
     MediaId m_language;
  Common::MediaticData::EntityType m_type;
  FsaStringsPool* m_sp;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  std::set< LinguisticCode > m_microsToKeep;

};


}

}

}

#endif
