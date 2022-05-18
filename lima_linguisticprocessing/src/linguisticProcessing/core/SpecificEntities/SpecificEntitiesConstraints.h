// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#define isAlphaPossessiveId "isAlphaPossessive"
#define isASpecificEntityId "isASpecificEntity"
#define CreateSpecificEntityId "CreateSpecificEntity"
#define SetEntityFeatureId "SetEntityFeature"
#define AddEntityFeatureAsEntityId "AddEntityFeatureAsEntity"
#define AddEntityFeatureId "AddEntityFeature"
#define AppendEntityFeatureId "AppendEntityFeature"
#define ClearEntityFeaturesId "ClearEntityFeatures"
#define NormalizeEntityId "NormalizeEntity"

/**
@author Benoit Mathieu
*/
class LIMA_SPECIFICENTITIES_EXPORT isAlphaPossessive : public Automaton::ConstraintFunction
{
public:
  isAlphaPossessive(MediaId language,
                    const LimaString& complement=LimaString());
  ~isAlphaPossessive() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v,
                  AnalysisContent& analysis) const override;
};

class LIMA_SPECIFICENTITIES_EXPORT isASpecificEntity : public Automaton::ConstraintFunction
{
public:
  isASpecificEntity(MediaId language,
                    const LimaString& complement=LimaString());
  ~isASpecificEntity() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v,
                  AnalysisContent& analysis) const override;

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
                  AnalysisContent& analysis) const override;

  bool actionNeedsRecognizedExpression() override { return true; }

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

/** 
 * @brief This action set the value of a feature for an entity during the recognition 
 * of the entity (i.e. during the rule matching process). 
 * If a value already exists for the feaure, the new value overwrite the previous one.
 * Unary operator: associate the given 
 * vertex to the entity feature specified in the complement. Binary operator: associate the string 
 * delimited by the two vertices to the entity feature specified in the complement.
 *
 */
class LIMA_SPECIFICENTITIES_EXPORT SetEntityFeature : public Automaton::ConstraintFunction
{
public:
  SetEntityFeature(MediaId language,
                   const LimaString& complement=LimaString());
  ~SetEntityFeature() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                          const LinguisticGraphVertex& vertex,
                          AnalysisContent& analysis) const override;
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                          const LinguisticGraphVertex& v1,
                          const LinguisticGraphVertex& v2,
                          AnalysisContent& analysis) const override;

private:
  std::string m_featureName;
  Common::MediaticData::EntityType m_type;
  QVariant::Type m_featureType;
};

/** 
 * @brief This action add a vertex as an embeded entity
 * of the entity (i.e. during the rule matching process). 
 *
 */
class LIMA_SPECIFICENTITIES_EXPORT AddEntityFeatureAsEntity : public Automaton::ConstraintFunction
{
public:
  AddEntityFeatureAsEntity(MediaId language,
                   const LimaString& complement=LimaString());
  ~AddEntityFeatureAsEntity() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                          const LinguisticGraphVertex& vertex,
                          AnalysisContent& analysis) const override;

private:
  std::string m_featureName;
  Common::MediaticData::EntityType m_type;
};

/** 
 * @brief This action set the value of a feature for an entity during the recognition 
 * of the entity (i.e. during the rule matching process). 
 * If a value already exists for the feaure, the new value overwrite the previous one.
 * Unary operator: associate the given 
 * vertex to the entity feature specified in the complement. Binary operator: associate the string 
 * delimited by the two vertices to the entity feature specified in the complement.
 *
 */
class LIMA_SPECIFICENTITIES_EXPORT AddEntityFeature : public Automaton::ConstraintFunction
{
public:
  AddEntityFeature(MediaId language,
                   const LimaString& complement=LimaString());
  ~AddEntityFeature() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                          const LinguisticGraphVertex& vertex,
                          AnalysisContent& analysis) const override;
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                          const LinguisticGraphVertex& v1,
                          const LinguisticGraphVertex& v2,
                          AnalysisContent& analysis) const override;

private:
  std::string m_featureName;
  Common::MediaticData::EntityType m_type;
  QVariant::Type m_featureType;
};

/** 
 * @brief This action appends a value to the previously set value of a feature for an entity during 
 * the recognition  of the entity (i.e. during the rule matching process).
 * Appends performs a concatenatipon when the type of the property is a string.
 * Appends performs an adition when the type of the property is an int.
 * Unary operator: associate the given vertex to the entity feature specified in the complement. Binary operator: associate the string 
 * delimited by the two vertices to the entity feature specified in the complement.
 *
 */
class LIMA_SPECIFICENTITIES_EXPORT AppendEntityFeature : public Automaton::ConstraintFunction
{
public:
  AppendEntityFeature(MediaId language,
                   const LimaString& complement=LimaString());
  ~AppendEntityFeature() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                          const LinguisticGraphVertex& vertex,
                          AnalysisContent& analysis) const override;
  uint64_t minPos( const uint64_t pos1, const uint64_t pos2 )const;
  uint64_t maxPos( const uint64_t pos1, const uint64_t pos2 )const;

private:
  std::string m_featureName;
  Common::MediaticData::EntityType m_type;
  QVariant::Type m_featureType;
};

/** 
 * @brief This action clears features stored for one entity. 
 * This action needs to be called if rule matching fails (otherwise, features 
 * are accumulated from different matching tests)
 *
 */
class LIMA_SPECIFICENTITIES_EXPORT ClearEntityFeatures : public Automaton::ConstraintFunction
{
public:
  ClearEntityFeatures(MediaId language,
                   const LimaString& complement=LimaString());
  ~ClearEntityFeatures() {}
  bool operator()(AnalysisContent& analysis) const override;

private:

};

/** 
 * @brief This action performs the normalization of an entity according to stored features. 
 * This action uses the features stored by the setEntityFeature() function to perform the 
 * normalization of the entity. 
 *
 */
class LIMA_SPECIFICENTITIES_EXPORT NormalizeEntity : public Automaton::ConstraintFunction
{
public:
  NormalizeEntity(MediaId language,
                   const LimaString& complement=LimaString());
  ~NormalizeEntity() {}
  bool operator()(Automaton::RecognizerMatch& match,
                  AnalysisContent& analysis) const override;

  bool actionNeedsRecognizedExpression() override { return true; }

private:

};

  
}
}
}

#endif
