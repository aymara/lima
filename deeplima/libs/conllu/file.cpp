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

#include <fstream>

#include "file.h"

using namespace std;

namespace deeplima
{
namespace CoNLLU
{
  void CoNLLUFile::load(const string& filename)
  {
    ifstream input(filename);
    if (!input.is_open())
    {
      throw invalid_argument(string("Can't open file \"") + filename + "\"");
    }

    string line;
    size_t n = 0;
    while (getline(input, line))
    {
      CoNLLULine l(line);
      _lines.push_back(l);
      n += 1;
    }
  }

  void CoNLLUFile::save(const string& filename, bool force)
  {
    ofstream output(filename);
    if (!output.is_open())
    {
      throw invalid_argument(string("Can't open file \"") + filename + "\"");
    }

    for (const CoNLLULine& l : _lines)
    {
      string s = l.write();
      output << s << endl;
    }
  }

} // namespace CoNLLU
} // namespace deeplima

