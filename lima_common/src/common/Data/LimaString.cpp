// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @file               LimaString.cpp
  * @date               Created on  : Thu Oct 9, 2003
  * @author             Gael de Chalendar <Gael.de-Chalendar@cea.fr>

  *                     Copyright (c) 2003-2012 by CEA LIST
  * @version            $Id$
  */

#include "LimaString.h"


namespace Lima {

  std::ostream& operator<<(std::ostream &os, const QString& s)
  {
    os << s.toUtf8().data();
    return os;
  }


} // closing namespace Lima
