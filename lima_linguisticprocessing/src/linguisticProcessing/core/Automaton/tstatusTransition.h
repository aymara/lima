// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       tstatusTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan 13 2003
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      transitions that are status of the tokenizer
 * 
 ***********************************************************************/

#ifndef TSTATUSTRANSITION_H
#define TSTATUSTRANSITION_H

#include "AutomatonExport.h"
#include "automatonCommon.h"
#include "transitionUnit.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/TStatus.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT TStatusTransition : public TransitionUnit
{
 public:
  TStatusTransition(); 
  TStatusTransition(LinguisticAnalysisStructure::TStatus s, bool keep=true); 
  //  TStatusTransition(const LimaString& str);
  TStatusTransition(const TStatusTransition&);
  virtual ~TStatusTransition();
  TStatusTransition& operator = (const TStatusTransition&);

  TStatusTransition* clone() const override;
  TStatusTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;

  TypeTransition type() const override;

  const LinguisticAnalysisStructure::TStatus& status() const;
  LinguisticAnalysisStructure::TStatus& status();
  void setStatus(const LinguisticAnalysisStructure::TStatus& s);
  
 private:
  LinguisticAnalysisStructure::TStatus m_status;
  bool compareTStatus(const LinguisticAnalysisStructure::TStatus& t) const;

};


/***********************************************************************/
// inline access functions
/***********************************************************************/
inline const LinguisticAnalysisStructure::TStatus& 
TStatusTransition::status() const { return m_status; }

inline LinguisticAnalysisStructure::TStatus& 
TStatusTransition::status() { return m_status; }

inline void TStatusTransition::
setStatus(const LinguisticAnalysisStructure::TStatus& s) { m_status = s; }

inline TypeTransition TStatusTransition::type() const { return T_TSTATUS; }

inline TStatusTransition* TStatusTransition::clone() const { 
  return new TStatusTransition(*this); }
inline TStatusTransition* TStatusTransition::create() const {
  return new TStatusTransition(); }

} // namespace end
} // namespace end
} // namespace end

#endif
