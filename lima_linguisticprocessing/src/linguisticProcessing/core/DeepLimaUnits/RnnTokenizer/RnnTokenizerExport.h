// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_RNNTOKENIZEREXPORT_H
#define LIMA_RNNTOKENIZEREXPORT_H

#include <common/LimaCommon.h>

#ifdef WIN32

#ifdef LIMA_RNNTOKENIZER_EXPORTING
   #define LIMA_RNNTOKENIZER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_RNNTOKENIZER_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_RNNTOKENIZER_EXPORT

#endif

#endif
