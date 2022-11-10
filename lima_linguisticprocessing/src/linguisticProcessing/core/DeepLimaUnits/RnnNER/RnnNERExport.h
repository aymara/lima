// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_RNNNEREXPORT_H
#define LIMA_RNNNEREXPORT_H

#include <common/LimaCommon.h>

#ifdef WIN32

#ifdef LIMA_RNNTAGGER_EXPORTING
   #define LIMA_RNNTAGGER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_RNNTAGGER_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_RNNNER_EXPORT

#endif

#endif
