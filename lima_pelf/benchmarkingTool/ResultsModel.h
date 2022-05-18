// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_RESULTSMODEL_H
#define LIMA_PELF_RESULTSMODEL_H

#include <QtCore/QtDebug>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QDateTime>
#include <QtWidgets/QTableView>
#include <QtGui/QColor>

#include "Pipeline.h"
#include "EvaluationResult.h"
#include "EvaluationResultDimension.h"

namespace Lima {
namespace Pelf {

class ResultsModel : public QAbstractTableModel
{

Q_OBJECT

public:

    enum MEASURE_DISPLAY {
        FMEASURE,
        PRECISION,
        RECALL,
        TYPE_ERROR_RATIO_ALL,
        TYPE_ERROR_RATIO_PRECISION,
        TYPE_ERROR_RATIO_RECALL,
        CORRECT,
        FOUND,
        CORRECT_FOUND,
        CORRECT_NOTFOUND,
        INCORRECT_FOUND,
    };
    static MEASURE_DISPLAY measureDisplay;
    static QString selectedUnitTextPath;
    static bool revertColors;

    ResultsModel (QTableView* view = 0, Pipeline* p = 0);
    int rowCount (const QModelIndex& parent = QModelIndex()) const;
    int columnCount (const QModelIndex& parent = QModelIndex()) const;
    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data (const QModelIndex& index, int role) const;
    double dataForCell (int row, int column) const;

private:

    Pipeline* pipeline;
    QList<EvaluationResultDimension*> dimensions;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESULTSMODEL_H
