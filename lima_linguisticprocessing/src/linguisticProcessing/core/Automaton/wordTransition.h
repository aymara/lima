// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       wordTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Oct 14 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      transitions that are surface form : simple words
 * 
 ***********************************************************************/

#ifndef WORDTRANSITION_H
#define WORDTRANSITION_H

#include "AutomatonExport.h"
#include "automatonCommon.h"
#include "transitionUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT WordTransition : public TransitionUnit
{
 public:
  WordTransition(); 
  WordTransition(Tword s, bool keep=true); 
  WordTransition(const WordTransition&);
  virtual ~WordTransition();
  WordTransition& operator = (const WordTransition&);
  
  WordTransition* clone() const override;
  WordTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;
  
  TypeTransition type() const override;

  Tword word() const;
  LimaString wordString(const FsaStringsPool& sp) const;
  void setWord(Tword s);
  
 private:
  Tword m_word;

};


/***********************************************************************/
// inline access functions
/***********************************************************************/
inline Tword WordTransition::word() const { return m_word; }
inline void WordTransition::setWord(Tword s) { m_word = s; }
inline TypeTransition WordTransition::type() const { return T_WORD; }

inline WordTransition* WordTransition::clone() const { 
  return new WordTransition(*this); }
inline WordTransition* WordTransition::create() const {
  return new WordTransition(); }


} // namespace end
} // namespace end
} // namespace end

#endif
