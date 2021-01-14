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

#include "specificEntitiesHandler.h"

#include <QTextEdit>
#include <iostream>

SpecificEntitiesHandler::SpecificEntitiesHandler(
  QList<SpecificEntity*>& entities,
  QTextEdit *textEdit,
  QMap<QString,quint32>& entityNames2Types ) :
      textEdit( textEdit ),
      m_entityNames2Types(entityNames2Types),
      bpos(false), blen(false), btype(false),
      m_entities(entities)
{
  metSpecificEntitiesTag = false;
}

bool SpecificEntitiesHandler::startElement(
    const QString & /* namespaceURI */,
    const QString & /* localName */,
    const QString &qName,
    const QXmlAttributes & /* attributes */ )
{
//   if ( !metSpecificEntitiesTag && qName != "specific-entities" )
//   {
//     errorStr = QObject::tr( "The file is not a Specific Entities file." );
//     return false;
//   }

  qDebug() << "startElement:" << qName;
  if ( qName == "specific_entities" )
  {
    metSpecificEntitiesTag = true;
  }
  else if ( qName == "position" )
  {
    bpos = true;
  }
  else if ( qName == "length" )
  {
    blen = true;
  }
  else if ( qName == "type" )
  {
    btype = true;
  }
  else if ( qName == "string" )
  {
    bstring = true;
  }

  return true;
}

bool SpecificEntitiesHandler::endElement(
    const QString & /* namespaceURI */,
    const QString & /* localName */,
    const QString & qName )
{
  if ( qName == "specific_entities" )
  {
    QTextCursor cursor = textEdit->textCursor();
    cursor.setPosition(0);
    textEdit->setTextCursor(cursor);
  }
  else if ( qName == "specific_entity" )
  {
    qDebug() << "entity " << m_str << type << " at " << pos << " on " << len;
    SpecificEntity* entity = new SpecificEntity(pos.toInt()-1,len.toInt(),m_entityNames2Types[type],m_str);
    m_entities.push_back(entity);
 }
  else if ( qName == "position" )
  {
    bpos = false;
  }
  else if ( qName == "length" )
  {
    blen = false;
  }
  else if ( qName == "type" )
  {
    btype = false;
  }
  else if ( qName == "string" )
  {
    bstring = false;
  }
  return true;
}

bool SpecificEntitiesHandler::characters( const QString & str )
{
  if (bpos)
  {
    pos = str;
  }
  else if (blen)
  {
    len = str;
  }
  else if (btype)
  {
    type = str;
  }
  else if (bstring)
  {
    m_str = str;
  }
  return true;
}

bool SpecificEntitiesHandler::fatalError(
     const QXmlParseException &exception )
{
  QMessageBox::information( textEdit->window(), QObject::tr( "Specific Entities" ),
                            QObject::tr( "Parse error at line %1, column %2:\n"
                                         "%3" )
                            .arg( exception.lineNumber() )
                            .arg( exception.columnNumber() )
                            .arg( exception.message() ) );
  return false;
}

QString SpecificEntitiesHandler::errorString() const

{
  return errorStr;
}

