// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_COMMAND_H
#define LIMA_PELF_COMMAND_H

#include <QtCore/QtDebug>

namespace Lima {
namespace Pelf {

class Command
{

public:

    QString commandLine;
    Command* nextCommand;

    Command ();

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_COMMAND_H
