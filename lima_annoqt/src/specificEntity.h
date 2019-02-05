/*
 *    Copyright 2002-2013 CEA LIST
 * 
 *    This file is part of LIMA.
 * 
 *    LIMA is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    LIMA is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 * 
 *    You should have received a copy of the GNU Affero General Public License
 *    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */
/***************************************************************************
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#ifndef SPECIFICENTITY_H
#define SPECIFICENTITY_H

#include <QObject>

class SpecificEntity : public QObject
{
Q_OBJECT

public:
  SpecificEntity(quint32 pos, quint32 len, quint32 type, const QString& str, QObject* parent = 0);

  SpecificEntity(const SpecificEntity& se);

  SpecificEntity& operator=(const SpecificEntity& se);

  virtual ~SpecificEntity() {}

   inline quint32 position() const {return m_position;}
   inline quint32 length() const {return m_length;}
   inline quint32 type() const {return m_type;}
   inline const QString& string() const {return m_string;}

   inline void setPosition(quint32 pos) {m_position = pos;}
   inline void setLength(quint32 len) {m_length = len;}
   inline void setType(quint32 type) {m_type = type;}
   inline void setString(const QString& str) {m_string =str;}

public Q_SLOTS:
  void slotTriggered();

Q_SIGNALS:
  void triggered(SpecificEntity*);
  
private:

  quint32 m_position;
  quint32 m_length;
  quint32 m_type;
  QString m_string;
};

#endif // SPECIFICENTITIESHANDLER_H
