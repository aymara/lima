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
// jys 24-JUL-2002
//
// TokenizerAutomaton is the main program of TokenizerAutomaton stuff.

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_TOKENIZERAUTOMATON_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_TOKENIZERAUTOMATON_H

#include "FlatTokenizerExport.h"
#include "common/Data/LimaString.h"
#include "Text.h"
#include "Automaton.h"
#include <wchar.h>

#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

#define FLATTOKENIZERAUTOMATON_CLASSID "FlatTokenizerAutomaton"

class LIMA_FLATTOKENIZER_EXPORT TokenizerAutomaton : public AbstractResource
{
  Q_OBJECT

public:
  TokenizerAutomaton();
  virtual ~TokenizerAutomaton();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  inline const std::vector<State*>& states() const {return m_states;};
  inline std::vector<State*>& states() {return m_states;};

  inline const Text& text() const {return *m_text;}
  inline Text& text() {return *m_text;}
  
  const State* stateNamed(const LimaString& name) const;
  State* stateNamed(const LimaString& name);

  inline const CharChart* charChart() const {return _charChart;}
  inline CharChart* charChart() {return _charChart;}
  inline void setCharChart(CharChart* charChart) {_charChart = charChart;}

  private:
  Text* m_text;

  Automaton* _automaton;
  CharChart* _charChart;
  MediaId _language;

  std::vector<State*> m_states;
};

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima

#endif // LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_TOKENIZERAUTOMATON_H
