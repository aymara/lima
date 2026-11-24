// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SEGMENTATION_IMPL_SEGM_LEXICON_H
#define DEEPLIMA_SEGMENTATION_IMPL_SEGM_LEXICON_H

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace deeplima
{
namespace segmentation
{
namespace impl
{

/**
 * Word list for the segmenter's lexicon feature (ngram_descr_t::lexicon_ngram).
 *
 * For each character, the feature is a 6-bit mask: bit k (k = 0..4) is set
 * when a word of the list of length k + 2 ends at this character, bit 5 when
 * one of length 7 or 8 does. It only looks back (at most MAX_LEN characters),
 * so the streaming reader computes it when it reads the character, with no
 * lookahead; the segmenter's backward RNN carries it to the word's start.
 * Meant for scripts written without spaces (Chinese, Japanese) and for
 * Vietnamese: an external dictionary (CC-CEDICT for Chinese) gives the
 * segmenter the words its treebank lacks (Chinese GSDSimp, a prototype: word
 * F1 89 -> 93).
 *
 * File: UTF-8, one word per line, the first tab-separated field (a count or
 * other columns may follow); words shorter than 2 or longer than MAX_LEN
 * characters are ignored. Words are matched as written, spaces included (the
 * reader sees every character): a Vietnamese "bắt chuyện" matches its text.
 */
class SegmLexicon
{
public:
  static constexpr size_t MAX_LEN = 8;
  static constexpr uint8_t BITS = 6;

  SegmLexicon() = default;

  explicit SegmLexicon(const std::string& fn)
  {
    load(fn);
  }

  void load(const std::string& fn)
  {
    std::ifstream f(fn);
    if (!f.is_open())
    {
      throw std::runtime_error("Can't open the segmenter lexicon " + fn);
    }
    m_words.clear();
    std::string line;
    while (std::getline(f, line))
    {
      if (!line.empty() && '\r' == line.back())
      {
        line.pop_back();
      }
      const std::u32string w = utf8_to_u32(line.substr(0, line.find('\t')));
      if (w.size() >= 2 && w.size() <= MAX_LEN)
      {
        m_words.insert(w);
      }
    }
  }

  size_t size() const
  {
    return m_words.size();
  }

  // The mask of the character history[n-1] (history[0..n-1] = the last n
  // characters read, oldest first, n <= MAX_LEN).
  uint32_t mask(const char32_t* history, size_t n) const
  {
    uint32_t m = 0;
    std::u32string key;
    key.reserve(MAX_LEN);
    for (size_t len = 2; len <= n && len <= MAX_LEN; ++len)
    {
      key.assign(history + n - len, history + n);
      if (m_words.count(key) > 0)
      {
        m |= 1u << (len <= 6 ? len - 2 : 5);
      }
    }
    return m;
  }

  static std::u32string utf8_to_u32(const std::string& s)
  {
    std::u32string out;
    for (size_t i = 0; i < s.size();)
    {
      const unsigned char c = s[i];
      char32_t cp;
      size_t n;
      if (c < 0x80) { cp = c; n = 1; }
      else if ((c >> 5) == 0x6) { cp = c & 0x1f; n = 2; }
      else if ((c >> 4) == 0xe) { cp = c & 0x0f; n = 3; }
      else if ((c >> 3) == 0x1e) { cp = c & 0x07; n = 4; }
      else { cp = 0xfffd; n = 1; }
      if (i + n > s.size())
      {
        break;
      }
      for (size_t k = 1; k < n; ++k)
      {
        cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3f);
      }
      out.push_back(cp);
      i += n;
    }
    return out;
  }

private:
  std::unordered_set<std::u32string> m_words;
};

} // namespace impl
} // namespace segmentation
} // namespace deeplima

#endif
