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
