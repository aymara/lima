/*
    Copyright 2002-2020 CEA LIST

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
  * @date               Created on  : Mon Aug 04 2003
  * @author             Gael de Chalendar <Gael.de-Chalendar@cea.fr>

  *                     Copyright (c) 2003-2020 CEA LIST
  * @version            $Id$
  */

#ifndef LIMA_LIMASTRING_H
#define LIMA_LIMASTRING_H

#include "common/LimaCommon.h"

#include <functional>

#include <QtGlobal>
#include <QtCore/QString>
#include <QtCore/QHash>

namespace Lima
{
  /** @deprecated LimaChar is an alias for QChar. All its occurences will be
   *  removed enventually. */
  using LimaChar = QChar;
  /** @deprecated LimaString is an alias for QString. All its occurences will be
   *  removed enventually. */
  using LimaString = QString;
  // keep ouput operators to avoid explicit conversion to utf8 at each output
  LIMA_DATA_EXPORT std::ostream& operator<<(std::ostream &os, const QString& s);
} // closing namespace Lima

namespace std
{

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  // With Qt 5.14 and higher, this definition is not necessary anymore
  template<> struct hash<QString> {
    std::size_t operator()(const QString& s) const noexcept {
      return (size_t) qHash(s);
    }
  };
#endif

}


#endif // LIMA_LIMASTRING_H
