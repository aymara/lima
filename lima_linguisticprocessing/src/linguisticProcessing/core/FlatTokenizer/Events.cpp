// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 11-JUL-2002
//
// Events is a string of events. Class of characters and other
// events are in the form of printable bytes. Class Events
// holds events which are of the same effect.

#include "Events.h"
#include "CharChart.h"
#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"

#include <string>

using namespace Lima::Common::Misc;

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

Events::Events(std::shared_ptr<CharChart> chart) : std::vector<const CharClass*>(),
               m_chart(chart)
{
}

Events::~Events() {}

void Events::addEventNamed(const LimaString& eventName)
{
    push_back(m_chart->classNamed(eventName));
}

bool Events::isRecognized(const Lima::LimaChar& event) const
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
#endif
  for (uint64_t i = 0; i < size(); i++)
  {
//   LDEBUG << "Events::isRecognized("<<i<<","<<event<<") ; size=" << size();
    const CharClass* eventClass = m_chart->charClass(event);
    while (eventClass != 0)
    {
#ifdef DEBUG_LP 
      LDEBUG << "| | | test event '"<< eventClass->name()
          << "' on '" << ((*this)[i]==0?"":(*this)[i]->name()) <<"'";
#endif
      if ( (*this)[i] == eventClass )
      {
        return true;
      }
      eventClass = eventClass->superClass();
    }
  }
  return false;
}

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima
