// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*************************************************************************
*
* File        : lemmaTransition.cpp
* Author      : Romaric Besan�n (besanconr@zoe.cea.fr)
* Created on  : Wed Nov 13 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
*************************************************************************/

#include "lemmaTransition.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// constructors
/***********************************************************************/
LemmaTransition::LemmaTransition():
TransitionUnit(),
m_lemma(),
m_pos()
{}

LemmaTransition::LemmaTransition(const Tword s,
                                 const Tpos p,
                                 const Common::PropertyCode::PropertyAccessor* macroAccessor,
                                 const Common::PropertyCode::PropertyAccessor* microAccessor,
                                 bool keep):
TransitionUnit(keep),
m_lemma(s),
m_pos(p),
m_macroAccessor(macroAccessor),
m_microAccessor(microAccessor) {
}

LemmaTransition::LemmaTransition(const LemmaTransition& t):
TransitionUnit(t),
m_lemma(t.lemma()),
m_pos(t.partOfSpeech()),
m_macroAccessor(t.m_macroAccessor),
m_microAccessor(t.m_microAccessor) {
//   copyProperties(t);
}

LemmaTransition::~LemmaTransition() {}

LemmaTransition& LemmaTransition::operator = (const LemmaTransition& t) {
  if (this != &t) {
    m_lemma = t.lemma();
    m_pos = t.partOfSpeech();
    m_macroAccessor = t.m_macroAccessor;
    m_microAccessor = t.m_microAccessor;
    copyProperties(t);
  }
  return *this;
}

/***********************************************************************/
// operators ==
/***********************************************************************/
bool LemmaTransition::operator== (const TransitionUnit& tright) const {
  if (type() == tright.type()) {
    const LemmaTransition& other=
      static_cast<const LemmaTransition&>(tright);
    if (m_lemma == other.lemma() && m_pos == other.partOfSpeech()) {
      return compareProperties(tright);
    }
  }
  return false;
}

bool LemmaTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
        const LinguisticGraphVertex& /*vertex*/,
        AnalysisContent& /*analysis*/,
        const LinguisticAnalysisStructure::Token* /*token*/,
        const LinguisticAnalysisStructure::MorphoSyntacticData* data) const
{
  MorphoSyntacticData::const_iterator
    it=data->begin(),
    it_end=data->end();
  for (; it!=it_end; it++) {
    if ( ((*it).lemma == m_lemma) &&
         compareTpos(m_pos,(*it).properties,
                     *m_macroAccessor,
                     *m_microAccessor)) {
      return true;
    }
  }
  return false;
}

/***********************************************************************/
// output
/***********************************************************************/
LimaString LemmaTransition::lemmaString(const FsaStringsPool& sp) const {
  return sp[m_lemma];
}

std::string LemmaTransition::printValue() const {
  ostringstream oss;
/*  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(0);
  LimaString s = sp[(StringsPoolIndex)m_lemma];
  oss << s << "|$" << m_pos;*/
  oss << "lemmaT'" << m_lemma << "'$" << m_pos.toString();
  return oss.str();
}

} // namespace end
} // namespace end
} // namespace end
