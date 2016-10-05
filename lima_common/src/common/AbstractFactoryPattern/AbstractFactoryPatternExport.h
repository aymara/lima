/*
    Copyright 2002-2013 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
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
