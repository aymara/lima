// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
