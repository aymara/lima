// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT


#ifndef DEEPLIMA_CONVERTFROMTORCHEXPORT_H
#define DEEPLIMA_CONVERTFROMTORCHEXPORT_H

#ifdef WIN32

#ifdef DEEPLIMA_CONVERTFROMTORCH_EXPORTING
   #define DEEPLIMA_CONVERTFROMTORCH_EXPORT    __declspec(dllexport)
#else
   #define DEEPLIMA_CONVERTFROMTORCH_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define DEEPLIMA_CONVERTFROMTORCH_EXPORT

#endif

#endif
