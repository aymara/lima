// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        corefSolver.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr)

  *              Copyright (c) 2004 by CEA
  * @date        Created on Dec, 1 2004
  * @version     $Id$
  *
  */

#ifndef LIMA_COREFSOLVING_COREFSOLVER_H
#define LIMA_COREFSOLVING_COREFSOLVER_H

#include "CorefSolvingExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "coreferentAnnotation.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"

#include <string>


namespace Lima
{
namespace Common {
namespace AnnotationGraphs {
    class AnnotationData;
}
}
namespace LinguisticProcessing
{
namespace SyntacticAnalysis {
class SyntacticData;
}
namespace LinguisticAnalysisStructure {
class AnalysisGraph;
}
namespace Coreferences
{
class CoreferentAnnotation;

#define COREFSOLVINGPU_CLASSID "CoreferencesSolving"

typedef std::set<CoreferentAnnotation*> Vertices;
typedef std::map<CoreferentAnnotation*,std::set<CoreferentAnnotation*> > VerticesRelation;
typedef std::map< CoreferentAnnotation*,std::map <CoreferentAnnotation*,float> > WeightedVerticesRelation;



/** @brief A  process unit implementing the Lappin & Leass algorithm for coreference solving
 * The process unit configuration parameters are (@TODO add signification, default value and whether they are mandatory or optional):
 * - scope
 * - threshold
 * - Resolve Definites
 * - Resolve non third person pronouns
 * - LexicalAnaphora
 * - UndefinitePronouns
 * - PossessivePronouns
 * - PrepRelation
 * - PleonasticRelation
 * - DefiniteRelation
 * - SubjectRelation
 * - AttributeRelation
 * - CODRelation
 * - COIRelation
 * - AdjunctRelation
 * - AgentRelation
 * - NPDeterminerRelation
 * - SalienceFactors
 * - SlotValues
 */
class LIMA_COREFSOLVING_EXPORT CorefSolver : public MediaProcessUnit
{
public:
  CorefSolver();

  virtual ~CorefSolver() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;


  MediaId language() const;
  uint64_t scope() const;
  uint64_t threshold() const;
  bool resolveDefinites() const;
  bool resolveN3PPronouns() const;
  std::set< LinguisticCode > inNpCategs() const;
  std::set< LinguisticCode > definiteCategs() const;
  LinguisticCode L_PRON() const;
  LinguisticCode L_VERB() const;
  LinguisticCode L_PREP() const;
  std::map<std::string,LinguisticCode> tagLocalDef() const;
  std::map< std::string,std::deque<std::string> > relLocalDef() const;
  std::set<LinguisticCode> reflexiveReciprocal() const;
  std::set<LinguisticCode> undefPronouns() const;
  std::set<LinguisticCode> possPronouns() const;
  const Common::PropertyCode::PropertyAccessor* macroAccessor() const;
  const Common::PropertyCode::PropertyAccessor* microAccessor() const;
  const Common::PropertyCode::PropertyAccessor* genderAccessor() const;
  const Common::PropertyCode::PropertyAccessor* personAccessor() const;
  const Common::PropertyCode::PropertyAccessor* numberAccessor() const;
  const std::map<std::string,float> salienceWeights() const;
  const std::map<std::string, int> slotValues() const;


private:
  void initSyntacticFilter(
    AnalysisContent& ac,
    LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    LinguisticProcessing::SyntacticAnalysis::SyntacticData* syntacticData,
    CoreferentAnnotation::Vertices* npAnaphora,
    std::deque<CoreferentAnnotation::Vertices>* npCandidates,
    CoreferentAnnotation::VerticesRelation* roBinding) const;

  void bindingLexicalAnaphora(
    AnalysisContent& ac,
    LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    LinguisticProcessing::SyntacticAnalysis::SyntacticData* syntacticData,
    CoreferentAnnotation::Vertices* npAnaphora,
    std::deque<CoreferentAnnotation::Vertices>* npCandidates,
    CoreferentAnnotation::WeightedVerticesRelation* pBinding) const;

  void bindingPotentialCandidates(
    //Common::AnnotationGraphs::AnnotationData* ad,
    LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    //  LinguisticProcessing::SyntacticAnalysis::SyntacticData* syntacticData,
    CoreferentAnnotation::Vertices* npAnaphora,
    std::deque<CoreferentAnnotation::Vertices>* npCandidates,
    CoreferentAnnotation::WeightedVerticesRelation* pBinding) const;

  void getBest(
    const SyntacticAnalysis::SyntacticData* sd,
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    WeightedVerticesRelation* binding,
//     std::vector<std::pair<CoreferentAnnotation, CoreferentAnnotation> >* results,
    std::deque<CoreferentAnnotation::Vertices>* npCandidates,
    Common::AnnotationGraphs::AnnotationData* ad) const;

  void applyEquivalentClassFilter(
    const LinguisticGraph* graph,
    std::deque<CoreferentAnnotation::Vertices>* npCandidates) const;

  void adjustSaliences(
    LinguisticProcessing::SyntacticAnalysis::SyntacticData* sd,
    std::deque<Vertices>* npCandidates,
    CoreferentAnnotation::WeightedVerticesRelation* pBinding,
    LinguisticGraphVertex endSentence,
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    AnalysisContent& ac) const;

  void applyThresholdFilter(
    CoreferentAnnotation::WeightedVerticesRelation* pBinding) const;

  void applyMorphoSyntacticFilter(
    CoreferentAnnotation::WeightedVerticesRelation* pBinding,
    CoreferentAnnotation::VerticesRelation* roBinding) const;
    void applyCircularFilter(WeightedVerticesRelation* pBinding) const;


protected:
  MediaId m_language;
  uint64_t m_scope;
  uint64_t m_threshold;
  bool m_resolveDefinites;
  bool m_resolveN3PPronouns;
  std::set< LinguisticCode > m_inNpCategs;
  std::set< LinguisticCode > m_definiteCategs;
  std::map<std::string,LinguisticCode> m_tagLocalDef;
  std::map< std::string,std::deque<std::string> > m_relLocalDef;
  std::set<LinguisticCode> m_reflexiveReciprocal;
  std::set<LinguisticCode> m_undefPronouns;
  std::set<LinguisticCode> m_possPronouns;
  LinguisticCode m_conjCoord;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
 // const Common::PropertyCode::PropertyManager& m_microManager;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  const Common::PropertyCode::PropertyAccessor* m_genderAccessor;
  const Common::PropertyCode::PropertyAccessor* m_personAccessor;
  const Common::PropertyCode::PropertyAccessor* m_numberAccessor;
  std::map<std::string,float> m_salienceWeights;
  std::map<std::string, int> m_slotValues;
};

inline MediaId CorefSolver::language() const
{
  return m_language;
}
inline uint64_t CorefSolver::scope() const
{
  return m_scope;
}
inline uint64_t CorefSolver::threshold() const
{
  return m_threshold;
}
inline bool CorefSolver::resolveDefinites() const
{
  return m_resolveDefinites;
}
inline bool CorefSolver::resolveN3PPronouns() const
{
  return m_resolveN3PPronouns;
}
inline std::set< LinguisticCode > CorefSolver::inNpCategs() const
{
  return m_inNpCategs;
}
inline std::set< LinguisticCode > CorefSolver::definiteCategs() const
{
  return m_definiteCategs;
}
inline LinguisticCode CorefSolver::L_PRON() const
{
  return (*m_tagLocalDef.find("PRON")).second;
}
inline LinguisticCode CorefSolver::L_VERB() const
{
  return (*m_tagLocalDef.find("V")).second;
}
inline LinguisticCode CorefSolver::L_PREP() const
{
  return (*m_tagLocalDef.find("PREP")).second;
}
inline std::map<std::string,LinguisticCode> CorefSolver::tagLocalDef() const
{
  return m_tagLocalDef;
}
inline std::map< std::string,std::deque<std::string> > CorefSolver::relLocalDef() const
{
  return m_relLocalDef;
}
inline std::set<LinguisticCode> CorefSolver::reflexiveReciprocal() const
{
  return m_reflexiveReciprocal;
}
inline std::set<LinguisticCode> CorefSolver::undefPronouns() const
{
  return m_undefPronouns;
}
inline std::set<LinguisticCode> CorefSolver::possPronouns() const
{
  return m_possPronouns;
}
inline const Common::PropertyCode::PropertyAccessor* CorefSolver::macroAccessor() const
{
  return m_macroAccessor;
}
inline
  const Common::PropertyCode::PropertyAccessor* CorefSolver::microAccessor() const
{
  return m_microAccessor;
}
inline
  const Common::PropertyCode::PropertyAccessor* CorefSolver::genderAccessor() const
{
  return m_genderAccessor;
}
inline
  const Common::PropertyCode::PropertyAccessor* CorefSolver::personAccessor() const
{
  return m_personAccessor;
}
inline
  const Common::PropertyCode::PropertyAccessor* CorefSolver::numberAccessor() const
{
  return m_numberAccessor;
}
inline
  const std::map<std::string,float> CorefSolver::salienceWeights() const
{
  return m_salienceWeights;
}
  const std::map<std::string,int> CorefSolver::slotValues() const
{
  return m_slotValues;
}



} // closing namespace Coreferences
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_COREFSOLVING_COREFSOLVER_H
