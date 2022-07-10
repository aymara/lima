// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       posTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Oct 14 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      representation of POS transitions : the transitions are 
 * Parts-Of-Speech
 * 
 ***********************************************************************/

#ifndef POSTRANSITION_H
#define POSTRANSITION_H

#include "AutomatonExport.h"
#include "automatonCommon.h"
#include "transitionUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT PosTransition : public TransitionUnit
{
 public:
  PosTransition(); 
  PosTransition(const Tpos&,
                const Common::PropertyCode::PropertyAccessor* macroAccessor,
                const Common::PropertyCode::PropertyAccessor* microAccessor,
                bool keep=true); 
  PosTransition(const PosTransition&);
  virtual ~PosTransition();
  PosTransition& operator = (const PosTransition&);

  PosTransition* clone() const override;
  PosTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;

  TypeTransition type() const override;

  Tpos pos() const;
  void setPos(Tpos);
  bool comparePos(const LinguisticCode& pos) const;

 private:
  Tpos m_pos;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  
};

/***********************************************************************/
// inline access functions
/***********************************************************************/
inline Tpos PosTransition::pos() const { return m_pos; }
inline void PosTransition::setPos(Tpos s) { m_pos = s; }
inline TypeTransition PosTransition::type() const { return T_POS; }


inline PosTransition* PosTransition::clone() const { 
  return new PosTransition(*this); }
inline PosTransition* PosTransition::create() const { 
  return new PosTransition(); }

} // namespace end
} // namespace end
} // namespace end

#endif
