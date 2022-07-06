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
  int m_upos;
  int m_xpos;
  int m_eos;
  int m_rel;
  bool m_feats;

  void add_feature(const std::string& name)
  {
    m_feat2idx[name] = m_idx2feat.size();
    m_idx2feat.push_back(name);
  }

public:
  ConlluFeatExtractor(const std::string& feats_to_train)
    : m_upos(-1),
      m_xpos(-1),
      m_eos(-1),
      m_rel(-1),
      m_feats(false)
  {
    for (const std::string& s : utils::split(feats_to_train, ','))
    {
      assert(s.size() > 0);

      if (s == "upos")
      {
        add_feature("upos");
        m_upos = m_feat2idx["upos"];
      }
      else if (s == "xpos")
      {
        add_feature("xpos");
        m_xpos = m_feat2idx["xpos"];
      }
      else if (s == "feats")
      {
        m_feats = true;
      }
      else if (s == "eos")
      {
        add_feature("eos");
        m_eos = m_feat2idx["eos"];
      }
      else if (s == "rel")
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

  inline std::string feat_value(const Token& token, int feat_no) const
  {
    assert(feat_no >= 0);

    if (-1 != m_upos && m_upos == feat_no)
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
