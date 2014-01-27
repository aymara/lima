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
 *   Copyright (C) 2007 by CEA LIST / LVIC *
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#ifndef SPECIFICENTITIESHANDLER_H
#define SPECIFICENTITIESHANDLER_H

#include "specificEntity.h"

#include <QIcon>
#include <QXmlDefaultHandler>

class QTextEdit;

class QListWidgetItem;

class SpecificEntitiesHandler : public QXmlDefaultHandler
{

public:
  SpecificEntitiesHandler(
      QList<SpecificEntity*>& entities,
      QTextEdit *textEdit,
      QMap<QString,quint32>& entityNames2Types );

  bool startElement( const
                     QString &namespaceURI, const
                     QString &localName,
                     const
                     QString &qName, const
                     QXmlAttributes &attributes );

  bool endElement( const
                   QString &namespaceURI, const
                   QString &localName,
                   const
                   QString &qName );

  bool characters( const
                   QString &str );

  bool fatalError( const
                   QXmlParseException &exception );

  QString errorString() const
  ;

private:
  // Will later have to change to QTreeWidget to handle multiple groups of
  // properties
  QTextEdit *textEdit;
  QMap<QString,quint32>& m_entityNames2Types;

  QString errorStr;

  bool metSpecificEntitiesTag;

  QString pos, len, type, m_str;
  bool bpos, blen, btype, bstring;

  QList<SpecificEntity*>& m_entities;
};

#endif // SPECIFICENTITIESHANDLER_H
