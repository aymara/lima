// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @brief       this file contains the definitions of several constraint
  *              functions for the detection of homosyntagmatic dependency
  *              relations
  * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  * @date        Created on  Thu Nov, 13 2003
  */

#ifndef LIMA_SYNTACTICANALYSIS_HOMOSYNTAGMATICCONSTRAINTS_H
#define LIMA_SYNTACTICANALYSIS_HOMOSYNTAGMATICCONSTRAINTS_H

#include "SyntacticAnalysisExport.h"
#include "SyntacticData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"
#include <iostream>

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

//**********************************************************************
// ids of constraints defined in this file
#define SecondUngovernedById "SecondUngovernedBy"
#define GovernorOfId "GovernorOf"
#define GovernedById "GovernedBy"
#define RemoveOutRelationFromId "RemoveOutRelationFrom"
#define SameNominalChainId "SameNominalChain"
#define SameVerbalChainId "SameVerbalChain"
#define CreateRelationBetweenId "CreateRelationBetween"
#define CreateRelationWithRelatedId "CreateRelationWithRelated"
#define CreateRelationReverseWithRelatedId "CreateRelationReverseWithRelated"
#define CopyRelationsOutOfToId "CopyRelationsOutOfTo"
#define CopyIncomingRelationsToId "CopyIncomingRelationsTo"
#define CreateCompoundTenseId "CreateCompoundTense"
#define CreateEasyCompoundTenseId "CreateEasyCompoundTense"
#define FindRelationFromId "FindRelationFrom"
#define EnforcePropertiesConstraintsId "EnforcePropertiesConstraints"
#define AddRelationInGraphId "AddRelationInGraph"
#define ModifyRelationInGraphId "ModifyRelationInGraph"
#define ClearStoredRelationsId "ClearStoredRelations"

/** @defgroup SAConstraints Syntactic analysis rules constraints (or action constraints) classes */
///@{

/**
 * @brief Abstract constraint function that takes a complement string
 * that represents a relation name (entity type)
 * (used by several constraint functions of this file)
 */
class LIMA_SYNTACTICANALYSIS_EXPORT ConstraintWithRelationComplement : public Automaton::ConstraintFunction
{
public:
  explicit ConstraintWithRelationComplement(MediaId language,
                                   const LimaString& complement=LimaString());
  ~ConstraintWithRelationComplement() {}
protected:
  Common::MediaticData::SyntacticRelationId m_relation;
};


/**
 * @brief This constraint tests if the second argument is not governed by the
 * first one with a relation of the given type.
 *
 * So , if there is a relation a-(X)->b in the graph, SecondUngovernedBy(a,b,X)
 * will return false and true otherwise.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT SecondUngovernedBy : public ConstraintWithRelationComplement
{
public:
  explicit SecondUngovernedBy(MediaId language,
                     const LimaString& complement=LimaString());
  ~SecondUngovernedBy() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
};


/**
 * @brief This constraint tests if its argument is the governor of a relation
 * of the given type.
 *
 * So , if there is a relation a-(X)->b in the graph, GovernorOf(a,X)
 * will return true and false otherwise.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT GovernorOf : public ConstraintWithRelationComplement
{
public:
  explicit GovernorOf(MediaId language,
                const LimaString& complement=LimaString());
  ~GovernorOf() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  AnalysisContent& analysis) const override;

private:
};

/**
 * @brief This constraint tests if its argument is governed by a relation
 * of the given type.
 *
 * So , if there is a relation a-(X)->b in the graph, GovernedBy(b,X)
 * will return true and false otherwise.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT GovernedBy : public ConstraintWithRelationComplement
{
public:
  explicit GovernedBy(MediaId language,
                const LimaString& complement=LimaString());
  ~GovernedBy() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  AnalysisContent& analysis) const override;

private:
};

/**
 * @brief This constraint tests if its two argument vertices have at least one
 * common nominal chain.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT SameNominalChain : public Automaton::ConstraintFunction
{
public:
  explicit SameNominalChain(MediaId language,
                   const LimaString& complement=LimaString());
  ~SameNominalChain() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
};


/**
 * @brief This constraint tests if its two argument vertices have at least one
 * common verbal chain.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT SameVerbalChain : public Automaton::ConstraintFunction
{
public:
  explicit SameVerbalChain(MediaId language,
                  const LimaString& complement=LimaString());
  ~SameVerbalChain() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
};


/**
 * @brief This constraint add the specified relation in the relations
 * buffer.
 *
 * It will be really added later on succes of the rule by the action
 * constraint AddRelationInGraph
 */
class LIMA_SYNTACTICANALYSIS_EXPORT CreateRelationBetween : public ConstraintWithRelationComplement
{
public:
  explicit CreateRelationBetween(MediaId language,
                        const LimaString& complement=LimaString());
  ~CreateRelationBetween() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
};

//**********************************************************************
/**
 * @brief This constraint add in the relations buffer the relations of the given
 * type from v1 to the targets of relations out of v2 of the given types.
 *
 * The complement must be of the form:
 * "rel2|…|reln,rel1"
 * with rel1 the type of the relation to create and rel2, …, reln the types of
 * the relations to follow.
 *
 * Relations added to the buffer will be really added later on succes of the
 * rule by the action constraint AddRelationInGraph
 */
class LIMA_SYNTACTICANALYSIS_EXPORT CreateRelationWithRelated : public Automaton::ConstraintFunction
{
public:
  explicit CreateRelationWithRelated(MediaId language,
                            const LimaString& complement=LimaString());
  ~CreateRelationWithRelated() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

protected:
  std::set<Common::MediaticData::SyntacticRelationId> m_relationsToFollow;
  Common::MediaticData::SyntacticRelationId m_relationToCreate;

  std::vector<LinguisticGraphVertex>
    findRelatedVertices(const LinguisticGraphVertex& v,
                        SyntacticData* syntacticData) const;

};

/**
 * @brief This constraint add in the relations buffer the relations of the given
 * type from the targets of relations out of v2 of the given types to v1.
 *
 * It allows to draw a relation (of type the last element in the complement
 * list) from the target (v1) of the given relations (all except the last in the
 * complement list) to the trigger.
 *
 * The complement must be of the form:
 * "rel2|…|reln,rel1"
 * with rel1 the type of the relation to create and rel2, …, reln the types of
 * the relations to follow.
 *
 * Relations added to the buffer will be really added later on succes of the
 * rule by the action constraint AddRelationInGraph
 */
class LIMA_SYNTACTICANALYSIS_EXPORT CreateRelationReverseWithRelated : public CreateRelationWithRelated
{
public:
  explicit CreateRelationReverseWithRelated(MediaId language,
                                   const LimaString& complement=LimaString());
  ~CreateRelationReverseWithRelated() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
};

/**
 * @brief This constraint removes any relation of the type given by the
 * complement outgoing from the given vertex.
 *
 * So, if there is a relation a-(X)->b in the graph, RemoveOutRelationFrom(a,X)
 * will remove it.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT RemoveOutRelationFrom : public ConstraintWithRelationComplement
{
public:
  explicit RemoveOutRelationFrom(MediaId language,
                const LimaString& complement=LimaString());
  ~RemoveOutRelationFrom() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  AnalysisContent& analysis) const override;

private:
};

/**
 *@brief Copy all relations out of v1 to relations out of v2. Targets and types are kept.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT CopyRelationsOutOfTo : public Automaton::ConstraintFunction
{
public:
  explicit CopyRelationsOutOfTo(MediaId language,
                                   const LimaString& complement=LimaString());
  ~CopyRelationsOutOfTo() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
  QStringList m_relations;
};

/**
 *@brief Copy all relations incoming to v1 to relations incoming to of v2. Targets and types are kept.
 *
 * Used to recopy relations
 *  - pointing to the first member of a coordination (target of COORD1) to the second member
 *    (target of COORD2) or
 *  - pointing to the second member of a coordination (target of COORD2) to the first member
 *    (target of COORD1)
 */
class LIMA_SYNTACTICANALYSIS_EXPORT CopyIncomingRelationsTo : public Automaton::ConstraintFunction
{
public:
  explicit CopyIncomingRelationsTo(MediaId language,
                                   const LimaString& complement=LimaString());
  ~CopyIncomingRelationsTo() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
  QStringList m_relations;
};

/** @brief This constraint creates a aux relation between its two
 * parameters
 *
 * @todo It was originaly supposed to replace the two vertices (auxiliary and
 * participle) by a uniq verb vertex with the good tense. This is currently
 * disabled as there is then problems for indexing, searching and highligting.
 * A solution should be found to allow again the right behavior.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT CreateCompoundTense : public Automaton::ConstraintFunction
{
public:
  explicit CreateCompoundTense(MediaId language,
                      const LimaString& complement=LimaString());
  ~CreateCompoundTense() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
  LinguisticCode m_macro; //< Macro category of the auxiliary
  LinguisticCode m_micro; //< Micro category of the auxiliary
  LinguisticCode m_tense; //< Tense of the auxiliary
  Common::MediaticData::SyntacticRelationId m_tempCompType;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  const Common::PropertyCode::PropertyAccessor* m_timeAccessor;
  const Common::PropertyCode::PropertyAccessor* m_genderAccessor;
  const Common::PropertyCode::PropertyAccessor* m_syntaxAccessor;
  const Common::PropertyCode::PropertyAccessor* m_personAccessor;
  const Common::PropertyCode::PropertyAccessor* m_numberAccessor;
};


/** @brief This constraint was developed specifically for Easy campaign,
 * creating a classic compound tense (aux partpass) instead of the
 * actually used (aux_partpass)
 */
class LIMA_SYNTACTICANALYSIS_EXPORT CreateEasyCompoundTense : public Automaton::ConstraintFunction
{
  public:
    CreateEasyCompoundTense(MediaId language,
                        const LimaString& complement=LimaString());
    ~CreateEasyCompoundTense() {}
    bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

  private:
    LinguisticCode m_macro;
    LinguisticCode m_micro;
    Common::MediaticData::SyntacticRelationId m_tempCompType;
    const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
    const Common::PropertyCode::PropertyAccessor* m_microAccessor;
    const Common::PropertyCode::PropertyAccessor* m_timeAccessor;
    const Common::PropertyCode::PropertyAccessor* m_genderAccessor;
    const Common::PropertyCode::PropertyAccessor* m_syntaxAccessor;
    const Common::PropertyCode::PropertyAccessor* m_personAccessor;
    const Common::PropertyCode::PropertyAccessor* m_numberAccessor;
};



/** @brief This constraint follows all relations from its argument until it
 * finds the relation given as its complement. Return true if the relation is
 * found and false otherwise.
 *
 * This class is used during the specializing of relations like CPL_V into
 * circumpstantials like CC_TEMPS. The found relation will later be used on
 * success by the action constraints ModifyRelationInGraph to do the
 * specialization.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT FindRelationFrom : public ConstraintWithRelationComplement
{
public:
  explicit FindRelationFrom(MediaId language,
                   const LimaString& complement=LimaString());
  ~FindRelationFrom() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  AnalysisContent& analysis) const override;
private:
};

/** @brief This constraint modifies the both arguments DicoWords to keep only
 * properties with compatible values given by the complement.
 *
 * For example, if the complement is "GENDER,NUMBER", the first argument is
 * masculine or feminine and singular and the second argument is masculine and
 * singular or plural, then, after the application of the constraints, both
 * arguments will be only masculine and singular.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT EnforcePropertiesConstraints : public Automaton::ConstraintFunction
{
public:
  explicit EnforcePropertiesConstraints(MediaId language,
                               const LimaString& complement=LimaString());
  ~EnforcePropertiesConstraints() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;

private:
  /** @brief Categories on which agreement must hold */
  std::vector<const Common::PropertyCode::PropertyAccessor*> m_categories;
  MediaId m_language;
};

/** @brief This action constraint does the real job of adding in the graph
 * relations stored by CreateRelationBetween
 */
class LIMA_SYNTACTICANALYSIS_EXPORT AddRelationInGraph : public ConstraintWithRelationComplement
{
public:
  explicit AddRelationInGraph(MediaId language,
                     const LimaString& complement=LimaString());
  ~AddRelationInGraph() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;
  // if no arguments, add relations stored in syntactic data
  bool operator()(AnalysisContent& analysis) const override;

private:
};

/** @brief This action constraint modifies the type of relations previously
 * found by FindRelationFrom. The new type is given by the complement.
 *
 * This class is used during the specializing of relations like CPL_V into
 * circumpstantials like CC_TEMPS.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT ModifyRelationInGraph : public ConstraintWithRelationComplement
{
public:
  explicit ModifyRelationInGraph(MediaId language,
                        const LimaString& complement=LimaString());
  ~ModifyRelationInGraph() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;
  // if no arguments, add relations stored in syntactic data
  bool operator()(AnalysisContent& analysis) const override;

private:
};

/** @brief This action constraint is used if a rule finally fail to remove
 * stored relations that were scheduled for creation.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT ClearStoredRelations : public Automaton::ConstraintFunction
{
public:
  explicit ClearStoredRelations(MediaId language,
                       const LimaString& complement=LimaString());
  ~ClearStoredRelations() {}
  bool operator()(AnalysisContent& analysis) const override;

private:
};

///@}

} // end namespace SyntacticAnalysis
} // end namespace LinguisticProcessing
} // end namespace Lima

#endif // LIMA_SYNTACTICANALYSIS_HOMOSYNTAGMATICCONSTRAINTS_H
