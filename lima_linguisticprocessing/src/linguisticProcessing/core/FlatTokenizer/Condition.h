// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 14-JUL-2002
//
// Condition is the class which groups various static
// conditions associated with a Transition. There are 3 types
// of static conditions :
// o value of characters before the current one in the string,
// o value of characters after the current one in the string,
// o value of inner automaton return status.

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_Condition_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_Condition_H

#include "FlatTokenizerExport.h"
#include "Text.h"
#include "ReturnStatus.h"
#include "Events.h"
// #include "linguisticProcessing/core/Tokenizer/ParseMarkup.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

class LIMA_FLATTOKENIZER_EXPORT Condition {

public:
  explicit Condition(std::shared_ptr<CharChart> chart);
  virtual ~Condition();

  // for run-time usage. Within a context, a condition is
  // fulfilled or not. Return true if fulfilled, false otherwise
  bool isFulfilled(const Text& text) const;

  inline const std::vector<Events>& before() const {return _before;}
  inline std::vector<Events>& before() {return _before;}

  inline const std::vector<Events>& after() const {return _after;}
  inline std::vector<Events>& after() {return _after;}

  inline const Events& status() const {return _status;}
  inline Events& status() {return _status;}

protected:

private:
  std::vector<Events> _before;
  std::vector<Events> _after;
  Events _status;
};

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima


#endif  
