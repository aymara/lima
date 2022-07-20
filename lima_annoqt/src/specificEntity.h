// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
