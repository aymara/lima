/* This file is part of the KDE libraries
   Copyright (C) 1999 Steffen Hansen (hansen@kde.org)
   Copyright (C) 2005 Joseph Wenninger (jowenn@kde.org)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kcolormimedata.h"

#include <QColor>
#include <QDrag>
#include <QMimeData>
#include <QPainter>

void
KColorMimeData::populateMimeData(QMimeData *mimeData, const QColor &color)
{
    mimeData->setColorData(color);
    mimeData->setText(color.name());
}

bool
KColorMimeData::canDecode(const QMimeData *mimeData)
{
    if (mimeData->hasColor())
        return true;
    if (mimeData->hasText())
    {
        const QString colorName=mimeData->text();
        if ((colorName.length() >= 4) && (colorName[0] == '#'))
            return true;
    }
    return false;
}

QColor
KColorMimeData::fromMimeData(const QMimeData *mimeData)
{
    if (mimeData->hasColor())
        return mimeData->colorData().value<QColor>();
    if (canDecode(mimeData))
        return QColor(mimeData->text());
    return QColor();
}


QDrag*
KColorMimeData::createDrag(const QColor &color, QWidget *dragsource)
{
    QDrag *drag=new QDrag(dragsource);
    QMimeData *mime=new QMimeData;
    populateMimeData(mime,color);
    drag->setMimeData(mime);
    QPixmap colorpix( 25, 20 );
    colorpix.fill( color );
    QPainter p( &colorpix );
    p.setPen( Qt::black );
    p.drawRect(0,0,24,19);
    p.end();
    drag->setPixmap(colorpix);
    drag->setHotSpot(QPoint(-5,-7));
    return drag;
}
