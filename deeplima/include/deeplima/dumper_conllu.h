// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_DUMPER_CONLLU_H
#define DEEPLIMA_DUMPER_CONLLU_H

#include <iostream>

// #include "deeplima/segmentation/impl/segmentation_impl.h"

#include "deeplima/token_type.h"

namespace deeplima
{
namespace dumper
{

struct ConllToken
{
  ConllToken() = default;
  ConllToken(const ConllToken&) = default;
  ConllToken& operator=(const ConllToken&) = default;
  ~ConllToken() = default;

    // ID: Word index, integer starting at 1 for each new sentence; may be a range for multiword tokens; may be a decimal number for empty nodes (decimal numbers can be lower than 1 but must be greater than 0).
  uint32_t id = 0;
    // FORM: Word form or punctuation symbol.
  std::string form = "_";
    // LEMMA: Lemma or stem of word form.
  std::string lemma = "_";
    // UPOS: Universal part-of-speech tag.
  std::string upos = "_";
    // XPOS: Language-specific part-of-speech tag; underscore if not available.
  std::string xpos = "_";
    // FEATS: List of morphological features from the universal feature inventory or from a defined language-specific extension; underscore if not available.
  std::string feats = "_";
    // HEAD: Head of the current word, which is either a value of ID or zero (0).
  int head = 0;
    // DEPREL: Universal dependency relation to the HEAD (root iff HEAD = 0) or a defined language-specific subtype of one.
  std::string deprel = "dep";
    // DEPS: Enhanced dependency graph in the form of a list of head-deprel pairs.
  std::string deps = "_";
    // MISC: Any other annotation.
  std::string misc = "_";

    // Multiword-token range. When mwt_last > 0, this token is the first sub-word
    // of an expanded multiword token, and a UD "id-mwt_last  mwt_surface" range
    // line is emitted just before this token's own line.
  uint32_t mwt_last = 0;
  std::string mwt_surface;
};

std::ostream& operator<<(std::ostream& oss, const ConllToken& token)
{
  if (token.mwt_last > 0)
  {
    // UD multiword-token range line: "N-M\tsurface\t_\t_\t_\t_\t_\t_\t_\t_"
    oss << token.id << "-" << token.mwt_last << "\t"
        << token.mwt_surface
        << "\t_\t_\t_\t_\t_\t_\t_\t_" << std::endl;
  }
  oss << token.id << "\t"
      << token.form << "\t"
      << token.lemma << "\t"
      << token.upos << "\t"
      << token.xpos << "\t"
      << token.feats << "\t"
      << token.head << "\t"
      << token.deprel << "\t"
      << token.deps << "\t"
      << token.misc << std::endl << std::flush;
  return oss;
}


template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v)
{
  out << '[';
  if ( !v.empty() )
  {
    std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
  }
  out << "]";
  return out;
}

bool dfs(int v, std::vector<uint32_t>& heads,  std::vector<int>& color,
         uint32_t& cycle_start, uint32_t& cycle_end)
{
    // std::cerr << "dfs " << v << ", " << heads << ", " << color << ", " << cycle_start << ", " << cycle_end << std::endl;
    color[v] = 1;
    auto u = heads[v];
    // A predicted head can be out of range (>= number of tokens) when the model
    // produces a bad arc; treat it like "no head" so the cycle walk never
    // indexes color[]/heads[] out of bounds.
    if (u == 0 || u >= heads.size())
    {
      color[v] = 2;
      return false;
    }
    if (color[u] == 0) {
        // parent[u] = v;
        if (dfs(u, heads, color, cycle_start, cycle_end))
            return true;
    } else if (color[u] == 1) {
        cycle_end = v;
        cycle_start = u;
        return true;
    }

    color[v] = 2;
    return false;
}

bool find_cycle(std::vector<uint32_t>& heads, uint32_t root)
{
  // std::cerr << "find_cycle " << heads << ", " << root << std::endl;
  uint32_t n = heads.size();
  std::vector<int> color;
  uint32_t cycle_start = std::numeric_limits<uint32_t>::max();
  uint32_t cycle_end = 0;
  color.assign(n, 0);

  for (uint32_t v = 1; v < n; v++)
  {
    if (v == root) continue;
    if (color[v] == 0 && dfs(v, heads, color, cycle_start, cycle_end))
        break;
  }

  if (cycle_start == std::numeric_limits<uint32_t>::max() || cycle_start == root)
  {
    // std::cerr << "Acyclic" << std::endl;
    return false;
  }
  else
  {
    // if (cycle_start > 0 && cycle_start != root)
    //   heads[cycle_start] = root;
    if (cycle_end > 0 && cycle_end != root && cycle_end < n)
    {
      // std::cerr << "Cycle found from " << cycle_start << ", " << cycle_end << " in " << heads << " with root: " << root << std::endl;
      heads[cycle_end] = root;
      return true;
    }
    else
    {
      // std::cerr << "Cycle not found from " << cycle_start << ", " << cycle_end << " in " << heads << " with root: " << root << std::endl;
      return false;
    }
  }
}

class AbstractDumper
{
protected:
  uint64_t m_token_counter;

  inline void increment_token_counter()
  {
    ++m_token_counter;
  }

public:
  virtual void operator()(const std::vector<deeplima::segmentation::token_pos>& tokens, uint32_t len) = 0;

  uint64_t get_token_counter() const
  {
    return m_token_counter;
  }

  AbstractDumper()
    : m_token_counter(0) { }

  virtual ~AbstractDumper() { }

  void reset()
  {
    m_token_counter = 0;
  }
};

class Horizontal : public AbstractDumper
{
public:
  Horizontal()
  {}

  virtual void operator()(const std::vector<deeplima::segmentation::token_pos>& tokens, uint32_t len)
  {
    std::string temp;
    for (size_t i = 0; i < len; i++)
    {
      const char* ptoken = tokens[i].m_pch;
      std::ostringstream s;
      if (temp.size() > 0)
      {
        s << temp;
        temp.clear();
      }

      for (size_t j = 0; j < tokens[i].m_len; j++)
      {
        if (*ptoken == '\r' || *ptoken == '\n' || *ptoken == '\t')
        {
          s << " ";
        }
        else
        {
          s << *ptoken;
        }
        ptoken++;
      }
      std::string str = s.str();
      if (std::string::npos == str.find_first_not_of(' '))
      {
        temp = str;
        continue;
      }
      std::cout << str << " ";

      if (tokens[i].m_flags & token_flags_t::sentence_brk ||
          tokens[i].m_flags & token_flags_t::paragraph_brk)
      {
        // std::cerr << "Horizontal endl" << std::endl;
        std::cout << std::endl;
      }
    }
  }
};

class TokensToConllU : public AbstractDumper
{
public:
  TokensToConllU()
    : m_next_token_idx(1)
  {}

  ~TokensToConllU()
  {
    if (m_next_token_idx > 1)
    {
      // std::cerr << "on destructor" << std::endl;
      std::cout << std::endl;
    }
  }

  virtual void operator()(const std::vector<deeplima::segmentation::token_pos>& tokens, uint32_t len)
  {
    std::string temp;
    for (size_t i = 0; i < len; i++)
    {
      const char* ptoken = tokens[i].m_pch;
      std::ostringstream s;
      if (temp.size() > 0)
      {
        s << temp;
        temp.clear();
      }
      for (size_t j = 0; j < tokens[i].m_len; j++)
      {
        if (*ptoken == '\r' || *ptoken == '\n' || *ptoken == '\t')
        {
          s << " ";
        }
        else
        {
          s << *ptoken;
        }
        ptoken++;
      }
      std::string str = s.str();
      if (std::string::npos == str.find_first_not_of(' '))
      {
        temp = str;
        continue;
      }
      // First sub-word of an expanded multiword token: emit the UD range line
      // "N-M\tsurface\t_..." just before this sub-word's own line, so the
      // tokenization-only output carries MWT (e.g. "28-29  au" then "à"/"le").
      if (tokens[i].m_mwt_len > 0 && nullptr != tokens[i].m_mwt_surface_pch)
      {
        std::string surface;
        const char* psurf = tokens[i].m_mwt_surface_pch;
        for (size_t j = 0; j < tokens[i].m_mwt_surface_len; j++)
        {
          surface += (*psurf == '\r' || *psurf == '\n' || *psurf == '\t') ? ' ' : *psurf;
          psurf++;
        }
        std::cout << m_next_token_idx << "-"
                  << (m_next_token_idx + tokens[i].m_mwt_len - 1) << "\t"
                  << surface << "\t_\t_\t_\t_\t_\t_\t_\t_" << std::endl;
      }
      std::cout << m_next_token_idx << "\t";
      std::cout << str;
      std::cout << "\t_\t_\t_\t_\t";
      std::cout << m_next_token_idx - 1;
      // std::cerr << "TokensToConllU::operator end of token" << std::endl;
      std::cout << "\t_\t_\t_" << std::endl;

      increment_token_counter();

      m_next_token_idx += 1;
      if (tokens[i].m_flags & token_flags_t::sentence_brk ||
          tokens[i].m_flags & token_flags_t::paragraph_brk)
      {
        // std::cerr << "TokensToConllU end of sentence" << std::endl;
        std::cout << std::endl;
        m_next_token_idx = 1;
      }
    }
  }

protected:
  uint32_t m_next_token_idx;
};

class DumperBase
{
public:
  virtual ~DumperBase() = default;
  virtual uint64_t get_token_counter() const = 0;
  virtual void flush() = 0;
  virtual void reset() = 0;
};

template <class I>
class AnalysisToConllU : public DumperBase
{
protected:
  uint64_t m_token_counter;
  uint32_t m_next_token_idx;
  std::vector<ConllToken> m_tokens;
  uint32_t m_root;

  void reset()
  {
    m_token_counter = 0;
  }

  inline void increment_token_counter()
  {
    ++m_token_counter;
  }

  std::vector<std::string> m_class_names;
  std::vector<std::vector<std::string>> m_classes;
  bool m_has_feats;

  size_t m_first_feature_to_print;

public:
  AnalysisToConllU()
    : m_token_counter(0),
      m_next_token_idx(0),
      m_root(0),
      m_has_feats(false),
      m_first_feature_to_print(0)
  {
    // std::cerr << "AnalysisToConllU()" << (void*)this << std::endl;
  }

  virtual ~AnalysisToConllU()
  {
    // std::cerr << "~AnalysisToConllU " << (void*)this << std::endl;
    // if (m_next_token_idx > 1)
    // {
    //   std::cout << std::endl;
    // }
  }

  const std::vector<std::vector<std::string>> &getMClasses() const {
      return m_classes;
  }

  virtual uint64_t get_token_counter() const
  {
    return m_token_counter;
  }

  void set_classes(size_t idx, const std::string& class_name, const std::vector<std::string>& data)
  {
    m_class_names.push_back(class_name);

    if (idx + 1 > m_classes.size())
    {
      m_classes.resize(idx + 1);
    }
    assert(0 == m_classes[idx].size());
    m_classes[idx] = data;

    if (m_classes.size() > 1)
    {
      m_has_feats = true;
      for (size_t i = 0; i < m_class_names.size(); ++i)
      {
        const std::string& feat_name = m_class_names[i];
        if (feat_name == "upos" || feat_name == "xpos" || feat_name == "eos")
        {
          continue;
        }
        m_first_feature_to_print = i;
        break;
      }
    }
  }

  std::string generate_feats(const I& iter)
  {
    std::string feat_str;

    for (size_t i = m_first_feature_to_print; i < m_classes.size(); ++i)
    {
      if (0 != iter.token_class(i))
      {
        if (!feat_str.empty())
        {
          feat_str += "|";
        }

        feat_str += m_class_names[i];
        feat_str += "=";
        feat_str += m_classes[i][iter.token_class(i)];
      }
    }

    return feat_str;
  }

  virtual void flush()
  {
    // Nothing pending: emit no separator. flush() is also called once at EOF
    // (apps/deeplima.cpp) after the last sentence already flushed on its
    // sentence break, so without this guard that final call would append a
    // stray blank line, leaving a doubled blank at end-of-file that strict
    // CoNLL-U scorers read as a trailing empty sentence. A genuinely pending
    // last sentence with no sentence-break flag still has tokens here and is
    // emitted normally.
    if (m_tokens.empty())
    {
      return;
    }
    m_next_token_idx = 1;
    std::vector<uint32_t> heads(m_tokens.size()+1);
    heads[0] = 0;
    for (size_t i = 1; i < heads.size(); i++)
    {
      heads[i] = m_tokens[i-1].head;
    }
    // std::cerr << "AnalysisToConllU::flush() heads before find_cycle: " << heads << std::endl;
    while (find_cycle(heads, m_root))
    {
      // std::cerr << "AnalysisToConllU::flush() heads after cycle found: " << heads << std::endl;
    }
    // std::cerr << "AnalysisToConllU::flush() heads after no more cycle: " << heads << std::endl;
    for (size_t i = 1; i < heads.size(); i++)
    {
      m_tokens[i-1].head = heads[i];
    }
    for (const auto& token: m_tokens)
    {
      // std::cerr << "AnalysisToConllU::flush():567 " << token ;
      std::cout << token ;
    }
    m_tokens.clear();
    // std::cerr << "after clearing tokens. m_next_token_idx=" << m_next_token_idx << std::endl;
    std::cout << std::endl;
    m_root = 0;
  }

  void operator()(I& iter, uint32_t begin, uint32_t end, bool hasDeps = false)
  {
    // std::cerr << "AnalysisToConllU::operator() " << iter.form() << ", " << begin << ", " << end << ", "
    //           << m_next_token_idx << ", " << m_tokens << std::endl;
    m_tokens.reserve(end);
    // Inter-sentence blank lines are emitted by flush() only. When the previous
    // buffer ended exactly on a sentence break, flush() already wrote the
    // separator and reset m_next_token_idx to 1 and m_root to 0; emitting
    // another std::endl here produced *doubled* blank lines between sentences at
    // buffer boundaries, which strict CoNLL-U scorers (conll18) read as empty
    // sentences. So only initialize on the very first call (m_next_token_idx==0).
    if (m_next_token_idx == 0)
    {
      m_next_token_idx = 1;
    }
    std::string temp;
    while (!iter.end())
    {
      const char* ptoken = iter.form();
      if (std::string(ptoken) == "<ROOT>")
      {
        iter.next();
        continue;
      }
      ConllToken token;
      std::ostringstream s;
      if (temp.size() > 0)
      {
        s << temp;
        temp.clear();
      }
      while (0 != *ptoken)
      {
        if (*ptoken == '\r' || *ptoken == '\n' || *ptoken == '\t')
        {
          s << " ";
        }
        else
        {
          s << *ptoken;
        }
        ptoken++;
      }
      std::string str = s.str();
      if (std::string::npos == str.find_first_not_of(' '))
      {
        temp = str;
        continue;
      }
      // std::cerr << m_next_token_idx << "\t" << str << "\t" << iter.lemma()
      //           << "\t" << m_classes[0][iter.token_class(0)] << std::endl;
      token.id = m_next_token_idx;
      token.form = str;
      token.lemma = iter.lemma();
      token.upos = m_classes[0][iter.token_class(0)];
      // First sub-word of an expanded multiword token: record the "N-M  surface"
      // range to emit before this token's line (ids N..M are the sub-words).
      if (iter.mwt_len() > 0)
      {
        token.mwt_last = m_next_token_idx + iter.mwt_len() - 1;
        token.mwt_surface = iter.mwt_surface();
      }
      // std::cout << m_next_token_idx << "\t";
      // std::cout << str << "\t";
      // std::cout << iter.lemma();
      // if (true)
      // {
        // std::cout << "\t" << m_classes[0][iter.token_class(0)];
      // }
      // else
      // {
      //   std::cout << "\t_";
      // }

      // std::cerr << "\t_\t";
      // std::cout << "\t_\t";

      if (m_has_feats)
      {
        // std::cerr << generate_feats(iter);
        // std::cout << generate_feats(iter);
        token.feats = generate_feats(iter);
      }
      else
      {
        // std::cerr << "_";
        // std::cout << "_";
      }

      // std::cerr << "\t";
      // std::cout << "\t";
      if (hasDeps)
      {
        if ((m_root == begin) && (iter.head() == 0))
        {
          m_root = m_next_token_idx;
          token.head = 0;
          token.deprel = "root";
        }
        else if  (iter.head() >= end)
        {
          // std::cerr << "head is out of sentence. rehead to root or set it as root if no root is set " << m_root << "\tdep"<< std::endl;
          if (m_root == 0)
          {
            token.head = 0;
            token.deprel = "root";
            m_root = m_next_token_idx;
          }
          else
          {
            // std::cout << root << "\tdep";
            token.head = m_root;
            token.deprel = "dep";
          }
        }
        else if ((m_root != begin) && (iter.head() == 0))
        {
          // std::cerr << "multiple roots in the sentence. rehead to the first root" << std::endl;
          // std::cerr << m_root << "\tdep";
          // std::cout << root << "\tdep";
          token.head = m_root;
          token.deprel = "dep";
        }
        else if (iter.head() == m_next_token_idx)
        {
          token.head = m_root;
          token.deprel = "dep";
        }
        else
        {
          // std::cerr << iter.head() << "\tdep";
          // std::cout << iter.head() << "\tdep";
          token.head = iter.head();
          // Use the deprel predicted by the label decoder when available
          // (falls back to "dep" if the model has no label decoder).
          token.deprel = iter.deprel();
        }
      }
      else
      {
        // std::cerr << m_next_token_idx - 1 << "\t_";
        // std::cout << m_next_token_idx - 1 << "\t_";
        token.head = m_next_token_idx - 1;
        if (token.head == 0)
          token.deprel = "root";
      }
      // std::cerr << "\t_\t_" << std::endl;
      // std::cout << "\t_\t_" << std::endl;
      m_tokens.push_back(token);
      increment_token_counter();

      m_next_token_idx += 1;
      if (iter.flags() & token_flags_t::sentence_brk ||
          iter.flags() & token_flags_t::paragraph_brk)
      {
        // std::cerr << "AnalysisToConllU::operator() on sent/para break. m_next_token_idx="
        //           << m_next_token_idx << std::endl;
        flush();
      }
      iter.next();
    }
  }
};

} // namespace dumper
} // namespace deeplima

#endif
