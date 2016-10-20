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
/************************************************************************
 * @file     positionLengthList.cpp
 * @author   Mesnard Olivier
 * @date
 * @version
 * copyright Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/


#include <ostream>

#include "positionLengthList.h"

using namespace std;

#ifdef ANTINNO_SPECIFIC
// FWI 10/01/2014 : déclarations déplacée dans le namespace Misc
namespace Lima {
namespace Common {
namespace Misc {
#endif

QTextStream& operator << (QTextStream& os,
                           const Lima::Common::Misc::PositionLengthList& p)
{
  if (! p.empty())
  {
    Lima::Common::Misc::PositionLengthList::const_iterator pos=p.begin();
    os << "(" << (*pos).first << "," << (*pos).second << ")";
    pos++;
    while (pos != p.end())
    {
      os << "; (" << (*pos).first << "," << (*pos).second << ")";
      pos++;
    }
  }
  return os;
}

std::ostream& operator << (std::ostream& os,
                           const Lima::Common::Misc::PositionLengthList& p)
{
  if (! p.empty())
  {
    Lima::Common::Misc::PositionLengthList::const_iterator pos=p.begin();
    os << "(" << (*pos).first << "," << (*pos).second << ")";
    pos++;
    while (pos != p.end())
    {
      os << "; (" << (*pos).first << "," << (*pos).second << ")";
      pos++;
    }
  }
  return os;
}

QDebug& operator << (QDebug& os,
                           const Lima::Common::Misc::PositionLengthList& p)
{
  if (! p.empty())
  {
    Lima::Common::Misc::PositionLengthList::const_iterator pos=p.begin();
    os << "(" << (*pos).first << "," << (*pos).second << ")";
    pos++;
    while (pos != p.end())
    {
      os << "; (" << (*pos).first << "," << (*pos).second << ")";
      pos++;
    }
  }
  return os;
}

#ifdef ANTINNO_SPECIFIC
}}}
#endif
