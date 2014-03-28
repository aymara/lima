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
#include "PipelineUnit.h"

#include <QMessageBox>

using namespace Lima::Pelf;

PipelineUnit::PipelineUnit () :
    status(STATUS_UNPROCESSED),
    dropIndicator(false),
    m_process(new QProcess(this))
{
  m_process->setProcessChannelMode(QProcess::MergedChannels);
  connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(commandFinished(int, QProcess::ExitStatus)));
  connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(commandError(QProcess::ProcessError)));
}

void PipelineUnit::setTextPath (QString tp)
{
    textPath = tp;
    QFileInfo textFile(textPath);
    if(textFile.exists() && textFile.isFile())
        name = textFile.fileName();
    else
        name = tp;
}

QString PipelineUnit::getTextPath ()
{
    return textPath;
}

void PipelineUnit::startBenchmarking (Command* command, QString workingDir, const QDateTime& startTime)
{
    if(status != STATUS_UNPROCESSED || command == 0)
        return;
    status = STATUS_PROCESSING;
    currentCommand = command;
    currentWorkingDir = workingDir;
    m_startTime = startTime;
    qDebug() << "Start benchmarking file " << name;
    commandStart();
}

void PipelineUnit::commandStart()
{
    if(status != STATUS_PROCESSING)
        return;
    if (m_process->state() != QProcess::NotRunning)
    {
      QMessageBox::warning(0, tr("Command Error"), tr("Cannot start command: process still running"));
      qWarning() << "Cannot start command: process still running";
      return;
    }
    QFileInfo fileInfo(currentWorkingDir);
    if(fileInfo.exists() && fileInfo.isDir())
        m_process->setWorkingDirectory(currentWorkingDir);
    else
        m_process->setWorkingDirectory("/tmp");
    qDebug() << "Start benchmarking command for file " << name;
    QString commandLine = QString(getenv("PELF_COMMANDS_ROOT"))+"/"+currentCommand->commandLine;
    commandLine.replace("$START_TIME",QString::number(m_startTime.toTime_t()));
    commandLine.replace("$TEXT_PATH", textPath);
    commandLine.replace("$REFERENCE_PATH", referencePath);
    qDebug() << "Issuing command " << commandLine;
    m_process->start(commandLine);
}

void PipelineUnit::commandFinished (int exitCode, QProcess::ExitStatus exitStatus)
{
    if(status != STATUS_PROCESSING)
        return;
    if(exitCode != 0 || exitStatus != QProcess::NormalExit)
    {
        qDebug() << "Command error while processing " << name << ", output is :" << m_process->readAll();
        return;
    }
    qDebug() << "Finished benchmarking command for file " << name;
    if(currentCommand->nextCommand != 0)
    {
        currentCommand = currentCommand->nextCommand;
        commandStart();
    }
    else
    {
//         qDebug() << "Finished benchmarking file " << name << ", updating results views";
        QString output = m_process->readAll();
        m_process->close();
//         qDebug() << "Finished benchmarking file " << name << ", output read finished";
        EvaluationResultSet* resultSet = new EvaluationResultSet();
        resultSet->findEvaluationResults(output);
//         qDebug() << "Finished benchmarking file " << name << ", evaluation results found, processing results output";
        status = STATUS_PROCESSED;
        emit unitResultsChanged(this, resultSet);
    }
}

void PipelineUnit::commandError (QProcess::ProcessError error)
{
  switch (error)
  {
  case QProcess::FailedToStart:
    qDebug() << "Command error while processing " << name << ": The process failed to start. ";
  break;
  case QProcess::Crashed:
    qDebug() << "Command error while processing " << name << ": The process crashed some time after starting successfully.";
  break;
  case QProcess::Timedout:
    qDebug() << "Command error while processing " << name << ": The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.";
  break;
  case QProcess::WriteError:
    qDebug() << "Command error while processing " << name << ": An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.";
  break;
  case QProcess::ReadError:
    qDebug() << "Command error while processing " << name << ": An error occurred when attempting to read from the process. For example, the process may not be running.";
  break;
  case QProcess::UnknownError:;
  default:
    qDebug() << "Undetermined command error while processing " << name << " (QT code: " << error << ")";
  }
}

#include "PipelineUnit.moc"
