/*
    Copyright 2002-2021 CEA LIST

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
#ifndef LIMA_TEST_CMD_LINE_HELPERS
#define LIMA_TEST_CMD_LINE_HELPERS

#include <string>
#include <map>

#include <QtCore/QString>

namespace Lima
{

inline std::map<std::string, std::string> parse_options_line(const QString& meta,
                                                             char comma,
                                                             char colon,
                                                             const std::map<std::string, std::string>& append = {})
{
  std::map<std::string, std::string> opts;
  std::string s = meta.toStdString();

  size_t start = 0;
  size_t comma_pos = s.find(comma, 0);
  do
  {
    std::string key, value;
    size_t colon_pos = s.find(colon, start);
    if (colon_pos != std::string::npos && colon_pos != comma_pos)
    {
      key = s.substr(start, colon_pos - start);
      size_t value_start = colon_pos + 1;
      value = s.substr(value_start, comma_pos == std::string::npos ? comma_pos : (comma_pos - value_start));
    }
    else
    {
      key = s.substr(start, comma_pos == std::string::npos ? comma_pos : (comma_pos - start - 1));
      value = "";
    }

    if (key.size() > 0)
    {
      if (opts.find(key) == opts.end())
      {
        opts[key] = value;
      }
      else
      {
        opts[key] = opts[key] + "," + value;
      }
    }
    start = (comma_pos == std::string::npos) ? comma_pos : comma_pos + 1;
    comma_pos = s.find(comma, start);
  } while (start != std::string::npos);

  for (const auto& kv : append)
  {
    if (opts.end() == opts.find(kv.first))
    {
      opts[kv.first] = kv.second;
    }
  }

  return opts;
}

}

#endif // LIMA_TEST_CMD_LINE_HELPERS
