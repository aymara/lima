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

  LemmaTransition* clone() const;
  LemmaTransition* create() const;

  std::string printValue() const;
  bool operator== (const TransitionUnit&) const;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const;

  TypeTransition type() const;

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
