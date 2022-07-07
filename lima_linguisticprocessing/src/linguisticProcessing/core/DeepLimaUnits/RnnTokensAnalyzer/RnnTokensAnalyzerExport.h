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

#include <common/LimaCommon.h>

#ifndef LIMA_RNNTOKENSANALYZEREXPORT_H
#define LIMA_RNNTOKENSANALYZEREXPORT_H

#ifdef WIN32

#ifdef LIMA_RNNTOKENSANALYZER_EXPORTING
   #define LIMA_RNNTOKENSANALYZER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_RNNTOKENSANALYZER_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_RNNTOKENSANALYZER_EXPORT

#endif

#endif
