// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_FEAT_EXTRACTORS_H
#define DEEPLIMA_FEAT_EXTRACTORS_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "deeplima/utils/split_string.h"

namespace deeplima
{

template <typename Token>
class TokenStrFeatExtractor
{
protected:
  inline static std::string to_lower(const std::string& src)
  {
    std::string copy = src;
    std::transform(copy.begin(), copy.end(), copy.begin(),
      [](unsigned char c){ return std::tolower(c); });
    return copy;
  }

public:
  TokenStrFeatExtractor(const std::string& init_string = "")
  {
  }

  inline static bool needs_preprocessing()
  {
    return false;
  }

  inline void preprocess(const Token& token)
  {
  }

  inline static size_t get_feat_id(const std::string& name)
  {
    if (name == "form") return 0;
    if (name == "lc(form)") return 1;
    throw std::runtime_error("Unknown feature name.");
  }

  inline static std::string feat_value(const Token& token, size_t feat_no)
  {
    switch (feat_no)
    {
    case 0: // form as is
      return token.form();
    case 1: // form in lower case
      return to_lower(token.form());
    default:
      throw std::runtime_error("Unknown feature id.");
    }
  }

  inline bool should_preprocess_feature(int feat_no) const
  {
    return true;
  }

  inline static size_t size()
  {
    return 2;
  }
};

template <typename Token>
class TokenUIntFeatExtractor
{
public:
  inline static size_t get_feat_id(const std::string& name)
  {
    if (name == "eos") return 0;
    throw std::runtime_error("Unknown feature name.");
  }

  inline static uint64_t feat_value(const Token& token, size_t feat_no)
  {
    switch (feat_no)
    {
    case 0: // form as is
      return token.eos();
    default:
      throw std::runtime_error("Unknown feature id.");
    }
  }

  inline static size_t size()
  {
    return 1;
  }
};

template <typename Token>
class ConlluFeatExtractor
{
protected:
  std::vector<std::string> m_idx2feat;
  std::unordered_map<std::string, size_t> m_feat2idx;
  std::unordered_set<std::string> m_prohibited_feats;
  std::unordered_set<int> m_dont_preprocess;
  int m_form = -1;
  int m_form_lc = -1;
  int m_upos = -1;
  int m_xpos = -1;
  int m_eos = -1;
  int m_rel = -1;
  bool m_feats = false;

  void add_feature(const std::string& name)
  {
    m_feat2idx[name] = m_idx2feat.size();
    m_idx2feat.push_back(name);
  }

public:

  ConlluFeatExtractor() = default;

  ConlluFeatExtractor(const ConlluFeatExtractor& other) = default;

  ConlluFeatExtractor(const std::string& feats_to_train)
  {
    for (const std::string& s : utils::split(feats_to_train, ','))
    {
      assert(s.size() > 0);

      bool dont_preprocess = false;
      std::string feat_name = s;
      if (s[0] == '*')
      {
        dont_preprocess = true;
        feat_name = s.substr(1);
      }

      if (feat_name == "form")
      {
        add_feature(feat_name);
        m_form = m_feat2idx[feat_name];
      }
      else if (feat_name == "lc(form)")
      {
        add_feature(feat_name);
        m_form_lc = m_feat2idx[feat_name];
      }
      else if (feat_name == "upos")
      {
        add_feature("upos");
        m_upos = m_feat2idx["upos"];
      }
      else if (feat_name == "xpos")
      {
        add_feature("xpos");
        m_xpos = m_feat2idx["xpos"];
      }
      else if (feat_name == "feats")
      {
        m_feats = true;
      }
      else if (feat_name == "eos")
      {
        add_feature("eos");
        m_eos = m_feat2idx["eos"];
      }
      else if (feat_name == "rel")
      {
        add_feature("rel");
        m_rel = m_feat2idx["rel"];
      }
      else if (s[0] == '-')
      {
        std::string feat_name = s.substr(1);
        assert(feat_name.size() > 0);
        m_prohibited_feats.insert(feat_name);
      }
      else
      {
        throw std::invalid_argument("Can't parse list of features: \"" + feats_to_train + "\"");
      }

      if (dont_preprocess)
      {
        m_dont_preprocess.insert(m_feat2idx[s.substr(1)]);
      }
    }
  }

  inline static bool needs_preprocessing()
  {
    return true;
  }

  inline void preprocess(const Token& token)
  {
    if (m_feats)
    {
      for (const auto& fv : token.feats())
      {
        if (m_prohibited_feats.end() != m_prohibited_feats.find(fv.first))
        {
          continue;
        }
        if (m_feat2idx.end() == m_feat2idx.find(fv.first))
        {
          m_feat2idx[fv.first] = m_idx2feat.size();
          m_idx2feat.push_back(fv.first);
        }
      }
    }
  }

  inline size_t get_feat_id(const std::string& name) const
  {
    const auto it = m_feat2idx.find(name);
    if (m_feat2idx.cend() == it)
    {
      throw std::runtime_error("Unknown feature name.");
    }
    return it->second;
  }

  inline std::string feat_value(const Token& token, int feat_no) const
  {
    assert(feat_no >= 0);

    if (-1 != m_form && m_form == feat_no)
    {
      return TokenStrFeatExtractor<Token>::feat_value(token, 0);
    }
    else if (-1 != m_form && m_form == feat_no)
    {
      return TokenStrFeatExtractor<Token>::feat_value(token, 1);
    }
    else if (-1 != m_upos && m_upos == feat_no)
    {
      return token.upos();
    }
    else if (-1 != m_xpos && m_xpos == feat_no)
    {
      return token.xpos();
    }
    else if (-1 != m_rel && m_rel == feat_no)
    {
      return token.deprel();
    }
    else if (-1 != m_eos && m_eos == feat_no)
    {
      return token.eos() ? "Yes" : "No";
    }
    else if (m_feats)
    {
      assert(size_t(feat_no) < m_idx2feat.size());
      const std::string feat_name = m_idx2feat[feat_no];
      const auto& fv = token.feats();
      auto it = fv.find(feat_name);
      if (fv.end() == it)
      {
        return "-";
      }
      assert(!it->second.empty());
      assert(it->second.size() == 1);
      return *(it->second.begin());
    }

    throw std::invalid_argument("Unknown feature identifier");
  }

  inline bool should_preprocess_feature(int feat_no) const
  {
    return m_dont_preprocess.cend() == m_dont_preprocess.find(feat_no);
  }

  inline size_t size() const
  {
    return m_idx2feat.size();
  }

  std::vector<std::string> feats() const
  {
    return m_idx2feat;
  }
};

} // namespace deeplima

#endif
