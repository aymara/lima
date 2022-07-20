// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_INCLUDE_PATH_RESOLVER
#define DEEPLIMA_INCLUDE_PATH_RESOLVER

#include <string>
#include <vector>

namespace deeplima
{

class PathResolver
{
public:
  PathResolver();

  std::string resolve(const std::string& prefix, const std::string& path, const std::vector<std::string>& accepted_ext = {}) const;

protected:
  std::string find_user_data_home();
  void init();

  std::vector<std::string> m_resources_paths;
};

}

#endif

