// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2019 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_TENSORFLOWTOKENIZEREXPORT_H
#define LIMA_TENSORFLOWTOKENIZEREXPORT_H

#ifdef WIN32

#ifdef LIMA_TENSORFLOWTOKENIZER_EXPORTING
   #define LIMA_TENSORFLOWTOKENIZER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_TENSORFLOWTOKENIZER_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_TENSORFLOWTOKENIZER_EXPORT

#endif

#endif
