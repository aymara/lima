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

#ifndef CONLLU_FILE_H
#define CONLLU_FILE_H

#include "line.h"

namespace deeplima
{
namespace CoNLLU
{
  class CoNLLUFile
  {
  public:
    CoNLLUFile() { }
    explicit CoNLLUFile(const std::string& filename)
    {
      load(filename);
    }

    void load(const std::string& filename);
    void save(const std::string& filename, bool force = false);

    struct sentence_t
    {
      sentence_t() : _first_line(0), _first_word_line(0), _length(0) { }
      sentence_t(size_t first_line, size_t first_word_line, size_t length)
        : _first_line(first_line), _first_word_line(first_word_line), _length(length) { }

      size_t _first_line;
      size_t _first_word_line;
      size_t _length;           // number of words
    };

  protected:

    std::vector<CoNLLULine> _lines;
    std::vector<sentence_t> _sentences;
  };

} // namespace CoNLLU
} // namespace deeplima

#endif
