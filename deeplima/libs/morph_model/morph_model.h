// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_MORPH_MODEL_H
#define DEEPLIMA_LIBS_MORPH_MODEL_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <cassert>

namespace deeplima
{
namespace morph_model
{

typedef uint64_t feat_base_t;

class morph_feats_t
{
  feat_base_t m_value;

public:

  morph_feats_t()
    : m_value(0) { }
  morph_feats_t(const morph_feats_t& other)
    : m_value(other.m_value) { }
  morph_feats_t(feat_base_t val)
    : m_value(val) { }

  inline void append(feat_base_t val)
  {
    assert((m_value & val) == 0);
    m_value |= val;
  }

  inline bool operator==(const morph_feats_t other) const
  {
    return m_value == other.m_value;
  }

  inline bool operator!=(const morph_feats_t other) const
  {
    return m_value != other.m_value;
  }

  inline bool operator<(const morph_feats_t other) const
  {
    return m_value < other.m_value;
  }

  const feat_base_t& toBaseType() const
  {
    return m_value;
  }

  inline feat_base_t subvalue(feat_base_t mask, uint8_t offset) const
  {
    return (m_value & mask) >> offset;
  }
};

class morph_model_builder;

class morph_model_t
{
  struct dict_t
  {
    std::unordered_map<std::string, size_t> m_key2id;
    std::vector<std::string> m_id2key;

    dict_t() { }
    dict_t(const std::string& str);
    static dict_t from_string(const std::string& str);

    std::string to_string() const;

    const std::vector<std::string>& get_vec_ref() const
    {
      return m_id2key;
    }

    size_t get_id(const std::string& name) const
    {
      auto it = m_key2id.find(name);
      assert(m_key2id.end() != it);
      return it->second;
    }

    const std::string& get_key(size_t id) const
    {
      assert(id < m_id2key.size());
      return m_id2key[id];
    }

    size_t add_key(const std::string& name)
    {
      if (m_key2id.end() == m_key2id.find(name))
      {
        m_id2key.push_back(name);
        m_key2id[name] = m_id2key.size() - 1;
      }
      return get_id(name);
    }

    bool has(const std::string& name) const
    {
      auto it = m_key2id.find(name);
      return m_key2id.end() != it;
    }
  };

  dict_t m_upos_dict;
  dict_t m_feat_dict;

public:
  morph_model_t() { }
  morph_model_t(const std::string& str);
  std::string to_string() const;
  std::string to_string(const morph_feats_t& feats) const;

  inline const std::vector<std::string>& get_feat_vec_ref(size_t feat_id) const
  {
    if (0 == feat_id)
    {
      return m_upos_dict.get_vec_ref();
    }
    return m_feats[feat_id].get_vec_ref();
  }

  size_t get_feats_count() const;
  inline bool has_feat(const std::string& feat_name) const
  {
    return m_feat_dict.has(feat_name);
  }

  inline const std::string& get_feat_name(size_t feat_id) const
  {
    return m_feat_dict.get_key(feat_id);
  }

  inline size_t get_feat_idx(const std::string& feat_name) const
  {
    return m_feat_dict.get_id(feat_name);
  }

  inline feat_base_t decode_upos(const morph_feats_t& feats) const
  {
    return feats.subvalue(m_upos_mask, 0);
  }

  inline feat_base_t decode_feat(const morph_feats_t& feats, size_t feat_id) const
  {
    feat_base_t upos_id = decode_upos(feats);
    assert(upos_id < m_feats2mask.size());
    auto it = m_feats2mask[upos_id].find(feat_id);
    if (m_feats2mask[upos_id].end() == it)
    {
      return feat_base_t(0);
    }
    size_t feat_mask_id = it->second;
    return feats.subvalue(m_masks[feat_mask_id], m_offset[feat_mask_id]);
  }

  size_t get_upos_id(const std::string& name) const
  {
    return m_upos_dict.get_id(name);
  }

protected:
  size_t add_upos(const std::string& name)
  {
    return m_upos_dict.add_key(name);
  }

  size_t get_feat_id(const std::string& name) const
  {
    return m_feat_dict.get_id(name);
  }

  size_t add_feat(const std::string& name)
  {
    return m_feat_dict.add_key(name);
  }

  struct feat_pos_t
  {
    uint8_t m_start;
    uint8_t m_len;

    feat_pos_t()
      : m_start(0), m_len(0) { }
  };

public:
  feat_base_t m_upos_mask;
  // feat_value == ( feat_base & mask ) >> offset
  std::vector<feat_base_t> m_masks; // mask_id -> mask
  std::vector<uint8_t> m_offset;    // mask_id -> offset

  std::vector<std::map<size_t, size_t>> m_feats2mask; // upos_id -> (feat_id -> mask_id)
  std::vector<dict_t> m_feats; // feat_id -> feat_values

public:

  morph_feats_t convert(const std::string& upos, const std::map<std::string, std::set<std::string>>& feats) const;

  const std::string& decode_upos_to_str(const morph_feats_t& feats) const;

  friend class morph_model_builder;
};

} // morph_model
} // deeplima

#endif

