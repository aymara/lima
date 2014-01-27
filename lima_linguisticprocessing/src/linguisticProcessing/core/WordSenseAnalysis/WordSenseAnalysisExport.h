/*
    Copyright 2002-2013 CEA LIST

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
