// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
