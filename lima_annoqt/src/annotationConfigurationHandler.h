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
