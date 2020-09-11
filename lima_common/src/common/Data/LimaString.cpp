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
