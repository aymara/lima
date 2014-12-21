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

#include "LimaMainTaskRunner.h"

namespace Lima {


LimaMainTaskRunner::LimaMainTaskRunner(int aargc,char** aargv,void (*afoo)(int, char**), QObject *parent) :
    QObject(parent),
    argc(aargc),
    argv(aargv), foo(afoo)
{
//   qDebug() << "LimaMainTaskRunner::LimaMainTaskRunner" <<argc << *argv;
}

void LimaMainTaskRunner::run()
{
  foo(argc, argv);
  Q_EMIT finished();
}

}
