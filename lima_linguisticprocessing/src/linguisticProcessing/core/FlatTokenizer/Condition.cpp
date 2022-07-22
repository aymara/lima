// Copyright 2002-2019 CEA LIST
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
