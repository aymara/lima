// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_FLATTOKENIZEREXPORT_H
#define LIMA_FLATTOKENIZEREXPORT_H

#ifdef WIN32

#ifdef LIMA_FLATTOKENIZER_EXPORTING
   #define LIMA_FLATTOKENIZER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_FLATTOKENIZER_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_FLATTOKENIZER_EXPORT

#endif

#endif
