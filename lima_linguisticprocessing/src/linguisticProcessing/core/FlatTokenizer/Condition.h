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
  explicit Condition(const CharChart* chart);
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
