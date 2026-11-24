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

#include "deeplima/mwt_lexicon.h"
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
 * The lexicon is the one produced by `deeplima-mwt-dict` / extract_mwt_dict
 * (see deeplima/mwt_lexicon.h): a dictionary of the training surfaces, then
 * affix rules learned from it. A flagged surface missing from the dictionary
 * is expanded by the best matching rule (productive clitics: Polish "-em",
 * Arabic "و"+, Tamil fused endings). Rules are never used without the
 * segmenter's flag. Non-concatenative MWTs no rule reproduces would need a
 * seq2seq fallback, not implemented here.
 */
class MwtExpander
{
public:
  typedef std::function<void(const std::vector<segmentation::token_pos>&, uint32_t)> callback_t;

  explicit MwtExpander(const std::string& dict_fn)
    : m_lexicon(dict_fn)
  {
  }

  void register_handler(const callback_t& fn)
  {
    m_next = fn;
  }

  size_t size() const
  {
    return m_lexicon.size();
  }

  size_t rules_size() const
  {
    return m_lexicon.rules_size();
  }

  bool has_seq2seq() const
  {
    return m_lexicon.has_seq2seq();
  }

  // When true (the default), only surface tokens the segmenter flagged as
  // multiword (token_flags_t::multiword) are expanded — the model decides, in
  // context, which ambiguous forms (e.g. French "des") are contractions. Set
  // false for a non-MWT-aware tokenizer to fall back to expanding every dict
  // match (the original dictionary-only behaviour).
  void set_require_flag(bool require_flag)
  {
    m_require_flag = require_flag;
  }

  void operator()(const std::vector<segmentation::token_pos>& tokens, uint32_t len)
  {
    m_out.clear();
    m_storage.clear();

    for (uint32_t i = 0; i < len; ++i)
    {
      const segmentation::token_pos& t = tokens[i];
      const bool flagged = (t.m_flags & token_flags_t::multiword);
      const std::string surface(t.m_pch, t.m_len);
      // When gating on the model, only flagged surfaces are expanded, from the
      // dictionary then by the rules; otherwise, every dictionary match.
      if ((m_require_flag && !flagged) || !m_lexicon.expand(surface, m_words, m_require_flag)
          || m_words.size() < 2)
      {
        // Not a multiword token: pass through unchanged.
        m_out.push_back(t);
        continue;
      }

      const std::vector<std::string>& words = m_words;

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
        // sentence/paragraph break of the surface attaches to the last. The
        // multiword flag belongs to the surface only, never to its sub-words.
        sub.m_offset = (0 == k) ? t.m_offset : 0;
        sub.m_flags = (k + 1 == words.size())
                          ? token_flags_t(t.m_flags & ~token_flags_t::multiword)
                          : token_flags_t::none;
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
  MwtLexicon m_lexicon;
  callback_t m_next;
  bool m_require_flag = true;

  // Scratch reused on every call.
  std::vector<std::string> m_words;
  std::vector<segmentation::token_pos> m_out;
  std::deque<std::string> m_storage; // owns sub-word/surface bytes; pointer-stable
};

} // namespace deeplima

#endif
