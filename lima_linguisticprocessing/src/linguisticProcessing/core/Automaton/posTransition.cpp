// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*************************************************************************
*
* File        : posTransition.cpp
* Author      : Romaric Besan�n (besanconr@zoe.cea.fr)
* Created on  : Mon Oct 14 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
*************************************************************************/


#include "posTransition.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// constructors
/***********************************************************************/
PosTransition::PosTransition():TransitionUnit(),m_pos() {}
PosTransition::PosTransition(const Tpos& p,
                              const Common::PropertyCode::PropertyAccessor* macroAccessor,
                              const Common::PropertyCode::PropertyAccessor* microAccessor,
                              bool keep):
  TransitionUnit(keep),
  m_pos(p),
  m_macroAccessor(macroAccessor),
  m_microAccessor(microAccessor) {
}
PosTransition::PosTransition(const PosTransition& t):
  TransitionUnit(t),
  m_pos(t.pos()),
  m_macroAccessor(t.m_macroAccessor),
  m_microAccessor(t.m_microAccessor) {
//   copyProperties(t);
}

/***********************************************************************/
// destructor
/***********************************************************************/
PosTransition::~PosTransition() {}

/***********************************************************************/
// copy
/***********************************************************************/
PosTransition& PosTransition::operator = (const PosTransition& t) {
  if (this != &t) {
    m_pos = t.pos();
    m_macroAccessor = t.m_macroAccessor;
    m_microAccessor = t.m_microAccessor;
    copyProperties(t);
  }
  return *this;
}

std::string PosTransition::printValue() const {
  ostringstream oss;
  oss << "posT_$" << m_pos.toString();
  return oss.str();
}


/***********************************************************************/
// comparison tests
/***********************************************************************/
bool PosTransition::operator== (const TransitionUnit& tright) const {
  if ( (type() == tright.type())
       &&(m_pos == static_cast<const PosTransition&>(tright).pos())
       ) {
    return compareProperties(tright);
  }
  else {
    return false;
  }
}

//**********************************************************************
bool PosTransition::
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
    if (compareTpos(m_pos,(*it).properties,
                    *m_macroAccessor,
                    *m_microAccessor)) {
      return true;
    }
  }
  return false;
}

bool PosTransition::
comparePos(const LinguisticCode& pos) const
{
  return compareTpos(m_pos,pos,
                     *m_macroAccessor,
                     *m_microAccessor);
}

} // namespace end
} // namespace end
} // namespace end
