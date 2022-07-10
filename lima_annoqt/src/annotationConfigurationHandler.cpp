// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#include "annotationConfigurationHandler.h"

#include <QListWidget>
#include <QMessageBox>

AnnotationConfigurationHandler::AnnotationConfigurationHandler(
  QListWidget *listWidget, QList<QColor>* colors,
  QMap<QString,QString>* colorNames2EntityTypes, QList<QString>* recursiveEntityTypes,
  bool isEditor)
    : listWidget( listWidget ), colors(colors), colorNames2EntityTypes(colorNames2EntityTypes), m_recursiveEntityTypes(recursiveEntityTypes), m_isEditor(isEditor)
{
  metAnnotationConfigurationTag = false;
}

bool AnnotationConfigurationHandler::startElement(
    const QString & /* namespaceURI */,
    const QString & /* localName */,
    const QString &qName,
    const QXmlAttributes &attributes )
{
  if ( !metAnnotationConfigurationTag && qName != "annotationConfiguration" )
  {
    errorStr = QObject::tr( "The file is not an Annotation Configuration file." );
    return false;
  }

  if ( qName == "annotationConfiguration" )
  {
    metAnnotationConfigurationTag = true;
  }
  else if ( qName == "entity" )
  {
    QListWidgetItem *item = new QListWidgetItem(listWidget);
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    if (m_isEditor)
    {
      item->setFlags( item->flags() | Qt::ItemIsUserCheckable);
    }
    item->setText( attributes.value( "name" ) );
    QColor color( attributes.value( "color" ) );
    QColor white( Qt::white );
    QBrush brush( color );

    colors->push_back( QColor( attributes.value( "color" ).toLower() ) );
    (*colorNames2EntityTypes)[attributes.value( "color" ).toLower()] = attributes.value( "name" );
    /// @todo setBackgroundColor is deprecated in QT 4.2, replace by
    /// setBackground below after upgrading.
    /// item->setBackgroundColor  ( QColor( attributes.value( "color" ) ) );
    if (attributes.value("recursive") == "true")
    {
      m_recursiveEntityTypes->push_back(attributes.value( "name" ));
      QFont font = item->font();
      font.setItalic(true);
      font.setBold(true);
      item->setFont(font);
      if (m_isEditor)
      {
        item->setCheckState(Qt::Checked);
      }
    }
    else
    {
      if (m_isEditor)
      {
        item->setCheckState(Qt::Unchecked);
      }
    }
    item->setBackground  ( brush );
    item->setTextColor  ( white );
  }

  return true;
}

bool AnnotationConfigurationHandler::endElement(
    const QString & /* namespaceURI */,
    const QString & /* localName */,
    const QString & /* qName */ )
{
  return true;
}

bool AnnotationConfigurationHandler::characters( const QString & /* str */ )
{
  return true;
}

bool AnnotationConfigurationHandler::fatalError(
     const QXmlParseException &exception )
{
  QMessageBox::information( listWidget->window(), QObject::tr( "SAX Bookmarks" ),
                            QObject::tr( "Parse error at line %1, column %2:\n"
                                         "%3" )
                            .arg( exception.lineNumber() )
                            .arg( exception.columnNumber() )
                            .arg( exception.message() ) );
  return false;
}

QString AnnotationConfigurationHandler::errorString() const

{
  return errorStr;
}

