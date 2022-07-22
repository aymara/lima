// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       lemmaTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Nov 13 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      transitions that are normalized forms : lemma
 * 
 * 
 ***********************************************************************/

#ifndef LEMMATRANSITION_H
#define LEMMATRANSITION_H

#include "AutomatonExport.h"
#include "automatonCommon.h"
#include "transitionUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT LemmaTransition : public TransitionUnit
{
 public:
  LemmaTransition(); 
  LemmaTransition(const Tword, const Tpos, 
                  const Common::PropertyCode::PropertyAccessor* macroAccessor,
                  const Common::PropertyCode::PropertyAccessor* microAccessor,
                  bool keep=true);
  LemmaTransition(const LemmaTransition&);
  virtual ~LemmaTransition();
  LemmaTransition& operator = (const LemmaTransition&);

  LemmaTransition* clone() const override;
  LemmaTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;

  TypeTransition type() const override;

  Tword lemma() const;
  LimaString lemmaString(const FsaStringsPool& sp) const;
  void setLemma(const Tword&);
  Tpos partOfSpeech() const;
  void setPartOfSpeech(const Tpos&);
  
 private:
  Tword m_lemma;
  Tpos m_pos;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  
};


/***********************************************************************/
// inline access functions
/***********************************************************************/
inline Tword LemmaTransition::lemma() const { return m_lemma; }
inline void LemmaTransition::setLemma(const Tword& s) { m_lemma = s; }
inline Tpos LemmaTransition::partOfSpeech() const { return m_pos; }
inline void LemmaTransition::setPartOfSpeech(const Tpos& p) { m_pos = p; }
inline TypeTransition LemmaTransition::type() const { return T_LEMMA; }

inline LemmaTransition* LemmaTransition::clone() const { 
  return new LemmaTransition(*this); } 
inline LemmaTransition* LemmaTransition::create() const {
  return new LemmaTransition(); }

} // namespace end
} // namespace end
} // namespace end

#endif
