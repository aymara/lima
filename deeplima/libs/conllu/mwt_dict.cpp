// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "mwt_dict.h"

#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>

#include "line.h"

using namespace std;

namespace deeplima
{
namespace CoNLLU
{

size_t extract_mwt_dict(const vector<string>& conllu_files, ostream& out)
{
  // surface -> (expansion -> count)
  map<string, map<vector<string>, size_t>> dict;

  for (const auto& fn : conllu_files)
  {
    ifstream input(fn);
    if (!input.is_open())
    {
      throw runtime_error("extract_mwt_dict: can't open \"" + fn + "\"");
    }

    string line;
    while (getline(input, line))
    {
      if (line.empty() || line[0] == '#')
      {
        continue;
      }

      CoNLLULine l(line);
      if (!l.is_token_line() || !l.idx().is_multiword())
      {
        continue;
      }

      const size_t n_sub = size_t(l.idx()._last - l.idx()._first + 1);
      const string surface = l.form();

      // Collect the forms of the (n_sub) real-word lines that compose this MWT.
      // They immediately follow the range line in UD; bail out cleanly on any
      // malformed span rather than over-consuming into the next sentence.
      vector<string> expansion;
      expansion.reserve(n_sub);
      size_t collected = 0;
      while (collected < n_sub && getline(input, line))
      {
        if (line.empty())
        {
          break; // sentence boundary reached too early: malformed span
        }
        if (line[0] == '#')
        {
          continue;
        }
        CoNLLULine w(line);
        if (!w.is_token_line() || w.idx().is_empty())
        {
          continue;
        }
        if (w.idx().is_multiword())
        {
          break; // a nested range here means the corpus is malformed
        }
        expansion.push_back(w.form());
        ++collected;
      }

      if (collected == n_sub && !surface.empty() && expansion.size() >= 2)
      {
        dict[surface][expansion]++;
      }
    }
  }

  size_t written = 0;
  for (const auto& [surface, expansions] : dict)
  {
    const vector<string>* best = nullptr;
    size_t best_count = 0;
    size_t total = 0;
    for (const auto& [exp, count] : expansions)
    {
      total += count;
      if (count > best_count)
      {
        best_count = count;
        best = &exp;
      }
    }
    if (nullptr == best)
    {
      continue;
    }

    if (expansions.size() > 1)
    {
      cerr << "extract_mwt_dict: ambiguous surface \"" << surface << "\" ("
           << expansions.size() << " expansions); keeping the most frequent ("
           << best_count << "/" << total << ")." << endl;
    }

    out << surface << "\t" << best_count;
    for (const auto& w : *best)
    {
      out << "\t" << w;
    }
    out << "\n";
    ++written;
  }

  return written;
}

} // namespace CoNLLU
} // namespace deeplima
