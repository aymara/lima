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
#include "PipelineUnitDelegate.h"
using namespace Lima::Pelf;

PipelineUnitDelegate::PipelineUnitDelegate (QObject* p, Pipeline* p2) :
    QItemDelegate(p)
{
    pipeline = p2;
}

void PipelineUnitDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    PipelineUnit* unit = pipeline->getUnit(index);
    switch(unit->status)
    {
    case PipelineUnit::STATUS_UNPROCESSED:
        painter->fillRect(option.rect, QBrush(Qt::green));
        break;
    case PipelineUnit::STATUS_PROCESSING:
        painter->fillRect(option.rect, QBrush(Qt::yellow));
        break;
    case PipelineUnit::STATUS_PROCESSED:
        painter->fillRect(option.rect, QBrush(Qt::gray));
        break;
    }
    if(unit->dropIndicator)
        painter->drawLine(option.rect.topLeft(), option.rect.topRight());
    QItemDelegate::paint(painter, option, index);
}

#include "PipelineUnitDelegate.moc"
