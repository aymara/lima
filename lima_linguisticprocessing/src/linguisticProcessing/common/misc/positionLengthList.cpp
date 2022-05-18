// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
