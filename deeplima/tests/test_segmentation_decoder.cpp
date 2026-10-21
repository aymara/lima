// Copyright 2026 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// Unit tests for SegmentationDecoder: the tokens it emits for given character
// tags, in particular the last token of the text when the model leaves it
// open (its last character tagged B or I).

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "deeplima/segmentation/impl/segmentation_decoder.h"

using namespace deeplima;
using namespace deeplima::segmentation;
using namespace deeplima::segmentation::impl;

static int g_failures = 0;

#define CHECK(cond, msg)                                        \
  do {                                                          \
    if (!(cond)) {                                              \
      std::cerr << "FAIL: " << (msg) << std::endl;              \
      ++g_failures;                                             \
    }                                                           \
  } while (0)

struct emitted_t
{
  std::string m_form;
  bool m_eos;
};

// Decodes text (ASCII, one tag per byte) in one buffer, then flushes, and
// returns the emitted tokens
static std::vector<emitted_t> decode(const std::string& text, const std::vector<uint8_t>& tags,
                                     bool flush = true)
{
  auto out = std::make_shared< StdMatrix<uint8_t> >();
  out->resize(1);
  (*out)[0] = tags;
  std::vector<uint8_t> len(tags.size(), 1);

  std::vector<emitted_t> emitted;
  SegmentationDecoder decoder(out, len);
  decoder.register_handler([&emitted](const std::vector<token_pos>& tokens, uint32_t n)
  {
    for (uint32_t i = 0; i < n; i++)
    {
      emitted.push_back({ std::string(tokens[i].m_pch, tokens[i].m_len),
                          bool(tokens[i].m_flags & token_flags_t::sentence_brk) });
    }
  });

  const char* pch = text.c_str();
  decoder.decode(&pch, uint32_t(text.size()), 0, tags.size());
  if (flush)
  {
    decoder.flush();
  }
  return emitted;
}

int main()
{
  const uint8_t X = segm_tag_t::X, B = segm_tag_t::B, I = segm_tag_t::I,
                E = segm_tag_t::E, S_EOS = segm_tag_t::S_EOS;

  // Every token closed: flush adds nothing
  {
    auto t = decode("ab cd.", { B, E, X, B, E, S_EOS });
    CHECK(t.size() == 3, "closed tokens: 3 tokens");
    CHECK(t.size() == 3 && t[0].m_form == "ab" && t[1].m_form == "cd" && t[2].m_form == ".",
          "closed tokens: forms");
    CHECK(t.size() == 3 && t[2].m_eos, "closed tokens: '.' ends the sentence");
  }

  // The last token left open: decode keeps it for a next buffer
  {
    auto t = decode("ab cd", { B, E, X, B, I }, false);
    CHECK(t.size() == 1 && t[0].m_form == "ab", "open last token: not emitted by decode");
  }

  // ... and flush emits it, as the end of the last sentence
  {
    auto t = decode("ab cd", { B, E, X, B, I });
    CHECK(t.size() == 2, "open last token: flushed");
    CHECK(t.size() == 2 && t[1].m_form == "cd", "open last token: its form");
    CHECK(t.size() == 2 && t[1].m_eos, "open last token: ends the sentence");
  }

  // Trailing spaces only: nothing to flush
  {
    auto t = decode("ab  ", { B, E, X, X });
    CHECK(t.size() == 1 && t[0].m_form == "ab", "trailing spaces: one token");
  }

  if (g_failures == 0)
  {
    std::cout << "test_segmentation_decoder: all tests passed" << std::endl;
  }
  return g_failures == 0 ? 0 : 1;
}
