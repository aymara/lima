/*
    Copyright 2012-2021 CEA LIST

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
//************************************************************************
//! @author     gael.de-chalendar@cea.fr
//! @date       October 2012
//! @brief  Symbol exporting for Windows DLL CoreXmlReaderClient.dll

#ifndef COREXMLREADERCLIENT_EXPORT_H
#define COREXMLREADERCLIENT_EXPORT_H

#ifdef WIN32
#include <cstdint>

#pragma warning( disable : 4512 )

// Avoids compilation errors redefining struc sockaddr in ws2def.h
#define _WINSOCKAPI_

#undef min
#undef max
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

#ifdef COREXMLREADERCLIENT_EXPORTING
  #define COREXMLREADERCLIENT_EXPORT    __declspec(dllexport)
#else
  #define COREXMLREADERCLIENT_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define COREXMLREADERCLIENT_EXPORT

#endif

#endif // COREXMLREADERCLIENT_EXPORT_H
