// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

  void CoNLLUFile::save(const string& filename, bool /*force*/)
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

