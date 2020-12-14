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
#include "WordSenseUnit.h"
#include "CommonTypedefs.h"

#include <iostream>

namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{
  WordSenseUnit::WordSenseUnit(uint senseId,
                               uint64_t parentLemmaId,
                               Mode mode,
                               std::string& senseTag,
                               std::set<uint64_t>& senseMembersIds,
                               int freq):
     m_parentLemmaId(parentLemmaId),
     m_mode(mode),
     m_senseId(senseId),
     m_senseTag(senseTag),
     m_senseMembersIds(senseMembersIds),
     m_freq(freq)
     {}

  std::ostream& operator << (std::ostream& os, const WordSenseUnit& wsu)
  {
    os <<  wsu.parentLemmaId() << "#"<<wsu.senseId() << "(" << wsu.mode() << "):" << wsu.senseTag() ;
    return os;
  }




}  // namespace WordSenseDisambiguation
}  // namespace LinguisticProcessing
}  // namespace Lima
