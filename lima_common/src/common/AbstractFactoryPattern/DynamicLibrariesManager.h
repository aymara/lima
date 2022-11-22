// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** **********************************************************************
 *
 * @file       DynamicLibrariesManager.h
 * @author     Romaric Besancon <romaric.besancon@cea.fr> (initial implementation)
 * @author     Gael de Chalendar <gael.de-chalendar@cea.fr> (port to QLibrary)
 * @date       Wed Feb  6 2008
 * copyright   Copyright (C) 2008-2012 by CEA LIST
 * Project     common
 * 
 * @brief      handle dynamic library opening (wrapper around QLibrary)
 * 
 * 
 ***********************************************************************/

#ifndef DYNAMICLIBRARIES_H
#define DYNAMICLIBRARIES_H

#include "common/AbstractFactoryPattern/AbstractFactoryPatternExport.h"
#include "common/AbstractFactoryPattern/Singleton.h"

#include <QtCore/QLibrary>

#include <string>
#include <map>
#include <vector>
#include <memory>

class QString;
namespace Lima {
namespace Common {

class DynamicLibrariesManagerPrivate;
class LIMA_FACTORY_EXPORT DynamicLibrariesManager: public Singleton<DynamicLibrariesManager>
{
friend class Singleton<DynamicLibrariesManager>;

public:
  ~DynamicLibrariesManager();
  
  bool isLoaded(const std::string& libName);
#if defined(__has_feature)
#  if __has_feature(address_sanitizer)
    __attribute__((no_sanitize("address")))
    __attribute__((no_sanitize("leak")))
    __attribute__((disable_sanitizer_instrumentation))
#  endif
#endif
  __attribute__((no_sanitize("address", "thread")))
  bool
   loadLibrary(const std::string& libName);
  void addSearchPath(const std::string& searchPath);
  void addSearchPathes(QString searchPathes);

 private:
  DynamicLibrariesManager();
  
  std::unique_ptr<DynamicLibrariesManagerPrivate> m_d;
};

} // end namespace
} // end namespace

#endif
