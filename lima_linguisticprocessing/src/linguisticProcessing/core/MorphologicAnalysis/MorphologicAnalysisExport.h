// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_MORPHOLOGICANALYSISEXPORT_H
#define LIMA_MORPHOLOGICANALYSISEXPORT_H

#ifdef WIN32

#ifdef LIMA_MORPHOLOGICANALYSIS_EXPORTING

   #define LIMA_MORPHOLOGICANALYSIS_EXPORT    __declspec(dllexport)
#else
   #define LIMA_MORPHOLOGICANALYSIS_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_MORPHOLOGICANALYSIS_EXPORT

#endif

#endif
