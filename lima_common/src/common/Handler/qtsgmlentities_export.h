//************************************************************************
//! @author     gael.de-chalendar@cea.fr
//! @date       December 2021
//! copyright   Copyright (C) 2012 by CEA LIST (DIASI/LVIC)
//! @brief  Symbol exporting for Windows DLL

#ifndef QTSGMLENTITIES_EXPORT_H
#define QTSGMLENTITIES_EXPORT_H

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

#ifdef QTSGMLENTITIES_EXPORTING
  #define QTSGMLENTITIES_EXPORT    __declspec(dllexport)
#else
  #define QTSGMLENTITIES_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define QTSGMLENTITIES_EXPORT

#endif

#endif // QTSGMLENTITIES_EXPORT_H
