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

#include "morph_model.h"
#include "deeplima/utils/pretty.h"

#include <iostream>

using namespace std;

namespace deeplima
{
namespace morph_model
{

morph_feats_t morph_model_t::convert(const string& upos, const map<string, set<string>>& feats) const
{
  assert(m_upos_dict.has(upos));
  size_t upos_id = m_upos_dict.get_id(upos);
  morph_feats_t v(upos_id);

  const auto& feats2mask = m_feats2mask[upos_id];

  for ( const auto& kv : feats )
  {
    const string& feat_name = kv.first;
    assert(kv.second.size() == 1);
    assert(m_feat_dict.has(feat_name));
    size_t feat_id = m_feat_dict.get_id(feat_name);

    const auto it = feats2mask.find(feat_id);
    assert(feats2mask.end() != it);
    size_t mask_id = it->second;
    assert(mask_id < m_offset.size());
    uint8_t offset = m_offset[mask_id];

    const string& feat_value = *(kv.second.begin());
    size_t value_id = m_feats[feat_id].get_id(feat_value);
    v.append(value_id << offset);
  }

#ifndef NDEBUG
  //cerr << pretty_bits_to_string(v.toBaseType()) << endl;
  const string& decoded_upos = decode_upos_to_str(v);
  assert(upos == decoded_upos);
#endif

  return v;
}

const string& morph_model_t::decode_upos_to_str(const morph_feats_t& feats) const
{
  feat_base_t upos_id = decode_upos(feats);
  return m_upos_dict.get_key(upos_id);
}

size_t morph_model_t::get_feats_count() const
{
  return m_feats.size();
}

morph_model_t::dict_t::dict_t(const string& str)
{
  istringstream iss(str);
  size_t sz = 0;
  iss >> sz;
  string t;
  while (iss >> t)
  {
    add_key(t);
  }
}

morph_model_t::dict_t morph_model_t::dict_t::from_string(const string& str)
{
  dict_t d(str);
  return d;
}

string morph_model_t::dict_t::to_string() const
{
  ostringstream oss;

  oss << m_id2key.size();
  for (const string& v : m_id2key)
  {
    assert(string::npos == v.find(' '));
    assert(string::npos == v.find('\n'));
    oss << " " << v;
  }

  return oss.str();
}

morph_model_t::morph_model_t(const string& str)
{
  istringstream iss(str);
  string t;

  // m_upos_dict
  t.clear();
  while (t.empty())
  {
    getline(iss, t);
  }
  if (t.empty())
  {
    throw;
  }
  m_upos_dict = morph_model_t::dict_t::from_string(t);

  // m_feat_dict
  t.clear();
  while (t.empty())
  {
    getline(iss, t);
  }  if (t.empty())
  {
    throw;
  }
  m_feat_dict = morph_model_t::dict_t::from_string(t);

  // m_upos_mask
  iss >> m_upos_mask;

  // m_masks
  assert(m_masks.size() == 0);
  t.clear();
  while (t.empty())
  {
    getline(iss, t);
  }

  {
    istringstream temp_iss(t);
    feat_base_t v;
    while (temp_iss >> v)
    {
      m_masks.push_back(v);
    }
  }

  // m_offset
  assert(m_offset.size() == 0);
  t.clear();
  while (t.empty())
  {
    getline(iss, t);
  }

  {
    istringstream temp_iss(t);
    int v;
    while (temp_iss >> v)
    {
      if (v < 0 || v > numeric_limits<uint8_t>::max())
      {
        throw;
      }
      uint8_t ui = uint8_t(v);
      m_offset.push_back(ui);
    }
  }

  // m_feats2mask
  assert(m_feats2mask.size() == 0);
  t.clear();
  while (t.empty())
  {
    getline(iss, t);
  }
  do
  {
    istringstream temp_iss(t);
    string p;
    m_feats2mask.resize(m_feats2mask.size() + 1);
    while (temp_iss >> p)
    {
      string::size_type i = p.find('_');
      if (string::npos == i)
      {
        throw;
      }
      p[i] = ' ';
      istringstream p_iss(p);
      size_t a, b;
      p_iss >> a >> b;
      if (m_feats2mask[m_feats2mask.size() - 1].end() != m_feats2mask[m_feats2mask.size() - 1].find(a))
      {
        throw;
      }
      m_feats2mask[m_feats2mask.size() - 1][a] = b;
    }
    getline(iss, t);
  } while (!t.empty());

  // m_feats
  assert(m_feats.size() == 0);
  t.clear();
  while (t.empty())
  {
    getline(iss, t);
  }
  while (!t.empty())
  {
    m_feats.resize(m_feats.size() + 1);
    m_feats[m_feats.size() - 1] = morph_model_t::dict_t::from_string(t);
    getline(iss, t);
  }
}

string morph_model_t::to_string() const
{
  ostringstream oss;

  oss << m_upos_dict.to_string() << endl;
  oss << m_feat_dict.to_string() << endl;
  oss << m_upos_mask << endl;

  // m_masks
  for (size_t i = 0; i < m_masks.size(); ++i)
  {
    if (i > 0)
    {
      oss << " ";
    }
    oss << m_masks[i];
  }
  oss << endl;

  // m_offset
  for (size_t i = 0; i < m_offset.size(); ++i)
  {
    if (i > 0)
    {
      oss << " ";
    }
    oss << int(m_offset[i]);
  }
  oss << endl;

  // m_feats2mask
  for (const map<size_t, size_t>& m : m_feats2mask)
  {
    size_t c = 0;
    for (const pair<size_t, size_t>& kv : m)
    {
      if (c > 0)
      {
        oss << " ";
      }
      oss << kv.first << "_" << kv.second;
      c++;
    }
    oss << endl;
  }
  oss << endl;

  // m_feats
  for (const dict_t& d : m_feats)
  {
    oss << d.to_string() << endl;
  }
  oss << endl;

  return oss.str();
}

std::string morph_model_t::to_string(const morph_feats_t& feats) const
{

  std::vector<std::string> out_strings;
  out_strings.emplace_back(decode_upos_to_str(feats));

  //feat_base_t upos_id = decode_upos(feats);
  for (size_t feat_idx = 1; feat_idx < m_feats.size(); ++feat_idx)
  {
    feat_base_t feat_val = decode_feat(feats, feat_idx);
    out_strings.emplace_back(m_feats[feat_idx].get_key(feat_val));
  }

  ostringstream oss;
  for (size_t i = 0; i < out_strings.size(); ++i)
  {
    oss << " " << out_strings[i];
  }
  return oss.str();
}

} // morph_model
} // deeplima

