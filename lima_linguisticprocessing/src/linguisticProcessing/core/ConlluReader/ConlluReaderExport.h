// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_CONLLUREADEREXPORT_H
#define LIMA_CONLLUREADEREXPORT_H

#ifdef WIN32

#ifdef LIMA_CONLLUREADER_EXPORTING
   #define LIMA_CONLLUREADER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_CONLLUREADER_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_CONLLUREADER_EXPORT

#endif

#endif
