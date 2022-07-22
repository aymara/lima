// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#if defined(__WIN32__) || defined(WIN32)
#include "PortableGettimeofday.h"

void gettimeofday(struct timeval *tv, struct timezone *tz)
{
     DWORD count = GetTickCount();
     tv->tv_sec = (int)(count / 1000);
     tv->tv_usec = (count % 1000) * 1000;
}
#endif
