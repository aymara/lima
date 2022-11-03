// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EntityGroupTransition.h
 * @author     Olivier Mesnard (olivier.mesnard@cea.fr)
 * @date       Mon oct 5 2015
 * copyright   (c) 2006-2015 by CEA
 * Project     Automaton
 * 
 * @brief      transitions that are previously recognized entities
 * 
 ***********************************************************************/

#ifndef ENTITYGROUPTRANSITION_H
#define ENTITYGROUPTRANSITION_H

#include "AutomatonExport.h"
#include "automatonCommon.h"
#include "transitionUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT EntityGroupTransition : public TransitionUnit
{
 public:
  EntityGroupTransition(); 
  EntityGroupTransition(Common::MediaticData::EntityGroupId, bool keep=true); 
  virtual ~EntityGroupTransition();
  
  EntityGroupTransition* clone() const override;
  EntityGroupTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;
  
  TypeTransition type() const override;
  Common::MediaticData::EntityGroupId entityGroupId() const { return m_entityGroupId; }
  void setEntityGroupId(Common::MediaticData::EntityGroupId groupId) { m_entityGroupId=groupId; }
  
 private:
  Common::MediaticData::EntityGroupId m_entityGroupId;
  static LimaString m_entityAnnotation;
};


/***********************************************************************/
// inline access functions
/***********************************************************************/
inline TypeTransition EntityGroupTransition::type() const { return T_ENTITY_GROUP; }

inline EntityGroupTransition* EntityGroupTransition::clone() const { 
  return new EntityGroupTransition(*this); }
inline EntityGroupTransition* EntityGroupTransition::create() const {
  return new EntityGroupTransition(); }


} // namespace end
} // namespace end
} // namespace end

#endif
