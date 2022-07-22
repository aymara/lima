// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "ResultsModel.h"
using namespace Lima::Pelf;

ResultsModel::ResultsModel (QTableView* view, Pipeline* p) :
    QAbstractTableModel(view),
    pipeline(p) {}

int ResultsModel::rowCount (const QModelIndex& parent) const
{
    return pipeline->results.size();
}

int ResultsModel::columnCount (const QModelIndex& parent) const
{
    return EvaluationResult::getDimensionsVisible().size();
}

QVariant ResultsModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal)
    {
        QList<EvaluationResultDimension*> dimensionsVisible = EvaluationResult::getDimensionsVisible();
        if(section < 0 || section >= (int)dimensionsVisible.size())
            return QVariant();
        QString name = dimensionsVisible[section]->name;
        if(name == "ALL CONSTITUANTS")
            name = "CONSTS";
        if(name == "ALL RELATIONS")
            name = "RELS";
        return name;
    }
    if(orientation == Qt::Vertical)
    {
        QList<BenchmarkingResult*> resultsVector = pipeline->getResultsVector();
        if(section < 0 || section >= (int)resultsVector.size())
            return QVariant();
        QDateTime resultsTime = resultsVector[section]->time;
        if(resultsTime.isNull())
            return QVariant();
        return "#"+QString::number(section+1)+" : "+resultsTime.toString("dd/MM hh:mm");
    }
    return QVariant();
}

QVariant ResultsModel::data (const QModelIndex& index, int role) const
{
    int row = index.row();
    int column = index.column();
    if(role == Qt::ToolTipRole)
    {
        QString toolTip;
        QList<BenchmarkingResult*> resultsVector = pipeline->getResultsVector();
        if(row < 0 || row >= (int)resultsVector.size())
            return QVariant();
        BenchmarkingResult* benchmarkingResult = resultsVector[row];
        if(!benchmarkingResult->time.isNull())
            toolTip += "\nStarted: "+benchmarkingResult->time.toString("dddd d MMMM yyyy at hh:mm:ss");
        if(!benchmarkingResult->comment.isNull())
            toolTip += "\nComment: "+benchmarkingResult->comment;
        return QVariant(toolTip.trimmed());
    }
    if(role == Qt::TextColorRole)
    {
        double displayedData = dataForCell(row, column);
        double prevDisplayedData = dataForCell(row - 1, column);
        if(displayedData == -1 || prevDisplayedData == -1 || displayedData == prevDisplayedData)
            return QVariant();
        else if(displayedData > prevDisplayedData)
            return QVariant(QColor(revertColors ? Qt::red : Qt::green));
        else
            return QVariant(QColor(revertColors ? Qt::green : Qt::red));
    }
    if(role != Qt::DisplayRole)
        return QVariant();
    double displayedData = dataForCell(row, column) * 100;
    int precision = ((int)(displayedData*100))%100 == 0 ? 0 : 2;
    QString displayedDataString = displayedData != -100 ? QString::number(displayedData, 'f', precision) : "/";
    return QVariant(displayedDataString);
}

double ResultsModel::dataForCell (int row, int column) const
{
    QList<BenchmarkingResult*> resultsVector = pipeline->getResultsVector();
    if(row < 0 || row >= (int)resultsVector.size())
        return -1;
    QMap<PipelineUnit*, EvaluationResultSet*>& puResultSet = resultsVector[row]->resultUnits;
    QList<EvaluationResultDimension*> dimensionsVisible = EvaluationResult::getDimensionsVisible();
    if(column < 0 || column >= (int)dimensionsVisible.size())
        return -1;
    EvaluationResultDimension* dimension = dimensionsVisible[column];
    double sumFoundCorrect = 0, sumFound = 0, sumCorrect = 0, sumErrorAll = 0, sumErrorPrecision = 0, sumErrorRecall = 0;
    EvaluationResult::DIMENSION_ID dimensionId = (EvaluationResult::DIMENSION_ID)dimension->id;
    EvaluationResult::DIMENSION_ID dimensionGroupId = EvaluationResult::getDimensionGroup(dimensionId);
    Q_FOREACH(PipelineUnit* pipelineUnit, puResultSet.keys())
    {
        if(selectedUnitTextPath.isEmpty() || selectedUnitTextPath == pipelineUnit->textPath)
        {
            EvaluationResultSet* resultSet = puResultSet[pipelineUnit];
            EvaluationResultSet::iterator resultSetIt;
            resultSetIt = resultSet->find(dimensionId);
            if(resultSetIt != resultSet->end())
            {
                sumFoundCorrect += (*resultSetIt)->fc;
                sumFound += (*resultSetIt)->fp;
                sumCorrect += (*resultSetIt)->cr;
            }
            if(dimensionGroupId != -1)
            {
                resultSetIt = resultSet->find(dimensionGroupId);
                if(resultSetIt != resultSet->end())
                {
                    double errors;
                    errors = (*resultSetIt)->fp - (*resultSetIt)->fc;
                    sumErrorAll += errors;
                    sumErrorPrecision += errors;
                    errors = (*resultSetIt)->cr - (*resultSetIt)->fc;
                    sumErrorAll += errors;
                    sumErrorRecall += errors;
                }
            }
        }
    }
    double result = -1;
    revertColors = false;
    if(measureDisplay == FMEASURE)
    {
        double precision = sumFound != 0 ? sumFoundCorrect/sumFound : 0;
        double recall = sumCorrect != 0 ? sumFoundCorrect/sumCorrect : 0;
        result = precision + recall != 0 ? 2*precision*recall/(precision + recall) : 0;
    }
    else if(measureDisplay == PRECISION)
        result = sumFound != 0 ? sumFoundCorrect/sumFound : 0;
    else if(measureDisplay == RECALL)
        result = sumCorrect != 0 ? sumFoundCorrect/sumCorrect : 0;
    else if(measureDisplay == TYPE_ERROR_RATIO_ALL && dimensionGroupId != -1)
        result = (sumFound + sumCorrect - 2*sumFoundCorrect)/sumErrorAll;
    else if(measureDisplay == TYPE_ERROR_RATIO_PRECISION && dimensionGroupId != -1)
        result = (sumFound - sumFoundCorrect)/sumErrorPrecision;
    else if(measureDisplay == TYPE_ERROR_RATIO_RECALL && dimensionGroupId != -1)
        result = (sumCorrect - sumFoundCorrect)/sumErrorRecall;
    else if(measureDisplay == CORRECT)
        result = sumCorrect;
    else if(measureDisplay == FOUND)
        result = sumFound;
    else if(measureDisplay == CORRECT_FOUND)
        result = sumFoundCorrect;
    else if(measureDisplay == CORRECT_NOTFOUND)
    {
        result = sumCorrect - sumFoundCorrect;
        revertColors = true;
    }
    else if(measureDisplay == INCORRECT_FOUND)
    {
        result = sumFound - sumFoundCorrect;
        revertColors = true;
    }
    return result;
}

ResultsModel::MEASURE_DISPLAY ResultsModel::measureDisplay = ResultsModel::FMEASURE;
QString ResultsModel::selectedUnitTextPath;
bool ResultsModel::revertColors;

#include "ResultsModel.moc"
