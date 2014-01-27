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

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_AUTOMATON_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_AUTOMATON_H

#include "FlatTokenizerExport.h"
#include "common/Data/LimaString.h"
#include "Text.h"
#include "ReturnStatus.h"


namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

class State;

#define FLATAUTOMATON_CLASSID "FlatAutomaton"

class LIMA_FLATTOKENIZER_EXPORT Automaton : public AbstractResource
{

public:
  Automaton();
  virtual ~Automaton();

  // just define it for compatibility with AbstractResource
  void init(
            Common::XMLConfigurationFiles::GroupConfigurationStructure&,
            Manager*)
      ;


  // Run-time. Returns true if a Transition was found open
  const State* run(Text& text, const State* state = 0) const;

  void loadFromFile(const std::string& fileName);

  inline const std::vector<State*>& states() const {return m_states;};
  inline std::vector<State*>& states() {return m_states;};

  const State* stateNamed(const LimaString& name) const;
  State* stateNamed(const LimaString& name);

  inline const CharChart* charChart() const {return m_charChart;}
  inline void setCharChart(const CharChart* cc) {m_charChart = cc;}
  
  inline void setStartState(const State* s) {m_startState = s;}
private:
  const State* m_startState;
  std::vector<State*> m_states;
  const CharChart* m_charChart;
};

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima


#endif
