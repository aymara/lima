// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef AMOSEPLUGINMANAGERS_H
#define AMOSEPLUGINMANAGERS_H

#include "common/AbstractFactoryPattern/AbstractFactoryPatternExport.h"
#include "common/AbstractFactoryPattern/Singleton.h"

#include <QtCore/QString>

namespace Lima
{

class LIMA_FACTORY_EXPORT AmosePluginsManager :
public Singleton<AmosePluginsManager>
{
  friend class Singleton<AmosePluginsManager>;
public:
  virtual ~AmosePluginsManager() {}

  /**
   * @brief Load plugins in the 'plugins' subdir of the semicolon separated
   * config dirs
   *
   * If a plugin is present in several config dirs, then only the first
   * occurrence is loaded.
   *
   * If a plugin is commented out (line starting with the '#' symbol), then it
   * is considered as being forbidenned and then it will be ignored if found
   * again in subsequent directories.
   *
   * @param configDirs semicolon separated list of config dirs. If empty, loads
   * a default location.
   *
   * @return true if there is no error and false otherwise. The function return
   * at the first error.
   */
#if defined(__has_feature)
#  if __has_feature(address_sanitizer)
    __attribute__((no_sanitize("address")))
    __attribute__((no_sanitize("leak")))
    __attribute__((disable_sanitizer_instrumentation))
#  endif
#endif
  __attribute__((no_sanitize("address", "thread")))
  bool
  loadPlugins(const QString& configDirs = "");

private:
  AmosePluginsManager();

};

}
#endif
