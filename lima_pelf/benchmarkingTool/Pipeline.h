// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

public Q_SLOTS:

    void unitResultsChanged (PipelineUnit* pu, EvaluationResultSet* ers);

Q_SIGNALS:

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
