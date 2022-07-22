// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "EvaluationResultDimension.h"

#include <QtGui/QBrush>
#include <QtGui/QPen>

using namespace Lima::Pelf;

EvaluationResultDimension::EvaluationResultDimension (QString n, QString k, QColor c, int i, bool r) :
    name(n),
    key(k),
    color(c),
    id(i),
    rel(r),
    visibilityState(Qt::Checked)
{
    markerSymbol = new QwtSymbol(QwtSymbol::XCross, QBrush(color), QPen(color), QSize(5, 5));
    markerSymbolSelected = new QwtSymbol(QwtSymbol::XCross, QBrush(color), QPen(color), QSize(10, 10));
}

EvaluationResultDimension::~EvaluationResultDimension()
{
  delete markerSymbol;
  delete markerSymbolSelected;
}

void EvaluationResultDimension::updateVisibleChanged (int state)
{
    visibilityState = (Qt::CheckState)state;
    Q_EMIT visibleChanged();
}

#include "EvaluationResultDimension.moc"
