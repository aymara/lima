// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef PORTABLEGETTIMEOFDAY_DEFINED
#define PORTABLEGETTIMEOFDAY_DEFINED 1

#if defined(__WIN32__) || defined(WIN32)

#include "windows.h"

//struct timeval {
// long tv_sec; /* seconds since Jan. 1, 1970 */
// long tv_usec; /* and microseconds */
//};

struct timezone {
int tz_minuteswest; /* of Greenwich */
int tz_dsttime; /* type of dst correction to apply */
};

extern void gettimeofday(struct timeval *tv, struct timezone *tz);

#else

#include <sys/time.h>

#endif

#endif
