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
  QString commandLine = QString("syanot --comparewith=%1 --utterances=%2 %3").arg(otherFilename).arg(utterances).arg(outputFilename);
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
