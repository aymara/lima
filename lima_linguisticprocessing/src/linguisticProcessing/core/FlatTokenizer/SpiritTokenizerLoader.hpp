// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef SPIRITTOKENIZERLOADER_HPP
#define SPIRITTOKENIZERLOADER_HPP

#include "FlatTokenizerExport.h"
#include "Automaton.h"

#include <string>

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{
  class State;
  
  class LIMA_FLATTOKENIZER_EXPORT SpiritTokenizerLoader
  {
  public:
    SpiritTokenizerLoader(Automaton& automaton, const std::string& fileName);
  private:
    // accessor with lazy initialization
    State* stateNamed(const LimaString& name);
    Automaton& m_automaton;
  };
  //]
} // FlatTokenizer
} // LinguisticProcessing
} // Lima

#endif // SPIRITTOKENIZERLOADER_HPP
