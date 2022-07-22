// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#ifndef ANNOTATIONCONFIGURATIONHANDLER_H
#define ANNOTATIONCONFIGURATIONHANDLER_H

#include <QIcon>
#include <QXmlDefaultHandler>

class QListWidget;

class QListWidgetItem;

class AnnotationConfigurationHandler : public QXmlDefaultHandler
{

public:
  AnnotationConfigurationHandler( QListWidget *listWidget, QList<QColor>* colors, QMap<QString,QString>* colorNames2EntityTypes, QList<QString>* recursiveEntityTypes, bool isEditor = false );

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
  QListWidget *listWidget;

  QList<QColor>* colors;

  QMap<QString,QString>* colorNames2EntityTypes;

  QString errorStr;

  bool metAnnotationConfigurationTag;

  QList<QString>* m_recursiveEntityTypes;

  bool m_isEditor;
};

#endif // ANNOTATIONCONFIGURATIONHANDLER_H
