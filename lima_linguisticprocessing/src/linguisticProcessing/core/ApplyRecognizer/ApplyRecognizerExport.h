// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_APPLYRECOGNIZEREXPORT_H
#define LIMA_APPLYRECOGNIZEREXPORT_H

#ifdef WIN32

#ifdef LIMA_APPLYRECOGNIZER_EXPORTING
   #define LIMA_APPLYRECOGNIZER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_APPLYRECOGNIZER_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_APPLYRECOGNIZER_EXPORT

#endif

#endif
