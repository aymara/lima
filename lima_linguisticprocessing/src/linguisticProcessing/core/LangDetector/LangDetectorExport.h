// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_LANGDEDECTOREXPORT_H
#define LIMA_LANGDEDECTOREXPORT_H

#ifdef WIN32

#ifdef LIMA_LANGDEDECTOR_EXPORTING
   #define LIMA_LANGDEDECTOR_EXPORT    __declspec(dllexport)
#else
   #define LIMA_LANGDEDECTOR_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_LANGDEDECTOR_EXPORT

#endif

#endif
