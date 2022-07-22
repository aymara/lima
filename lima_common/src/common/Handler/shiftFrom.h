// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     Gaël de Chalendar <gael.de-chalendar@cea.fr>
 * @date       Wed Dec 15 2021
 ***********************************************************************/

#ifndef SHIFTFROM_H
#define SHIFTFROM_H

#include "common/LimaCommon.h"

#include <QMap>

#include <memory>


namespace Lima {

class ShiftFromPrivate;
class LIMA_DATAHANDLER_EXPORT ShiftFrom:
    public QMap<int, std::tuple<int, QString, QString> >
{
  friend LIMA_DATAHANDLER_EXPORT QDebug& operator << (QDebug&, const ShiftFrom&)
  ;
public:
  ShiftFrom(const QString& xml);
  virtual ~ShiftFrom();
  const QString& xml();
  const QString& xml_noent();
  int correct_offset(int offset, int indexOfToken) const;
  QString rebuild_text(const QString& input, int offset) const;


private:
  ShiftFrom(const ShiftFrom&) = delete;
  ShiftFrom& operator=(const ShiftFrom&) = delete;

  ShiftFromPrivate* m_d;
};

LIMA_DATAHANDLER_EXPORT QDebug& operator<<(QDebug& os, const ShiftFrom& sf);

} // end namespace

#endif
