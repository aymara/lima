// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
