// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2019 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_TENSORFLOWMORPHOSYNTAXEXPORT_H
#define LIMA_TENSORFLOWMORPHOSYNTAXEXPORT_H

#ifdef WIN32

#ifdef LIMA_TENSORFLOWMORPHOSYNTAX_EXPORTING
   #define LIMA_TENSORFLOWMORPHOSYNTAX_EXPORT    __declspec(dllexport)
#else
   #define LIMA_TENSORFLOWMORPHOSYNTAX_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_TENSORFLOWMORPHOSYNTAX_EXPORT

#endif

#endif
