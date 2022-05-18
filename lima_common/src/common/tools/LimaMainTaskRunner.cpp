// Copyright 2014 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "LimaMainTaskRunner.h"

namespace Lima {


LimaMainTaskRunner::LimaMainTaskRunner(int aargc, char** aargv, int (*afoo)(int, char**), QObject *parent) :
    QObject(parent),
    argc(aargc),
    argv(aargv), foo(afoo)
{
//   qDebug() << "LimaMainTaskRunner::LimaMainTaskRunner" <<argc << *argv;
}

void LimaMainTaskRunner::run()
{
  int result = foo(argc, argv);
  Q_EMIT finished(result);
}

}
