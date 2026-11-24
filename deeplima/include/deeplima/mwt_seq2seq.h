// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_MWT_SEQ2SEQ_H
#define DEEPLIMA_MWT_SEQ2SEQ_H

#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <eigen3/Eigen/Dense>

namespace deeplima
{

/**
 * Character seq2seq fallback of the multiword-token expansion (see
 * MwtLexicon): writes the words of a surface the segmenter flagged as
 * multiword but that neither the dictionary nor the affix rules expand well.
 * Needed where the split is not a concatenation, as in the abugidas: Amharic
 * "ሄደ" = "ሄድ" + "ኧ" splits inside a syllable. For Ethiopic and Tamil, the
 * surface's syllables are first decomposed into consonant + vowel letter,
 * which turns most of these splits into concatenations, and the words are
 * recomposed afterwards.
 *
 * The model: a character embedding, a one-layer BiLSTM encoder, an LSTM cell
 * decoder fed with the previous character and the attention context, a
 * bilinear attention, and a copy mechanism (a learned gate mixes the
 * generated distribution with the attention over the source characters).
 * Greedy decoding; the output is the words separated by spaces. Expansions
 * are cached (up to CACHE_SIZE surfaces), as the same surfaces recur.
 *
 * Trained in Python by deeplima-training-scripts'
 * tokenization/train_mwt_seq2seq.py, which writes <segmenter>.pt.mwt.s2s:
 *   "MWTS2S01", then uint32 decomposed, before_rules, V, E, H,
 *   then V-4 uint32 code points (output ids 4..V-1; 0 pad, 1 bos, 2 eos, 3 unk),
 *   then 15 matrices, each uint32 rows, cols and rows*cols float32 row-major:
 *   embedding (V x E); encoder forward W_ih (4H x E), W_hh (4H x H), bias (4H);
 *   encoder backward, the same; decoder W_ih (8H x (E+2H)), W_hh (8H x 2H),
 *   bias (8H); attention (2H x 2H); output (V x 4H) and its bias (V);
 *   copy gate (1 x (4H+E)) and its bias (1). Biases are PyTorch's b_ih + b_hh,
 *   gates in PyTorch's order (input, forget, cell, output).
 */
class MwtSeq2Seq
{
public:
  typedef Eigen::MatrixXf Matrix;
  typedef Eigen::VectorXf Vector;

  MwtSeq2Seq() = default;

  bool loaded() const { return m_loaded; }
  // Should the lexicon try this model before its affix rules?
  bool before_rules() const { return m_before_rules; }

  void load(const std::string& fn)
  {
    std::ifstream f(fn, std::ios::binary);
    if (!f.is_open())
    {
      throw std::runtime_error("Can't open MWT seq2seq model " + fn);
    }
    char magic[8];
    f.read(magic, 8);
    if (!f || std::memcmp(magic, "MWTS2S01", 8) != 0)
    {
      throw std::runtime_error("Not an MWT seq2seq model: " + fn);
    }
    const uint32_t decomposed = read_u32(f), before = read_u32(f);
    const uint32_t V = read_u32(f), E = read_u32(f), H = read_u32(f);
    if (V < 5 || E == 0 || H == 0 || V > 1000000 || E > 4096 || H > 4096)
    {
      throw std::runtime_error("Bad MWT seq2seq model dimensions in " + fn);
    }
    m_decomposed = decomposed != 0;
    m_before_rules = before != 0;
    m_chars.assign(V, 0);
    m_ids.clear();
    for (uint32_t i = 4; i < V; ++i)
    {
      m_chars[i] = read_u32(f);
      m_ids[m_chars[i]] = i;
    }
    m_emb = read_matrix(f, V, E);
    m_enc_f_wih = read_matrix(f, 4 * H, E);
    m_enc_f_whh = read_matrix(f, 4 * H, H);
    m_enc_f_b = read_matrix(f, 4 * H, 1);
    m_enc_b_wih = read_matrix(f, 4 * H, E);
    m_enc_b_whh = read_matrix(f, 4 * H, H);
    m_enc_b_b = read_matrix(f, 4 * H, 1);
    m_dec_wih = read_matrix(f, 8 * H, E + 2 * H);
    m_dec_whh = read_matrix(f, 8 * H, 2 * H);
    m_dec_b = read_matrix(f, 8 * H, 1);
    m_att = read_matrix(f, 2 * H, 2 * H);
    m_out = read_matrix(f, V, 4 * H);
    m_out_b = read_matrix(f, V, 1);
    m_gate = read_matrix(f, 1, 4 * H + E);
    m_gate_b = read_matrix(f, 1, 1);
    m_H = H;
    m_loaded = true;
    m_cache = std::make_shared<Cache>();
  }

  void clear()
  {
    m_loaded = false;
    m_cache = std::make_shared<Cache>();
  }

  // The words of surface (UTF-8), or an empty vector when the model writes
  // fewer than two (the surface stays as it is).
  std::vector<std::string> expand(const std::string& surface) const
  {
    std::vector<std::string> words;
    if (!m_loaded || surface.empty())
    {
      return words;
    }
    {
      std::lock_guard<std::mutex> lock(m_cache->mutex);
      const auto it = m_cache->words.find(surface);
      if (m_cache->words.end() != it)
      {
        return it->second;
      }
    }
    std::u32string src = utf8_to_u32(surface);
    if (m_decomposed)
    {
      src = decompose(src);
    }
    const std::u32string out = decode(src);
    std::u32string word;
    for (char32_t c : out)
    {
      if (c == U' ')
      {
        if (!word.empty()) words.push_back(finish(word));
        word.clear();
      }
      else
      {
        word.push_back(c);
      }
    }
    if (!word.empty()) words.push_back(finish(word));
    if (words.size() < 2)
    {
      words.clear();
    }
    std::lock_guard<std::mutex> lock(m_cache->mutex);
    if (m_cache->words.size() >= CACHE_SIZE)
    {
      m_cache->words.clear();
    }
    m_cache->words.emplace(surface, words);
    return words;
  }

  // --------------------------------------------------- abugida syllables
  // Tamil consonant + vowel sign -> consonant + pulli + independent vowel
  // (a bare consonant carries the inherent "a"); Ethiopic syllable -> its
  // sixth-order (vowelless) form + the vowel letter of the glottal row.
  static std::u32string decompose(const std::u32string& s)
  {
    std::u32string out;
    for (size_t i = 0; i < s.size(); ++i)
    {
      const char32_t c = s[i];
      const char32_t n = i + 1 < s.size() ? s[i + 1] : 0;
      if (ta_consonant(c))
      {
        out.push_back(c);
        out.push_back(TA_PULLI);
        if (const char32_t v = ta_vowel_of_sign(n))
        {
          out.push_back(v);
          ++i;
        }
        else if (n == TA_PULLI)
        {
          ++i;
        }
        else
        {
          out.push_back(TA_A);
        }
        continue;
      }
      if (et_syllable(c))
      {
        const int order = int(c - 0x1200) % 8;
        if (const char32_t v = et_vowel(order))
        {
          out.push_back(c - order + 5);
          out.push_back(v);
          continue;
        }
      }
      out.push_back(c);
    }
    return out;
  }

  static std::u32string recompose(const std::u32string& s)
  {
    std::u32string out;
    for (size_t i = 0; i < s.size(); ++i)
    {
      const char32_t c = s[i];
      if (ta_consonant(c) && i + 2 < s.size() && s[i + 1] == TA_PULLI)
      {
        const char32_t v = s[i + 2];
        if (v == TA_A)
        {
          out.push_back(c);
          i += 2;
          continue;
        }
        if (const char32_t sign = ta_sign_of_vowel(v))
        {
          out.push_back(c);
          out.push_back(sign);
          i += 2;
          continue;
        }
      }
      if (et_syllable(c) && int(c - 0x1200) % 8 == 5 && i + 1 < s.size())
      {
        const int order = et_order_of_vowel(s[i + 1]);
        if (order >= 0)
        {
          out.push_back(c - 5 + order);
          ++i;
          continue;
        }
      }
      out.push_back(c);
    }
    return out;
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
        out.push_back(0xfffd);
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

  static std::string u32_to_utf8(const std::u32string& s)
  {
    std::string out;
    for (char32_t cp : s)
    {
      if (cp < 0x80)
      {
        out.push_back(char(cp));
      }
      else if (cp < 0x800)
      {
        out.push_back(char(0xc0 | (cp >> 6)));
        out.push_back(char(0x80 | (cp & 0x3f)));
      }
      else if (cp < 0x10000)
      {
        out.push_back(char(0xe0 | (cp >> 12)));
        out.push_back(char(0x80 | ((cp >> 6) & 0x3f)));
        out.push_back(char(0x80 | (cp & 0x3f)));
      }
      else
      {
        out.push_back(char(0xf0 | (cp >> 18)));
        out.push_back(char(0x80 | ((cp >> 12) & 0x3f)));
        out.push_back(char(0x80 | ((cp >> 6) & 0x3f)));
        out.push_back(char(0x80 | (cp & 0x3f)));
      }
    }
    return out;
  }

private:
  static constexpr uint32_t PAD = 0, BOS = 1, EOS = 2, UNK = 3;
  static constexpr size_t CACHE_SIZE = 100000;
  static constexpr char32_t TA_PULLI = 0x0bcd, TA_A = 0x0b85;

  static bool ta_consonant(char32_t c) { return c >= 0x0b95 && c <= 0x0bb9; }

  static char32_t ta_vowel_of_sign(char32_t s)
  {
    switch (s)
    {
      case 0x0bbe: return 0x0b86;
      case 0x0bbf: return 0x0b87;
      case 0x0bc0: return 0x0b88;
      case 0x0bc1: return 0x0b89;
      case 0x0bc2: return 0x0b8a;
      case 0x0bc6: return 0x0b8e;
      case 0x0bc7: return 0x0b8f;
      case 0x0bc8: return 0x0b90;
      case 0x0bca: return 0x0b92;
      case 0x0bcb: return 0x0b93;
      case 0x0bcc: return 0x0b94;
      default: return 0;
    }
  }

  static char32_t ta_sign_of_vowel(char32_t v)
  {
    for (char32_t s = 0x0bbe; s <= 0x0bcc; ++s)
    {
      if (ta_vowel_of_sign(s) == v && v != 0) return s;
    }
    return 0;
  }

  // The Ethiopic syllables, but the glottal row, whose letters are the vowels
  static bool et_syllable(char32_t c)
  {
    return c >= 0x1200 && c <= 0x1357 && !(c >= 0x12a0 && c <= 0x12a7);
  }

  static char32_t et_vowel(int order)
  {
    switch (order)
    {
      case 0: return 0x12a7;
      case 1: return 0x12a1;
      case 2: return 0x12a2;
      case 3: return 0x12a3;
      case 4: return 0x12a4;
      case 6: return 0x12a6;
      default: return 0;
    }
  }

  static int et_order_of_vowel(char32_t v)
  {
    for (int order = 0; order < 8; ++order)
    {
      if (et_vowel(order) == v && v != 0) return order;
    }
    return -1;
  }

  std::string finish(const std::u32string& word) const
  {
    return u32_to_utf8(m_decomposed ? recompose(word) : word);
  }

  static uint32_t read_u32(std::istream& f)
  {
    unsigned char b[4];
    f.read(reinterpret_cast<char*>(b), 4);
    if (!f)
    {
      throw std::runtime_error("Truncated MWT seq2seq model");
    }
    return uint32_t(b[0]) | (uint32_t(b[1]) << 8) | (uint32_t(b[2]) << 16) | (uint32_t(b[3]) << 24);
  }

  static Matrix read_matrix(std::istream& f, uint32_t rows, uint32_t cols)
  {
    const uint32_t r = read_u32(f), c = read_u32(f);
    if (r != rows || c != cols)
    {
      throw std::runtime_error("Unexpected matrix shape in MWT seq2seq model");
    }
    std::vector<float> data(size_t(r) * c);
    f.read(reinterpret_cast<char*>(data.data()), std::streamsize(data.size() * sizeof(float)));
    if (!f)
    {
      throw std::runtime_error("Truncated MWT seq2seq model");
    }
    Matrix m(r, c);
    for (uint32_t i = 0; i < r; ++i)
      for (uint32_t j = 0; j < c; ++j)
        m(i, j) = data[size_t(i) * c + j];
    return m;
  }

  static Vector sigmoid(const Vector& x)
  {
    return (1.0f + (-x.array()).exp()).inverse().matrix();
  }

  // One LSTM step, PyTorch's gate order (input, forget, cell, output)
  static void lstm_step(const Matrix& wih, const Matrix& whh, const Matrix& b,
                        const Vector& x, Vector& h, Vector& c)
  {
    const Eigen::Index n = h.size();
    const Vector g = wih * x + whh * h + b.col(0);
    const Vector i = sigmoid(g.segment(0, n));
    const Vector f = sigmoid(g.segment(n, n));
    const Vector cell = g.segment(2 * n, n).array().tanh().matrix();
    const Vector o = sigmoid(g.segment(3 * n, n));
    c = (f.array() * c.array() + i.array() * cell.array()).matrix();
    h = (o.array() * c.array().tanh()).matrix();
  }

  std::u32string decode(const std::u32string& src) const
  {
    const Eigen::Index T = Eigen::Index(src.size()), H = m_H, E = m_emb.cols();
    const Eigen::Index V = m_emb.rows();
    std::vector<uint32_t> x(src.size());
    for (size_t t = 0; t < src.size(); ++t)
    {
      const auto it = m_ids.find(src[t]);
      x[t] = (m_ids.end() == it) ? UNK : it->second;
    }

    // Encoder: enc.col(t) = [forward h_t; backward h_t]
    Matrix enc(2 * H, T);
    Vector h = Vector::Zero(H), c = Vector::Zero(H);
    for (Eigen::Index t = 0; t < T; ++t)
    {
      lstm_step(m_enc_f_wih, m_enc_f_whh, m_enc_f_b, m_emb.row(x[t]).transpose(), h, c);
      enc.col(t).head(H) = h;
    }
    h.setZero();
    c.setZero();
    for (Eigen::Index t = T - 1; t >= 0; --t)
    {
      lstm_step(m_enc_b_wih, m_enc_b_whh, m_enc_b_b, m_emb.row(x[t]).transpose(), h, c);
      enc.col(t).tail(H) = h;
    }

    // Decoder
    Vector dh = Vector::Zero(2 * H), dc = Vector::Zero(2 * H), ctx = Vector::Zero(2 * H);
    Vector input(E + 2 * H), feat(4 * H), gate_in(4 * H + E);
    uint32_t y = BOS;
    std::u32string out;
    const size_t max_len = 2 * src.size() + 10;
    for (size_t step = 0; step < max_len; ++step)
    {
      const Vector ye = m_emb.row(y).transpose();
      input << ye, ctx;
      lstm_step(m_dec_wih, m_dec_whh, m_dec_b, input, dh, dc);
      const Vector scores = enc.transpose() * (m_att * dh);
      const Vector a = softmax(scores);
      ctx = enc * a;
      feat << dh, ctx;
      const Vector logits = m_out * feat + m_out_b.col(0);
      Vector p = softmax(logits);
      gate_in << feat, ye;
      const float g = 1.0f / (1.0f + std::exp(-((m_gate.row(0) * gate_in)(0) + m_gate_b(0, 0))));
      p *= g;
      for (Eigen::Index t = 0; t < T; ++t)
      {
        p(x[t]) += (1.0f - g) * a(t);
      }
      Eigen::Index k = 0;
      p.maxCoeff(&k);
      if (k == EOS)
      {
        break;
      }
      if (k >= 4 && k < V)
      {
        out.push_back(m_chars[k]);
      }
      else
      {
        // An unknown character: copy the most attended source character
        Eigen::Index j = 0;
        a.maxCoeff(&j);
        out.push_back(src[j]);
      }
      y = uint32_t(k);
    }
    return out;
  }

  static Vector softmax(const Vector& v)
  {
    const Vector e = (v.array() - v.maxCoeff()).exp().matrix();
    return e / e.sum();
  }

  bool m_loaded = false;
  bool m_decomposed = false;
  bool m_before_rules = false;
  Eigen::Index m_H = 0;
  std::vector<char32_t> m_chars;
  std::unordered_map<char32_t, uint32_t> m_ids;
  Matrix m_emb, m_enc_f_wih, m_enc_f_whh, m_enc_f_b, m_enc_b_wih, m_enc_b_whh, m_enc_b_b;
  Matrix m_dec_wih, m_dec_whh, m_dec_b, m_att, m_out, m_out_b, m_gate, m_gate_b;
  // Shared by the copies of a model: they expand the same way
  struct Cache
  {
    std::mutex mutex;
    std::unordered_map<std::string, std::vector<std::string>> words;
  };
  std::shared_ptr<Cache> m_cache = std::make_shared<Cache>();
};

} // namespace deeplima

#endif
