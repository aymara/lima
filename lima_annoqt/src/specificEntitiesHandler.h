// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
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
                     QXmlAttributes &attributes ) override;

  bool endElement( const
                   QString &namespaceURI, const
                   QString &localName,
                   const
                   QString &qName ) override;

  bool characters( const
                   QString &str ) override;

  bool fatalError( const
                   QXmlParseException &exception ) override;

  QString errorString() const override;

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
