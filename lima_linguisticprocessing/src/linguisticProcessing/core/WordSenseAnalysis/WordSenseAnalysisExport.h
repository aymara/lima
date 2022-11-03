// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_WORDSENSEANALYSISEXPORT_H
#define LIMA_WORDSENSEANALYSISEXPORT_H

#ifdef WIN32

#ifdef LIMA_WORDSENSEANALYSIS_EXPORTING
   #define LIMA_WORDSENSEANALYSIS_EXPORT    __declspec(dllexport)
#else
   #define LIMA_WORDSENSEANALYSIS_EXPORT    __declspec(dllimport)
#endif

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int32 uint;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
// typedef uint64_t uint64_t;


#else // Not WIN32

#define LIMA_WORDSENSEANALYSIS_EXPORT

#include <cstdint>

#endif

#endif
