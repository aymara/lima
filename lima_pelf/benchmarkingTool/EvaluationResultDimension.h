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
#ifndef LIMA_PELF_EVALUATIONRESULTDIMENSION_H
#define LIMA_PELF_EVALUATIONRESULTDIMENSION_H

#include <QtCore/QtDebug>
#include <QtCore/QMultiMap>
#include <QtGui/QColor>

#include <qwt/qwt_symbol.h>

namespace Lima {
namespace Pelf {

class EvaluationResultDimension : public QObject
{

Q_OBJECT

public:
    virtual ~EvaluationResultDimension();
    
    QString name;
    QString key;
    QColor color;
    int id;
    bool rel;
    Qt::CheckState visibilityState;
    QwtSymbol* markerSymbol;
    QwtSymbol* markerSymbolSelected;

    EvaluationResultDimension (QString n, QString k, QColor c, int i, bool r);

Q_SIGNALS:
    void visibleChanged ();

public Q_SLOTS:

    void updateVisibleChanged (int state);


};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_EVALUATIONRESULTDIMENSION_H
