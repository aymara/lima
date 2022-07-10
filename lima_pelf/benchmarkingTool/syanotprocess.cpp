// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "syanotprocess.h"

#include <QtDebug>
#include <QtCore/QFileInfo>

SyanotProcess::SyanotProcess(const QString& workingDir, const QString& utterances,
const QString& otherFilename, const QString& outputFilename) : QProcess()
{
  connect(this,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(slotFinished(int,QProcess::ExitStatus)));

  setProcessChannelMode(QProcess::MergedChannels);

  QFileInfo fileInfo(workingDir);
  if(fileInfo.exists() && fileInfo.isDir())
      setWorkingDirectory(workingDir);
  else
      setWorkingDirectory("/tmp");
  QString commandLine = QString(QLatin1String("syanot --comparewith=%1 --utterances=%2 %3")).arg(otherFilename).arg(utterances).arg(outputFilename);
  qDebug() << "SyanotProcess::SyanotProcess Issuing command " << commandLine;
  start(commandLine);
}

SyanotProcess::~SyanotProcess()
{
}

void SyanotProcess::slotFinished ( int exitCode, QProcess::ExitStatus exitStatus )
{
  delete this;
}
