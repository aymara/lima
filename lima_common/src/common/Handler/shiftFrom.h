/*
    Copyright 2021 CEA LIST

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


} // end namespace

#endif
