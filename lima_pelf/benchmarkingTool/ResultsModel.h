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
