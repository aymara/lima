// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_SPECIFICENTITIESEXPORT_H
#define LIMA_SPECIFICENTITIESEXPORT_H

#ifdef WIN32

#undef min
#undef max

#ifdef LIMA_SPECIFICENTITIES_EXPORTING
   #define LIMA_SPECIFICENTITIES_EXPORT    __declspec(dllexport)
#else
   #define LIMA_SPECIFICENTITIES_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_SPECIFICENTITIES_DATETIME_EXPORTING
   #define LIMA_SPECIFICENTITIES_DATETIME_EXPORT    __declspec(dllexport)
#else
   #define LIMA_SPECIFICENTITIES_DATETIME_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_SPECIFICENTITIES_NUMBER_EXPORTING
   #define LIMA_SPECIFICENTITIES_NUMBER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_SPECIFICENTITIES_NUMBER_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_SPECIFICENTITIES_PERSON_EXPORTING
   #define LIMA_SPECIFICENTITIES_PERSON_EXPORT    __declspec(dllexport)
#else
   #define LIMA_SPECIFICENTITIES_PERSON_EXPORT    __declspec(dllimport)
#endif

#else // Not WIN32

#define LIMA_SPECIFICENTITIES_EXPORT
#define LIMA_SPECIFICENTITIES_DATETIME_EXPORT
#define LIMA_SPECIFICENTITIES_NUMBER_EXPORT
#define LIMA_SPECIFICENTITIES_PERSON_EXPORT

#endif

#endif
