// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#include <QApplication>
#include "annoqt.h"

int main( int argc, char *argv[] )
{
  Q_INIT_RESOURCE( annoqt );
  QApplication app( argc, argv );
  Annoqt * mw = new Annoqt();
  mw->show();
  return app.exec();
}

