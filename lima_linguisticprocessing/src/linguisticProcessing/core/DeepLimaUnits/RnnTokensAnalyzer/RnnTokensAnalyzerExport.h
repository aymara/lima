// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_RNNTOKENSANALYZEREXPORT_H
#define LIMA_RNNTOKENSANALYZEREXPORT_H

#include <common/LimaCommon.h>

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
