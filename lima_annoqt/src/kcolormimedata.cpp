// Copyright (C) 1999 Steffen Hansen (hansen@kde.org)
// Copyright (C) 2005 Joseph Wenninger (jowenn@kde.org)
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "kcolormimedata.h"

#include <QColor>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QWidget>
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
