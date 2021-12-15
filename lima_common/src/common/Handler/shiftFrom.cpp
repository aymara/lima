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

#include "shiftFrom.h"
#include "qtSgmlEntities.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <tuple>

namespace Lima {

class ShiftFromPrivate
{
public:
  ShiftFromPrivate(const QString& xml);
  ~ShiftFromPrivate() = default;

  void build_mapping();

  QMap<int, std::tuple<int, QString, QString> > m_shiftFrom;
  QString m_xml;
  QString m_xml_noent;
  QMap<int, QString> m_mapping;


};


const QString& ShiftFrom::xml()
{
  return m_d->m_xml;
}

const QString& ShiftFrom::xml_noent()
{
  return m_d->m_xml_noent;
}

/**
 * Replace in input the sequences in mapping (shifted by offset)
 *
 * This allowes tu put back interpreted entities that were replaced by
 * underscores to avoid introducing shifts in the input xml
 */
QString ShiftFrom::rebuild_text(const QString& input, int offset) const
{
  QString txt = input;
  qDebug() << "Handling '" << txt << "' at offset" << offset;
  qDebug() << "Mapping from" << offset << "to" << offset+txt.size();
  for (auto it = m_d->m_mapping.lowerBound(offset),
    it_end = m_d->m_mapping.upperBound(offset+txt.size());
       it != it_end; it++)
  {
    qDebug() << "it:" << it.key();
    txt = txt.replace(it.key()-offset, it.value().size(), it.value());
  }
  return txt;
}

/**
 * Return the correct absolute position of a token in its origin text
 * when found at position @ref indexOfToken in a shifted text, taking into
 * account shiftings in @ref shiftFrom and the initial @ref offset.
 */
int ShiftFrom::correct_offset(int offset, int indexOfToken) const
{
  qDebug() << "ShiftFrom::correct_offset("<<offset<<","<<indexOfToken<<")";
  qDebug() << "ShiftFrom::correct_offset" << m_d;
  qDebug() << "ShiftFrom::correct_offset" << m_d->m_shiftFrom.keys();
  auto correction = (
    m_d->m_shiftFrom.lowerBound(offset+indexOfToken)==m_d->m_shiftFrom.begin()
      ?0
      :(std::get<0>((m_d->m_shiftFrom.lowerBound(offset+indexOfToken)-1).value()))
                     );
  qDebug() << "correction:" << correction;
  auto correctedOffset = offset + correction;
  qDebug() << "corrected offset:" << correctedOffset;
  auto correctedPosition = correctedOffset+indexOfToken;
  qDebug() << "corrected position:" << correctedPosition;
  qDebug() << "";
  return correctedPosition;
}

ShiftFrom::ShiftFrom(const QString& xml):
  m_d(new ShiftFromPrivate(xml))
{
  qDebug() << "ShiftFrom::ShiftFrom" << xml;
}


ShiftFrom::~ShiftFrom()
{
  delete m_d;
}

ShiftFromPrivate::ShiftFromPrivate(const QString& xml):
  m_xml(xml)
// QMap<int, std::tuple<int, QString, QString> > buildShiftFrom(const QString& xml)
{
    qDebug() << "ShiftFromPrivate::ShiftFromPrivate" << m_xml;
    QRegExp rx("(&([^;]*);)");
    int shift = 0;
    int indexofent = 0;

    while ((indexofent = rx.indexIn(m_xml, indexofent)) != -1)
    {
      qDebug() << indexofent;
      qDebug() << rx.cap(1);
      int indexInResolved = indexofent-shift;
      QString entity = rx.cap(1);
      QString entityString = rx.cap(2);
      qDebug() << entity;
      QString parsedEntity = parseEntity(&entityString);
      shift += entity.size()-parsedEntity.size();
      qDebug() << parsedEntity;
      m_shiftFrom.insert(indexInResolved, {shift, entity, parsedEntity});
      qDebug() << "ShiftFromPrivate::ShiftFromPrivate indexofent:" << indexofent << "; indexInResolved:"
              << indexInResolved << "; shift:" << shift;

      indexofent += rx.matchedLength();
    }
    qDebug() << "ShiftFromPrivate::ShiftFromPrivate shiftFrom is:" << m_shiftFrom.keys();

    build_mapping();
}

void ShiftFromPrivate::build_mapping()
{
    qDebug() << m_xml;
    QRegExp rx("(&([^;]*);)");
    m_xml_noent = m_xml;
    int indexofent = 0;

    while ((indexofent = rx.indexIn(m_xml_noent, indexofent)) != -1)
    {
      auto entity = rx.cap(1);
      auto entityString = rx.cap(2);
      auto parsedEntity = parseEntity(&entityString);
      m_xml_noent.replace(indexofent, entity.size(),
                        QString(parsedEntity.size(),'_'));
      m_mapping.insert(indexofent, parsedEntity);
      indexofent += parsedEntity.size();
    }
    qDebug() << "xml_noent:" << m_xml_noent;
    qDebug() << "mapping:" << m_mapping;
}


} // end namespace
