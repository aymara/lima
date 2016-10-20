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
 * @file     positionLengthList.h
 * @author   Mesnard Olivier
 * @date     
 * @version
 * copyright Copyright (C) 2003 by CEA LIST
 *
 * @brief
 *
 ***********************************************************************/

#ifndef POSITION_LENGTH_LIST_H
#define POSITION_LENGTH_LIST_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <vector>
#include <iostream>

namespace Lima {
namespace Common {
namespace Misc {

  BOOST_STRONG_TYPEDEF(uint64_t, Position)
  BOOST_STRONG_TYPEDEF(uint64_t, Length)
  typedef std::vector<std::pair<Position, Length> > PositionLengthList;

} // namespace Misc
} // namespace Common
} // namespace Lima
namespace Lima {
namespace Common {
namespace Misc {
LIMA_LPMISC_EXPORT QTextStream& operator << (QTextStream& os, const PositionLengthList& p);
LIMA_LPMISC_EXPORT std::ostream& operator << (std::ostream& os, const PositionLengthList& p);
LIMA_LPMISC_EXPORT QDebug& operator << (QDebug& os, const PositionLengthList& p);

} // namespace Misc
} // namespace Common
} // namespace Lima

#endif // POSITION_LENGTH_LIST_H
