// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

LIMA_LPMISC_EXPORT QTextStream& operator << (QTextStream& os, const Lima::Common::Misc::PositionLengthList& p);
LIMA_LPMISC_EXPORT std::ostream& operator << (std::ostream& os, const Lima::Common::Misc::PositionLengthList& p);
LIMA_LPMISC_EXPORT QDebug& operator << (QDebug& os, const Lima::Common::Misc::PositionLengthList& p);

#endif // POSITION_LENGTH_LIST_H
