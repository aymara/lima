// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
