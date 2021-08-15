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

#ifndef CONLLU_TREEBANK_H
#define CONLLU_TREEBANK_H

#include "file.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <codecvt>

namespace deeplima
{
namespace CoNLLU
{
  class Annotation;

  class Word
  {

  };

  class Sentence
  {
  public:
    Sentence()
      : m_first_line(0), m_first_token_line(0), m_num_tokens(0) { }
    const Word& get_word(size_t idx) const;
    const Word& get_word(idx_t id) const;

  protected:
    size_t m_first_line;
    size_t m_first_token_line;
    size_t m_num_tokens;

    friend class Annotation;
  };

  class TokenItr
  {
  public:
    size_t length() const;
    size_t pos() const;

  protected:
    Sentence* m_psent;
    size_t m_line_idx;
  };

  class Document
  {
  public:
    void load(const std::string& fn);
    const std::wstring& get_text() const
    {
      return m_text;
    }

    const std::string& get_original_text() const
    {
      return m_original_text;
    }

  protected:
    std::wstring m_text;
    std::string m_original_text;

    friend class Annotation;
  };

  class Annotation
  {
  public:
    struct token_t
    {
      enum token_flags_t : uint8_t
      {
        none = 0x00,
        sentence_brk = 0x01,
        paragraph_brk = 0x02,
        max_flags
      };

      token_t()
        : m_line_idx(0), m_pos(0), m_len(0), m_flags(none) {}
      token_t(size_t line_idx, size_t pos, uint16_t len=0)
        : m_line_idx(line_idx), m_pos(pos), m_len(len), m_flags(none) {}

      inline bool eos() const
      {
        return m_flags & sentence_brk;
      }

      size_t m_line_idx;
      size_t m_pos;
      uint16_t m_len;
      token_flags_t m_flags;
    };

    struct word_t
    {
      word_t()
        : m_line_idx(0), m_token_idx(0), m_flags(token_t::token_flags_t::none) {}
      word_t(size_t line_idx, size_t token_idx)
        : m_line_idx(line_idx), m_token_idx(token_idx), m_flags(token_t::token_flags_t::none) {}

      inline bool eos() const
      {
        return m_flags & token_t::token_flags_t::sentence_brk;
      }

      size_t m_line_idx;
      size_t m_token_idx;
      token_t::token_flags_t m_flags;
    };

    Annotation() : m_pdoc(nullptr) {}

    void load(const std::string& fn);

    const Sentence& get_sentence(size_t idx) const;

    const std::vector<token_t>& get_tokens() const
    {
      return m_tokens;
    }

    const std::vector<word_t>& get_words() const
    {
      return m_words;
    }

    typedef std::vector<token_t>::const_iterator tokens_const_iterator;
    inline tokens_const_iterator tokens_begin() const
    {
      return m_tokens.begin();
    }

    inline tokens_const_iterator tokens_end() const
    {
      return m_tokens.end();
    }

    typedef std::vector<word_t>::const_iterator words_const_iterator;
    inline words_const_iterator words_begin() const
    {
      return m_words.begin();
    }

    inline words_const_iterator words_end() const
    {
      return m_words.end();
    }

    inline const CoNLLULine& get_line(size_t line_idx) const
    {
      assert(line_idx < m_lines.size());
      return m_lines[line_idx];
    }

  protected:
    void rebuild_structure(size_t num_sentences, size_t num_tokens, size_t num_words);
    bool test_structure() const;

    std::vector<CoNLLULine> m_lines;

    Document *m_pdoc;
    std::vector<token_t> m_tokens;
    std::vector<word_t> m_words;
    std::vector<Sentence> m_sentences;

    friend class Treebank;
  };

  class TokenLevelAdapter
  {
    class token_struct
    {
      const Annotation::tokens_const_iterator m_it;
      const CoNLLULine& m_line;

    public:
      token_struct(const Annotation::tokens_const_iterator& it, const CoNLLULine& line)
        : m_it(it), m_line(line)
      {  }

      inline bool eos() const
      {
        return m_it->eos();
      }

      inline bool is_word() const
      {
        return !m_line.idx().is_multiword();
      }

      inline const std::string& form() const
      {
        return m_line.form();
      }
    };

    struct iterator_struct
    {
      Annotation::tokens_const_iterator m_it;
      const Annotation& m_data;
      iterator_struct(Annotation::tokens_const_iterator it, const Annotation& data)
        : m_it(it), m_data(data) { }

      iterator_struct(const iterator_struct& it)
        : m_it(it.m_it), m_data(it.m_data) { }

      inline const token_struct operator*() const
      {
        return token_struct(m_it, m_data.get_line(m_it->m_line_idx));
      }

      inline bool operator==(const iterator_struct& other)
      {
        return m_it == other.m_it;
      }

      inline bool operator!=(const iterator_struct& other)
      {
        return m_it != other.m_it;
      }

      inline iterator_struct operator++(int)
      {
        iterator_struct rv = *this;
        m_it++;
        return rv;
      }
    };
  public:
    typedef Annotation data_t;
    typedef token_struct token_t;
    typedef iterator_struct const_iterator;

    TokenLevelAdapter(const Annotation* p)
      : m_data(p) {}

    const_iterator begin() const
    {
      assert(nullptr != m_data);
      return iterator_struct(m_data->tokens_begin(), *m_data);
    }

    const_iterator end() const
    {
      assert(nullptr != m_data);
      return iterator_struct(m_data->tokens_end(), *m_data);
    }

    const Annotation& data() const
    {
      assert(nullptr != m_data);
      return *m_data;
    }

    size_t size() const
    {
      assert(nullptr != m_data);
      return m_data->get_tokens().size();
    }

  protected:
    const Annotation* m_data;
  };

  class WordLevelAdapter
  {
    class word_struct
    {
      const Annotation::words_const_iterator m_it;
      const CoNLLULine& m_line;

    public:
      word_struct(const Annotation::words_const_iterator& it, const CoNLLULine& line)
        : m_it(it), m_line(line)
      {  }

      inline bool eos() const
      {
        return m_it->eos();
      }

      inline bool is_word() const
      {
        return m_line.idx().is_real_word();
      }

      inline const std::string& form() const
      {
        if ("_" == m_line.form())
        {
          return m_line.lemma();
        }
        return m_line.form();
      }

      inline const std::string& upos() const
      {
        return m_line.upos();
      }

      inline const std::string& xpos() const
      {
        return m_line.xpos();
      }

      inline const std::string& deprel() const
      {
        return m_line.deprel();
      }

      inline const std::map<std::string, std::set<std::string>>& feats() const
      {
        return m_line.feats();
      }
    };

    struct iterator_struct
    {
      Annotation::words_const_iterator m_it;
      const Annotation& m_data;
      iterator_struct(Annotation::words_const_iterator it, const Annotation& data)
        : m_it(it), m_data(data) { }

      iterator_struct(const iterator_struct& it)
        : m_it(it.m_it), m_data(it.m_data) { }

      inline const word_struct operator*() const
      {
        return word_struct(m_it, m_data.get_line(m_it->m_line_idx));
      }

      inline bool operator==(const iterator_struct& other)
      {
        return m_it == other.m_it;
      }

      inline bool operator!=(const iterator_struct& other)
      {
        return m_it != other.m_it;
      }

      inline iterator_struct operator++(int)
      {
        iterator_struct rv = *this;
        m_it++;
        return rv;
      }
    };
  public:
    typedef Annotation data_t;
    typedef word_struct token_t;
    typedef iterator_struct const_iterator;

    WordLevelAdapter(const Annotation* p)
      : m_data(p) {}

    const_iterator begin() const
    {
      assert(nullptr != m_data);
      return iterator_struct(m_data->words_begin(), *m_data);
    }

    const_iterator end() const
    {
      assert(nullptr != m_data);
      return iterator_struct(m_data->words_end(), *m_data);
    }

    const Annotation& data() const
    {
      assert(nullptr != m_data);
      return *m_data;
    }

    size_t size() const
    {
      assert(nullptr != m_data);
      return m_data->get_words().size();
    }

  protected:
    const Annotation* m_data;
  };

  class Treebank
  {
    struct AnnotatedDocument
    {
      Document doc;
      Annotation annot;
    };

  public:
    explicit Treebank(const std::string& path)
    {
      if (!load(path))
      {
        throw;
      }
    }

    const Document& get_doc(const std::string& name) const
    {
      auto it = m_parts.find(name);
      if (m_parts.end() != it)
      {
        return it->second.doc;
      }
      throw;
    }

    const Annotation& get_annot(const std::string& name) const
    {
      auto it = m_parts.find(name);
      if (m_parts.end() != it)
      {
        return it->second.annot;
      }
      throw;
    }

  protected:
    bool load(const std::string& path);
    bool parse_ud_file_name(const std::string& fn, std::map<std::string, std::string>& fields);

    std::map<std::string, AnnotatedDocument> m_parts;
  };

} // namespace CoNLLU
} // namespace deeplima

#endif
