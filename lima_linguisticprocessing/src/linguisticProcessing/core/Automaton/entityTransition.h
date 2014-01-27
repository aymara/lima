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
  
  EntityTransition* clone() const;
  EntityTransition* create() const;

  std::string printValue() const;
  bool operator== (const TransitionUnit&) const;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const;
  
  TypeTransition type() const;
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
