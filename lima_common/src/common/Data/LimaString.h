// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
