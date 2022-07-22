// Copyright 2014 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LIMAMAINTASKRUNNER_H
#define LIMA_LIMAMAINTASKRUNNER_H

#include "common/LimaCommon.h"
#include <QObject>

namespace Lima {

/**
 * @class LimaMainTaskRunner
 * @brief This class allows to run a function (obtained through a function pointer) inside the Qt event loop, thus allowing the objects it instantiates to emit and receive signals.
 *
 * With its argc and argv parameters it allows it to act as a simple delegate to
 * the program main function. All programs wishing to use the Lima framework
 * should use it like this:
 * \code{.cpp}
#include "common/tools/LimaMainTaskRunner.h"

void run(int argc,char** argv)
{
  QsLogging::initQsLog();
  Lima::AmosePluginsManager::single();

  <other stuff>
}

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  Lima::LimaMainTaskRunner* task = new Lima::LimaMainTaskRunner(argc, argv, run, &a);
  QObject::connect(task, SIGNAL(finished()), &a, SLOT(quit()));
  QTimer::singleShot(0, task, SLOT(run()));
  return a.exec();
}
  \endcode
 */
class LIMA_COMMONTOOLS_EXPORT LimaMainTaskRunner : public QObject
{
    Q_OBJECT
public:
    LimaMainTaskRunner(int aargc, char** aargv, int (*foo)(int, char**),  QObject *parent = 0);
    virtual ~LimaMainTaskRunner() {}

public Q_SLOTS:
    void run();

Q_SIGNALS:
    void finished(int);

private:
  int argc;
  char** argv;
  int (*foo)(int, char**);
};

}

#endif
