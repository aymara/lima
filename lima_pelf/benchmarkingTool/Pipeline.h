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
#ifndef LIMA_PELF_PIPELINE_H
#define LIMA_PELF_PIPELINE_H

#include <QtCore/QtDebug>
#include <QtCore/QDir>
#include <QtCore/QAbstractListModel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMessageBox>
#include <QDateTime>

#include "PipelineUnit.h"
#include "Command.h"
#include "BenchmarkingResult.h"
#include "EvaluationResult.h"
#include "EvaluationResultSet.h"

namespace Lima {
namespace Pelf {

/** Represents the currently loaded pipeline */
class Pipeline : public QAbstractListModel
{

Q_OBJECT

public:
    explicit Pipeline(QListView* view = 0);
    virtual ~Pipeline();
    
    Command* analyzerCommand;
    Command* evaluatorCommand;
    QString workingDir;
    int concurrentProcesses;
    bool processing;
    QDateTime startTime;
    QMap<QDateTime, BenchmarkingResult*> results;

    inline bool dirty() const {return m_dirty;}
    inline void setClean() {m_dirty = false;}
    inline void setDirty() {m_dirty = true;}
    bool isEmpty();
    void replaceConfiguration (QString wd, QString analyzerCmd, QString evaluatorCmd, int cp);
    PipelineUnit* getUnit (const QModelIndex& index) const;
    const QList<PipelineUnit*>& getUnits () const;
    PipelineUnit* replaceUnit (QString textFilePath, QString referenceFilePath, QString fileType, PipelineUnit::STATES status, int unitIndex = -1);
    void moveUnits (QModelIndexList sourceIndex, QModelIndex targetIndex);
    void confirmDeleteUnits (QModelIndexList unitIndexes);
    void deleteUnits (QModelIndexList unitIndexes);
    int deleteUnit (QModelIndex unitIndex, int alreadyErased = 0);
    void clearUnits ();
    bool resetBenchmarking ();
    QList<BenchmarkingResult*> getResultsVector ();
    void clear ();
    void startBenchmarking (BenchmarkingResult* benchmarkingResult);
    void pauseBenchmarking ();
    void resumeBenchmarking ();
    void continueBenchmarking ();
    void unitsUpdate ();
    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    QVariant data (const QModelIndex &index, int role) const;
    void clearDropIndicator ();
    void setDropIndicator (QModelIndex dropIndex);
    Qt::DropActions supportedDropActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

public slots:

    void unitResultsChanged (PipelineUnit* pu, EvaluationResultSet* ers);

signals:

    void unitsChanged ();
    void resultsChanged ();
    void finishedBenchmarking ();

private:

    QList<PipelineUnit*> units;
    QListView* pipelineView;
    bool m_dirty;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_PIPELINE_H
