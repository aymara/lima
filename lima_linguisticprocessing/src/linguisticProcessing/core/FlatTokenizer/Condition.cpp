/*
    Copyright 2002-2019 CEA LIST

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
// Input text is translated into characters class string.
// Characters class are coded using printable character, so,
// they can be maniplated as String.

#include "Condition.h"

#include "Events.h"
#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"

using namespace Lima::Common::Misc;

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

Condition::Condition(const CharChart* chart) :
_before(),
_after(),
_status(chart)
{
}

Condition::~Condition()
{
}

// for run-time usage. Within a context, a condition is
// fulfilled or not.
bool Condition::isFulfilled(const Text& text) const
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "| | | testing if condition is fullfilled with "
      << _before.size() << " events before and "
      << _after.size() << " events after.";
#endif
  try {
    int currentIndex = 0;
    for (uint64_t b = 0; b < _before.size(); b++)
    {
      LimaChar c;
      if (!(((currentIndex == 0) && _before[b].isRecognized(c))||(_before[b].isRecognized(text[--currentIndex]))))
      {
#ifdef DEBUG_LP
        LDEBUG << "| | | before condition is not fullfilled";
#endif
        return false;
      }
    }
    currentIndex = 0;
    for (uint64_t a = 0 ; a < _after.size(); a++)
    {
      if (!(_after[a].isRecognized(text[++currentIndex])))
      {
#ifdef DEBUG_LP
        LDEBUG << "| | | after condition is not fullfilled";
#endif
        return false;
      }
    }
  }
  catch (BoundsErrorException&)
  {
#ifdef DEBUG_LP
    TOKENIZERLOGINIT;
    LDEBUG << "Condition tried to reach out of text bounds";
#endif
    return false;
  }
#ifdef DEBUG_LP
  LDEBUG << "| | | condition is fullfilled";
#endif
  return true;
}

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima
