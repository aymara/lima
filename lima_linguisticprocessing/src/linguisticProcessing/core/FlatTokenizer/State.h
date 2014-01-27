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

// NAUTITIA
//
// jys 22-JUL-2002
//
// State is the recovery point for automatons. State is a
// collection of Transition and eventually inner automatons.
// Inner automatons are called once when state is entered at
// first time. Inner automatons are called before checking
// transition. Inner automatons can return a status which
// ccan be used by transitions.

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_State_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_State_H

#include "FlatTokenizerExport.h"
#include "Text.h"
#include "Automaton.h"
#include "ReturnStatus.h"
// #include "linguisticProcessing/core/Tokenizer/ParseMarkup.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{


class Transition;

class LIMA_FLATTOKENIZER_EXPORT State
{

public:
  explicit State(const Lima::LimaString& name, Automaton& automaton);
  virtual ~State();

  // Run-Time. Returns true if a transition was founs open
  const State* run(Text& text) const;

  inline const LimaString& name() const {return m_name;}

  inline std::list< Transition* >& transitions() {return m_transitions;}
  inline const std::list< Transition* >& transitions() const {return m_transitions;}

  inline const Automaton& automaton() const {return m_automaton;}

protected:

private:
  Automaton& m_automaton;
  std::list< Transition* > m_transitions;
  LimaString m_name;
};

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima

#endif
