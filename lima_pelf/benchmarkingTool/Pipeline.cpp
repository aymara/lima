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
#include "Pipeline.h"
using namespace Lima::Pelf;

Pipeline::Pipeline (QListView* view) :
    QAbstractListModel(view),
    analyzerCommand(new Command()),
    evaluatorCommand(new Command()),
    concurrentProcesses(1),
    processing(false),
    startTime(),
    pipelineView(view),
    m_dirty(false)
{
}

Pipeline::~Pipeline()
{
  delete analyzerCommand;
  delete evaluatorCommand;
}

bool Pipeline::isEmpty()
{
    return units.size() <= 0;
}

void Pipeline::replaceConfiguration (QString wd, QString analyzerCmd, QString evaluatorCmd, int cp)
{
    workingDir = wd;
    if (QDir::setCurrent(workingDir))
      qDebug() << "Current working directory set to: " << workingDir;
    else 
      qDebug() << "Failed to set Current working directory to: " << workingDir;
    analyzerCommand->commandLine = analyzerCmd;
    evaluatorCommand->commandLine = evaluatorCmd;
    if (evaluatorCmd.isEmpty())
      analyzerCommand->nextCommand = 0;
    else
      analyzerCommand->nextCommand = evaluatorCommand;
    concurrentProcesses = cp;
    setDirty();
}

const QList<PipelineUnit*>& Pipeline::getUnits () const
{
    return units;
}

PipelineUnit* Pipeline::getUnit (const QModelIndex& index) const
{
    if(index.isValid() && index.row() < (int)units.size())
        return units[index.row()];
    return 0;
}

PipelineUnit* Pipeline::replaceUnit (QString textFilePath, QString referenceFilePath, QString fileType, PipelineUnit::STATES status, int unitIndex)
{
    PipelineUnit* unit;
    if(unitIndex == -1)
    {
        beginInsertRows(QModelIndex(), units.size(), units.size() + 1);
//         qDebug() << "Adding file to pipeline";
///@TODO Possible memory leak:  ensure that this object will be deleted
        unit = new PipelineUnit();
        connect(
            unit,
            SIGNAL(unitResultsChanged(PipelineUnit*, EvaluationResultSet*)),
            this,
            SLOT(unitResultsChanged(PipelineUnit*, EvaluationResultSet*))
        );
    }
    else
    {
        if(unitIndex >= (int)units.size())
        {
            qDebug() << "Pipeline unit index " <<  unitIndex << " not found, aborting";
            return 0;
        }
        qDebug() << "Updating file in pipeline";
        unit = units[unitIndex];
    }
    if(unit == 0)
    {
        qDebug() << "Error while creating or replacing pipeline unit, aborting";
        return 0;
    }
    unit->setTextPath(textFilePath);
    unit->referencePath = referenceFilePath;
    unit->fileType = fileType;
    unit->status = status;
    if(unitIndex == -1)
    {
        units.push_back(unit);
        endInsertRows();
    }
    unitsUpdate();
    return unit;
}

void Pipeline::moveUnits (QModelIndexList sourceIndexes, QModelIndex targetIndex)
{
    if(sourceIndexes.size() < 0)
        return;
    qDebug() << "Reordering pipeline files";
    QList<QModelIndex>::const_iterator sourceIndexIt;
    Q_EMIT layoutAboutToBeChanged();
    qSort(sourceIndexes);
    QList<PipelineUnit*> movedUnits;
    int shiftSourceIndexes = 0, shiftTargetIndex = 0, targetIndexRow = targetIndex.row();
    for(sourceIndexIt = sourceIndexes.constBegin(); sourceIndexIt != sourceIndexes.constEnd(); sourceIndexIt++)
    {
        int sourceIndexRow = (*sourceIndexIt).row();
        movedUnits.push_back(units[sourceIndexRow + shiftSourceIndexes]);
        shiftSourceIndexes = deleteUnit(*sourceIndexIt, shiftSourceIndexes);
        if(sourceIndexRow < targetIndexRow)
            shiftTargetIndex = shiftSourceIndexes;
    }
    Q_FOREACH(PipelineUnit* movedUnit, movedUnits)
    {
      units.insert(units.begin() + targetIndex.row() + shiftTargetIndex, movedUnit);
      shiftTargetIndex++;
    }
    Q_EMIT layoutChanged();
    pipelineView->clearSelection();
    unitsUpdate();
    setDirty();
}

void Pipeline::confirmDeleteUnits (QModelIndexList unitIndexes)
{
    if(unitIndexes.size() < 0)
        return;
    if(QMessageBox::question(
        pipelineView,
        "Remove pipeline units ?",
        "Unit configuration and evaluation results would be lost, do you really want to remove the selected pipeline units ?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
      ) == QMessageBox::Ok)
        deleteUnits(unitIndexes);
}

void Pipeline::deleteUnits (QModelIndexList unitIndexes)
{
    qDebug() << "Removing pipeline files";
    QList<QModelIndex>::const_iterator unitIndexesIt;
    Q_EMIT layoutAboutToBeChanged();
    qSort(unitIndexes);
    int shiftIndexes = 0;
    for(unitIndexesIt = unitIndexes.constBegin(); unitIndexesIt != unitIndexes.constEnd(); unitIndexesIt++)
        shiftIndexes = deleteUnit(*unitIndexesIt, shiftIndexes);
    Q_EMIT layoutChanged();
    setDirty();
    pipelineView->clearSelection();
}

int Pipeline::deleteUnit (QModelIndex unitIndex, int shiftIndexes)
{
    int unitIndexRow = unitIndex.row() + shiftIndexes;
    if(unitIndexRow < (int)units.size())
    {
        PipelineUnit* unit = *(units.begin() + unitIndexRow);
        units.erase(units.begin() + unitIndexRow);
        delete unit;
        unitsUpdate();
        return shiftIndexes - 1;
    }
    unitsUpdate();
    setDirty();
    return shiftIndexes;
}

void Pipeline::clearUnits ()
{
    Q_EMIT layoutAboutToBeChanged();
    QList<PipelineUnit*>::iterator unitsIt = units.begin();
    for(; unitsIt < units.end(); unitsIt++)
        delete (*unitsIt);
    units.clear();
    Q_EMIT layoutChanged();
    pipelineView->clearSelection();
    unitsUpdate();
    setDirty();
}

bool Pipeline::resetBenchmarking ()
{
    bool allProcessed = true;
    QList<PipelineUnit*>::iterator unitsIt = units.begin();
    for(; unitsIt < units.end(); unitsIt++)
        allProcessed &= (*unitsIt)->status != PipelineUnit::STATUS_PROCESSING;
    if(!allProcessed && !startTime.isNull())
    {
        if(QMessageBox::question(
            pipelineView,
            "Reset pipeline files processing states ?",
            "Do you really want to abort and reset current evaluation ?",
            QMessageBox::Ok | QMessageBox::Cancel,
            QMessageBox::Cancel
           ) != QMessageBox::Ok)
            return false;
        if(results.find(startTime) != results.end())
          results.erase(results.find(startTime));
        startTime = QDateTime();
    }
    unitsIt = units.begin();
    for(; unitsIt < units.end(); unitsIt++)
        (*unitsIt)->status = PipelineUnit::STATUS_UNPROCESSED;
    pipelineView->reset();
    Q_EMIT resultsChanged();
    qDebug() << "Reseted pipeline files processing states";
    processing = false;
    return true;
}

QList<BenchmarkingResult*> Pipeline::getResultsVector ()
{
    QList<BenchmarkingResult*> resultsVector;
    QMap<QDateTime, BenchmarkingResult*>::iterator resultsIt;
    for(resultsIt = results.begin(); resultsIt != results.end(); resultsIt++)
        resultsVector.push_back(*resultsIt);
    return resultsVector;
}

void Pipeline::clear ()
{
    clearUnits();
    results.clear();
    if (analyzerCommand != 0)
      delete analyzerCommand;
    analyzerCommand = new Command();
    if (evaluatorCommand != 0)
      delete evaluatorCommand;
    evaluatorCommand = new Command();
    workingDir = "";
    setDirty();
}

void Pipeline::startBenchmarking (BenchmarkingResult* benchmarkingResult)
{
    if(processing)
        return;
    processing = true;
    setDirty();
    startTime = QDateTime::currentDateTime();
    benchmarkingResult->time = startTime;
    results[startTime] = benchmarkingResult;
    qDebug() << "Starting benchmarking (" << startTime.toTime_t() << ")";
    bool hasToResetBenchmark = false;
    QList<PipelineUnit*>::iterator unitsIt = units.begin();
    for(; unitsIt < units.end(); unitsIt++)
        hasToResetBenchmark |= (*unitsIt)->status != PipelineUnit::STATUS_UNPROCESSED;
    if(hasToResetBenchmark && !resetBenchmarking())
        return;
    continueBenchmarking();
}

void Pipeline::pauseBenchmarking ()
{
    if(!processing)
        return;
    qDebug() << "Pausing benchmarking (wait for evaluation commands to finish)";
    processing = false;
}

void Pipeline::resumeBenchmarking ()
{
    qDebug() << "Resuming benchmarking";
    processing = true;
    continueBenchmarking();
}

void Pipeline::continueBenchmarking ()
{
    if(!processing)
        return;
    int currentProcesses = 0;
    QList<PipelineUnit*>::iterator unitsIt = units.begin();
    for(; unitsIt != units.end(); unitsIt++)
        if((*unitsIt)->status == PipelineUnit::STATUS_PROCESSING)
            currentProcesses++;
    unitsIt = units.begin();
    while(currentProcesses < concurrentProcesses && unitsIt != units.end())
    {
        PipelineUnit* pipelineUnit = *unitsIt;
        if(pipelineUnit->status == PipelineUnit::STATUS_UNPROCESSED)
        {
            pipelineUnit->startBenchmarking(analyzerCommand, workingDir, startTime);
            currentProcesses++;
        }
        unitsIt++;
    }
    processing = false;
    unitsIt = units.begin();
    for(; unitsIt < units.end(); unitsIt++)
        processing |= (*unitsIt)->status != PipelineUnit::STATUS_PROCESSED;
    if(!processing)
    {
        unitsIt = units.begin();
        for(; unitsIt != units.end(); unitsIt++)
            if((*unitsIt)->status == PipelineUnit::STATUS_PROCESSED)
                (*unitsIt)->status = PipelineUnit::STATUS_UNPROCESSED;
        Q_EMIT finishedBenchmarking();
    }
}

void Pipeline::unitsUpdate ()
{
    QMap<QDateTime, BenchmarkingResult*>::iterator resultsIt;
    Q_FOREACH (BenchmarkingResult* result, results)
    {
        QMap<PipelineUnit*, EvaluationResultSet*> newResultUnits;
        QList<PipelineUnit*>::iterator unitsIt = units.begin();
        for(; unitsIt < units.end(); unitsIt++)
            if(result->resultUnits.find(*unitsIt) != result->resultUnits.end())
                newResultUnits[*unitsIt] = result->resultUnits[*unitsIt];
        result->resultUnits = newResultUnits;
    }
    Q_EMIT unitsChanged();
}

void Pipeline::unitResultsChanged (PipelineUnit* pu, EvaluationResultSet* ers)
{
    if(startTime.isNull())
    {
        qDebug() << "Pipeline::unitResultsChanged startTime is null. abort";
        return;
    }
    if(results.find(startTime) != results.end())
    {
//       qDebug() << "Pipeline::unitResultsChanged"<<startTime<<" one element:" << (*(*ers->begin())).fc;
      results[startTime]->resultUnits[pu] = ers;
    }
    else
    {
      qDebug() << "Pipeline::unitResultsChanged startTime"<<startTime<<" not found";
    }
    pipelineView->reset();
    Q_EMIT resultsChanged();
    if(processing)
        continueBenchmarking();
}

int Pipeline::rowCount (const QModelIndex& parent) const
{
    return units.size();
}

QVariant Pipeline::data (const QModelIndex& index, int role) const
{
    PipelineUnit* pipelineUnit = getUnit(index);
    if(pipelineUnit != 0 && role == Qt::DisplayRole)
        return QVariant(pipelineUnit->name);
    return QVariant();
}

Qt::DropActions Pipeline::supportedDropActions() const
{
    return Qt::MoveAction;
}

void Pipeline::setDropIndicator (QModelIndex dropIndex)
{
    clearDropIndicator();
    PipelineUnit* dropUnit = getUnit(dropIndex);
    if(dropUnit != 0)
        dropUnit->dropIndicator = true;
}

void Pipeline::clearDropIndicator ()
{
    QList<PipelineUnit*>::iterator unitsIt = units.begin();
    for(; unitsIt < units.end(); unitsIt++)
        (*unitsIt)->dropIndicator = false;
}

Qt::ItemFlags Pipeline::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    if(index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    return QAbstractListModel::flags(index);
}

#include "Pipeline.moc"
