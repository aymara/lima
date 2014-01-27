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
#ifndef SYANOTPROCESS_H
#define SYANOTPROCESS_H

#include <QtCore/QProcess>

class SyanotProcessFactory;

class SyanotProcess : public QProcess
{
friend class SyanotProcessFactory;

private:
  SyanotProcess(const QString& workingDir, const QString& utterances,
                const QString& otherFilename, const QString& outputFilename);
  virtual ~SyanotProcess();

private Q_SLOTS:
  void slotFinished ( int exitCode, QProcess::ExitStatus exitStatus );
};

class SyanotProcessFactory
{
public:
  static void instance(const QString& workingDir, const QString& utterances,
                        const QString& otherFilename, const QString& outputFilename)
  {
    new SyanotProcess(workingDir, utterances, otherFilename, outputFilename);
  }
};
#endif // SYANOTPROCESS_H
