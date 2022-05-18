// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
