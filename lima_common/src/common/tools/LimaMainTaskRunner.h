/*
    Copyright 2014 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
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
    LimaMainTaskRunner(int aargc,char** aargv,int (*foo)(int, char**),  QObject *parent = 0);
    virtual ~LimaMainTaskRunner() {}

    inline int lastResult() const { return result; }

public Q_SLOTS:
    void run();

Q_SIGNALS:
    void finished(int);

private:
  int argc;
  char** argv;
  int (*foo)(int, char**);
  int result;
};

}

#endif
