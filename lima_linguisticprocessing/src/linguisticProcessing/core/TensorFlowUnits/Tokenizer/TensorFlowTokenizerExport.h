/*
    Copyright 2002-2019 CEA LIST

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
 *   Copyright (C) 2004-2019 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_TENSORFLOWTOKENIZEREXPORT_H
#define LIMA_TENSORFLOWTOKENIZEREXPORT_H

#ifdef WIN32

#ifdef LIMA_TENSORFLOWTOKENIZER_EXPORTING
   #define LIMA_TENSORFLOWTOKENIZER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_TENSORFLOWTOKENIZER_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_TENSORFLOWTOKENIZER_EXPORT

#endif

#endif
