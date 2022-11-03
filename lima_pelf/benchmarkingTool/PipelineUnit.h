// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_PIPELINEUNIT_H
#define LIMA_PELF_PIPELINEUNIT_H

#include <QtCore/QtDebug>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>

#include "Command.h"
#include "EvaluationResult.h"
#include "EvaluationResultSet.h"

namespace Lima {
namespace Pelf {

class PipelineUnit : public QObject
{

Q_OBJECT

public:

    enum STATES { STATUS_UNPROCESSED = 0, STATUS_PROCESSING = 1, STATUS_PROCESSED = 2 };

    STATES status;
    bool dropIndicator;
    int fileId;
    QString name;
    QString textPath;
    QString referencePath;
    QString fileType;

    PipelineUnit ();
    void setTextPath (QString tp);
    QString getTextPath ();
    void startBenchmarking (Command* command, QString workingDir, const QDateTime& startTime);
    void commandStart ();

public Q_SLOTS:

    void commandFinished (int exitCode, QProcess::ExitStatus exitStatus);
    void commandError (QProcess::ProcessError error);

Q_SIGNALS:
    void unitResultsChanged (PipelineUnit* pipelineUnit, EvaluationResultSet* resultSet);

private:

    QProcess* m_process;
    Command* currentCommand;
    QString currentWorkingDir;
    QDateTime m_startTime;
};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_PIPELINEUNIT_H
