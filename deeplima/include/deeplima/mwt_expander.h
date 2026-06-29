// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_MWT_EXPANDER_H
#define DEEPLIMA_MWT_EXPANDER_H

#include <cstdint>
#include <deque>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "deeplima/segmentation/impl/segmentation_decoder.h"
#include "deeplima/token_type.h"

namespace deeplima
{

/**
 * Multiword-token (MWT) expansion stage. Sits between the tokenizer and the
 * TokenSequenceAnalyzer in the inference pipeline: each surface token that
 * matches a dictionary entry (e.g. French "du") is replaced by its syntactic
 * sub-words ("de", "le") *before* tagging/parsing. The tagger and parser are
 * trained on words (de=ADP/case, le=DET/det), so an unsplit surface token is
 * out-of-distribution; expanding here keeps the neural models in-distribution
 * and lets the CoNLL-U output carry the UD "N-M  surface" range line.
 *
 * The dictionary is the one produced by `deeplima-mwt-dict` / extract_mwt_dict:
 *   surface <TAB> count <TAB> word1 <TAB> word2 [<TAB> ...]
 *
 * This is the dictionary-only path (closed-class, concatenative MWTs: French,
 * German/Italian/Iberian contractions). Productive or non-concatenative MWTs
 * (Hebrew/Arabic) would need a seq2seq fallback, not implemented here.
 */
class MwtExpander
{
public:
  typedef std::function<void(const std::vector<segmentation::token_pos>&, uint32_t)> callback_t;

  explicit MwtExpander(const std::string& dict_fn)
  {
    load(dict_fn);
  }

  void register_handler(const callback_t& fn)
  {
    m_next = fn;
  }

  size_t size() const
  {
    return m_dict.size();
  }

  void operator()(const std::vector<segmentation::token_pos>& tokens, uint32_t len)
  {
    m_out.clear();
    m_storage.clear();

    for (uint32_t i = 0; i < len; ++i)
    {
      const segmentation::token_pos& t = tokens[i];
      const std::string surface(t.m_pch, t.m_len);
      const auto it = m_dict.find(surface);
      if (m_dict.end() == it)
      {
        // Not a known multiword token: pass through unchanged.
        m_out.push_back(t);
        continue;
      }

      const std::vector<std::string>& words = it->second;

      // Own the surface bytes for the range line. std::deque keeps element
      // pointers/references valid across push_back, so m_pch stays valid until
      // the downstream analyzer has interned everything (it does so
      // synchronously inside m_next, before we clear m_storage on the next call).
      m_storage.push_back(surface);
      const char* surf_pch = m_storage.back().c_str();

      for (size_t k = 0; k < words.size(); ++k)
      {
        m_storage.push_back(words[k]);

        segmentation::token_pos sub;
        sub.m_pch = m_storage.back().c_str();
        sub.m_len = uint16_t(words[k].size());
        // The surface's leading whitespace attaches to the first sub-word; a
        // sentence/paragraph break of the surface attaches to the last.
        sub.m_offset = (0 == k) ? t.m_offset : 0;
        sub.m_flags = (k + 1 == words.size()) ? t.m_flags : token_flags_t::none;
        if (0 == k)
        {
          sub.m_mwt_len = uint8_t(words.size());
          sub.m_mwt_surface_pch = surf_pch;
          sub.m_mwt_surface_len = uint16_t(surface.size());
        }
        m_out.push_back(sub);
      }
    }

    if (m_next)
    {
      m_next(m_out, uint32_t(m_out.size()));
    }
  }

private:
  void load(const std::string& fn)
  {
    std::ifstream f(fn);
    if (!f.is_open())
    {
      throw std::runtime_error("MwtExpander: can't open dictionary \"" + fn + "\"");
    }

    std::string line;
    while (std::getline(f, line))
    {
      if (line.empty() || '#' == line[0])
      {
        continue;
      }

      // surface <TAB> count <TAB> word1 <TAB> word2 ...
      std::vector<std::string> fields;
      size_t start = 0;
      for (;;)
      {
        const size_t tab = line.find('\t', start);
        fields.push_back(line.substr(start, std::string::npos == tab ? std::string::npos : tab - start));
        if (std::string::npos == tab)
        {
          break;
        }
        start = tab + 1;
      }

      if (fields.size() < 4)
      {
        // need surface, count, and at least 2 words for a real expansion
        continue;
      }
      const std::string& surface = fields[0];
      std::vector<std::string> words(fields.begin() + 2, fields.end());
      if (surface.empty() || words.size() < 2)
      {
        continue;
      }
      m_dict.emplace(surface, std::move(words));
    }
  }

  std::unordered_map<std::string, std::vector<std::string>> m_dict;
  callback_t m_next;

  // Scratch reused on every call.
  std::vector<segmentation::token_pos> m_out;
  std::deque<std::string> m_storage; // owns sub-word/surface bytes; pointer-stable
};

} // namespace deeplima

#endif
