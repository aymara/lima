/*
    Copyright 2021 CEA LIST

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

#include <cstdlib>
#include <boost/filesystem.hpp>

#include "path_resolver.h"

using namespace std;
namespace fs = boost::filesystem;

namespace deeplima
{

PathResolver::PathResolver()
{
  init();
}

string PathResolver::resolve(const string& prefix, const string& path, const vector<string>& accepted_ext) const
{
  fs::path p;
  if (prefix.empty())
  {
    p = path;
  }
  else
  {
    p = fs::path(prefix) / path;
  }

  if (p.is_absolute())
  {
    return p.string();
  }

  vector<string> candidates = { p.string() };
  if (!p.extension().empty())
  {
    candidates.push_back(p.stem().string());
  }

  for (const string& resources_path : m_resources_paths)
  {
    for (const string& fn : candidates)
    {
      if (fn.empty())
      {
        continue;
      }

      fs::path full_path = fs::path(resources_path) / fn;
      if (fs::is_regular_file(full_path) || fs::is_symlink(full_path))
      {
        return full_path.string();
      }

      for (const string& ext : accepted_ext)
      {
        if (ext.empty())
        {
          continue;
        }

        full_path = fs::path(resources_path) / fn;
        if (ext.substr(0, 1) != ".")
        {
          full_path += ".";
        }
        full_path += ext;

        if (fs::is_regular_file(full_path) || fs::is_symlink(full_path))
        {
          return full_path.string();
        }
      }
    }
  }

  return "";
}

string PathResolver::find_user_data_home()
{
  fs::path user_data_home;
  const char* p_xdg_data_home = getenv("XDG_DATA_HOME");

  if (nullptr != p_xdg_data_home)
  {
    user_data_home = p_xdg_data_home;
  }

  if (user_data_home.empty())
  {
    user_data_home = getenv("HOME");
    if (!user_data_home.empty())
    {
      user_data_home = user_data_home / ".local" / "share";
    }
  }

  return user_data_home.string();
}

void PathResolver::init()
{
  fs::path user_data_home = find_user_data_home();
  if (!user_data_home.empty())
  {
    m_resources_paths.emplace_back((user_data_home / "lima" / "resources").string());
  }
}

}

