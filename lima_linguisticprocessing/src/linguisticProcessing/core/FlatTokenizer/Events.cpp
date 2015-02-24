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

Events::Events(const CharChart* chart) : std::vector<const CharClass*>(),
               m_chart(chart)
{
}

Events::~Events() {}

void Events::addEventNamed(const LimaString& eventName)
{
    push_back(m_chart->classNamed(eventName));
}

bool Events::isRecognized(const LimaChar event) const
{
  TOKENIZERLOGINIT;
//   LDEBUG << "Events("<<i<<","<<event<<") ; size=" << size();
  for (uint64_t i = 0; i < size(); i++)
  {
    const CharClass* eventClass = m_chart->charClass(event);
    while (eventClass != 0)
    {
      LDEBUG << "| | | test event '"<< limastring2utf8stdstring(eventClass->name())
          << "' on '" << ((*this)[i]==0?"":limastring2utf8stdstring((*this)[i]->name())) <<"'";
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
