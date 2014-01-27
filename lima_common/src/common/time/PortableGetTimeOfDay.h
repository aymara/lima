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
#ifndef PORTABLEGETTIMEOFDAY_DEFINED
#define PORTABLEGETTIMEOFDAY_DEFINED 1

#include "common/LimaCommon.h"

#if defined(__WIN32__) || defined(WIN32)

#include <stdlib.h> // for timeval on Cygwin
#include <time.h>
#include <winsock2.h>

// Defined in winsock.h
//struct timeval {
// long tv_sec; /* seconds since Jan. 1, 1970 */
// long tv_usec; /* and microseconds */
//};

struct timezone {
int tz_minuteswest; /* of Greenwich */
int tz_dsttime; /* type of dst correction to apply */
};

LIMA_TIME_EXPORT void gettimeofday(struct timeval *tv, struct timezone *tz);

#else

#include <sys/time.h>

#endif

#endif
