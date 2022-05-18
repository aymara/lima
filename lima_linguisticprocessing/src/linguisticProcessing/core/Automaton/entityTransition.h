// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EntityTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Jul 28 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     Automaton
 * 
 * @brief      transitions that are previously recognized entities
 * 
 ***********************************************************************/

#ifndef ENTITYTRANSITION_H
#define ENTITYTRANSITION_H

#include "AutomatonExport.h"
#include "automatonCommon.h"
#include "transitionUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT EntityTransition : public TransitionUnit
{
 public:
  EntityTransition(); 
  EntityTransition(Common::MediaticData::EntityType, bool keep=true); 
  virtual ~EntityTransition();
  
  EntityTransition* clone() const override;
  EntityTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;
  
  TypeTransition type() const override;
  Common::MediaticData::EntityType entityType() const { return m_entityType; }
  void setEntityType(Common::MediaticData::EntityType type) { m_entityType=type; }
  
 private:
  Common::MediaticData::EntityType m_entityType;
  static LimaString m_entityAnnotation;
};


/***********************************************************************/
// inline access functions
/***********************************************************************/
inline TypeTransition EntityTransition::type() const { return T_ENTITY; }

inline EntityTransition* EntityTransition::clone() const { 
  return new EntityTransition(*this); }
inline EntityTransition* EntityTransition::create() const {
  return new EntityTransition(); }


} // namespace end
} // namespace end
} // namespace end

#endif
