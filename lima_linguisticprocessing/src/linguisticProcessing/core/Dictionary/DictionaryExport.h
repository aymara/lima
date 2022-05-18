// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_DICTIONARYEXPORT_H
#define LIMA_DICTIONARYEXPORT_H

#ifdef WIN32

#ifdef LIMA_DICTIONARY_EXPORTING
   #define LIMA_DICTIONARY_EXPORT    __declspec(dllexport)
#else
   #define LIMA_DICTIONARY_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_DICTIONARY_EXPORT

#endif

#endif
