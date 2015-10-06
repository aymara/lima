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
  
  EntityGroupTransition* clone() const;
  EntityGroupTransition* create() const;

  std::string printValue() const;
  bool operator== (const TransitionUnit&) const;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const;
  
  TypeTransition type() const;
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
