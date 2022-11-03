// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       AbstractFactoryPatternExport.h
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

 * @date       Sept, 12 2012
 * copyright   Copyright (C) 2012 by CEA LIST
 * Project     LIMA
 *
 * @brief      symbol exportation defines for Windows DLL
 *
 ***********************************************************************/
#ifndef LIMA_ABSTRACTFACTORYPATTERNEXPORT_H
#define LIMA_ABSTRACTFACTORYPATTERNEXPORT_H


#include <boost/serialization/strong_typedef.hpp>

#ifdef WIN32


#ifdef LIMA_FACTORY_EXPORTING
#define LIMA_FACTORY_EXPORT    __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define LIMA_FACTORY_EXPORT    __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif


#else // Not WIN32

#define EXPIMP_TEMPLATE

#define LIMA_FACTORY_EXPORT

#endif


#endif // LIMA_ABSTRACTFACTORYPATTERNEXPORT_H
