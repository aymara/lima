// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#include "specificEntity.h"

#include <QDebug>

SpecificEntity::SpecificEntity(quint32 pos, quint32 len, quint32 type, const QString& str, QObject* parent) :
  QObject(parent),
  m_position(pos),
  m_length(len),
  m_type(type),
  m_string(str)
{
}

SpecificEntity::SpecificEntity(const SpecificEntity& se) :
  QObject(),
  m_position(se.m_position),
  m_length(se.m_length),
  m_type(se.m_type),
  m_string(se.m_string)
{
}

SpecificEntity& SpecificEntity::operator=(const SpecificEntity& se)
{
  m_position = se.m_position;
  m_length = se.m_length;
  m_type = se.m_type;
  m_string = se.m_string;
  return *this;
}

void SpecificEntity::slotTriggered()
{
  qDebug() << "SpecificEntity::slotTriggered";
  Q_EMIT triggered( this );
}

