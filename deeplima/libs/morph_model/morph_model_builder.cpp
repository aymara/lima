// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <string>
#include <map>
#include <iostream>
#include <algorithm>

#include "morph_model_builder.h"
#include "deeplima/utils/pretty.h"

using namespace std;

namespace deeplima
{
namespace morph_model
{

namespace builder
{
void collect_statistics(const CoNLLU::Annotation& annot,
                        map<string, set<string>>& upos,
                        map<string, set<string>>& feats,
                        map<string, set<string>>& feat2upos)
{
  CoNLLU::WordLevelAdapter src(&annot);
  typename CoNLLU::WordLevelAdapter::const_iterator it = src.begin();
  while (src.end() != it)
  {
    while(!(*it).is_word() && src.end() != it)
    {
      it++;
    }
    if (src.end() == it)
    {
      break;
    }

    const string& u = (*it).upos();
    if (upos.end() == upos.find(u))
    {
      upos[u] = set<string>();
    }
    const map<string, set<string>>& f = (*it).feats();

    for ( const auto& kv : f )
    {
      if (!kv.second.empty())
      {
        upos[u].insert(kv.first);
        feat2upos[kv.first].insert(u);
      }
      vector<string> values;
      values.reserve(kv.second.size());
      assert(1 == kv.second.size()); // reader doesn't accept multiple values now
      for ( const string& s : kv.second )
      {
        values.push_back(s);
      }
      sort(values.begin(), values.end());
      string s;
      for ( const string& v : values )
      {
        if (!s.empty())
        {
          s += " ";
        }
        s += v;
      }
      feats[kv.first].insert(s);
    }

    it++;
  }
}

size_t min_bits_for_value(size_t val)
{
  size_t val_plus_one = val + 1;
  for (size_t i = 0; i <= 32; ++i)
  {
    size_t m = 1 << i;
    if (val_plus_one <= m)
    {
      return i;
    }
  }
  throw overflow_error("Value is too big");
}

void build_morph_model_inner(map<string, vector<string>>& upos2feat,
                             vector<string>& feats_by_use_count,
                             const map<string, set<string>>& upos,
                             const map<string, set<string>>& feats,
                             const vector<string>& fixed_feats)
{
  set<string> fixed_feats_set(fixed_feats.begin(), fixed_feats.end());
  size_t bits_per_upos = min_bits_for_value(upos.size());
  size_t max_bits = 0;
  cout << "bits_per_upos == " << bits_per_upos << endl;

  for ( const auto& kv : upos )
  {
    const string& upos_name = kv.first;

    // upos
    upos2feat[upos_name] = { "upos" };
    upos2feat[upos_name].reserve(kv.second.size() + fixed_feats.size() + 1);
    size_t total_bits = bits_per_upos;

    // features applicable to any upos value
    for ( const string& feat_name : fixed_feats )
    {
      auto it = feats.find(feat_name);
      if (feats.end() != it)
      {
        upos2feat[upos_name].push_back(feat_name);
        const set<string>& feat_values = it->second;
        total_bits += min_bits_for_value(feat_values.size());
      }
    }

    cout << "! " << upos_name << " " << total_bits << " bits ";

    // features specific for the given upos_name
    for ( const string& feat_name : feats_by_use_count )
    {
      auto it = feats.find(feat_name);
      assert(feats.end() != it);
      const set<string>& feat_values = it->second;
      if (kv.second.end() != kv.second.find(feat_name)
          && fixed_feats_set.end() == fixed_feats_set.find(feat_name))
      {
        upos2feat[upos_name].push_back(feat_name);
        total_bits += min_bits_for_value(feat_values.size());
        cout << feat_name << "/" << min_bits_for_value(feat_values.size()) << " ";
      }
    }

    cout << endl;
    max_bits = max(max_bits, total_bits);
  }
  cout << "Max bits: " << max_bits << endl;
}

} // namespace builder

morph_model_t morph_model_builder::build(const CoNLLU::Annotation& annotation1,
                                         const CoNLLU::Annotation& annotation2)
{
  map<string, set<string>> upos;      // UPOS -> set of features
  map<string, set<string>> feats;     // feature -> set of values
  map<string, set<string>> feat2upos; // feature -> upos it is used for

  builder::collect_statistics(annotation1, upos, feats, feat2upos);
  builder::collect_statistics(annotation2, upos, feats, feat2upos);

  for ( const auto& kv : feats )
  {
    cerr << kv.first << "\t:";
    for ( const string& s : kv.second )
    {
      cerr << " " << s;
    }
    cerr << endl;
  }
  cerr << endl;

  for ( const auto& kv : upos )
  {
    //cerr << kv.first << endl;
    for ( const string& s : kv.second )
    {
      cerr << kv.first << "\t" << s << endl;
    }
    cerr << endl;
  }

  for ( const auto& kv : feat2upos )
  {
    cerr << kv.first << "\t:";
    for ( const string& s : kv.second )
    {
      cerr << " " << s;
    }
    cerr << endl;
  }
  cerr << endl;

  vector<string> feats_by_use_count;
  feats_by_use_count.reserve(feat2upos.size());
  for ( const auto& kv : feat2upos )
  {
    feats_by_use_count.push_back(kv.first);
  }

  sort(feats_by_use_count.begin(), feats_by_use_count.end(), [&feat2upos](const string& a, const string& b){
    return feat2upos[a].size() > feat2upos[b].size();
  });

  size_t total = 0;
  cerr << "UPOS\t\t" << upos.size() << " " << builder::min_bits_for_value(upos.size()) << endl;
  total += upos.size();
  for ( const string& s : feats_by_use_count )
  {
    cerr << s << "\t" << feat2upos[s].size();
    size_t num_values = /*1 +*/ feats[s].size();
    cerr << "\t" << num_values << " " << builder::min_bits_for_value(num_values);
    total += builder::min_bits_for_value(num_values);

    cerr << endl;
  }
  cout << "Total bits: " << total << endl;

  map<string, vector<string>> upos2feat;
  vector<bool> used(feats_by_use_count.size(), false);
  vector<string> fixed_feats = { "Abbr", "Foreign", "Typo" };
  builder::build_morph_model_inner(upos2feat, feats_by_use_count, upos, feats, fixed_feats);

  morph_model_t model;
  model.add_feat("upos");
  for ( const auto& kv : upos )
  {
    const string& upos_name = kv.first;
    /*size_t upos_id =*/ model.add_upos(upos_name);

    for ( const string& feat_name : kv.second )
    {
      /*size_t feat_id =*/ model.add_feat(feat_name);
    }
  }

  map<pair<feat_base_t, uint8_t>, size_t> masks;
  vector<map<size_t, pair<feat_base_t, uint8_t>>> uposid2featid2mask;
  uposid2featid2mask.resize(upos2feat.size());
  for ( const auto& kv : upos2feat )
  {
    const string& upos_name = kv.first;
    size_t upos_id = model.get_upos_id(upos_name);

    size_t offset = 0;
    for ( const string& feat_name : kv.second )
    {
      size_t width = 0;
      size_t feat_id = model.get_feat_id(feat_name);

      if (feat_name == "upos")
      {
        width = builder::min_bits_for_value(upos.size());
      }
      else
      {
        auto it = feats.find(feat_name);
        assert(feats.end() != it);
        const set<string>& feat_values = it->second;
        width = builder::min_bits_for_value(feat_values.size());
      }
      assert(width > 0);

      feat_base_t mask = ((feat_base_t(1) << width) - 1) << offset;
      assert(mask != 0);
      cout << pretty_bits_to_string(mask) << " " << upos_name << " " << upos_id << " "
           << feat_name << " " << width << " " << offset
           << " " << endl;
      masks[make_pair(mask, offset)] += 1;
      uposid2featid2mask[upos_id][feat_id] = make_pair(mask, offset);

      offset += width;
    }
  }

  size_t upos_width = builder::min_bits_for_value(upos.size());
  model.m_upos_mask = ((feat_base_t(1) << upos_width) - 1);
  model.m_masks.reserve(masks.size());
  model.m_offset.reserve(masks.size());
  for ( const auto& kv : masks )
  {
    feat_base_t mask = kv.first.first;
    uint8_t offset = kv.first.second;
    masks[kv.first] = model.m_masks.size();
    model.m_masks.push_back(mask);
    model.m_offset.push_back(offset);
  }

  model.m_feats2mask.resize(uposid2featid2mask.size());
  for (size_t i = 0; i < uposid2featid2mask.size(); ++i)
  {
    for ( const auto& kv : uposid2featid2mask[i] )
    {
      model.m_feats2mask[i][kv.first] = masks[kv.second];
    }
  }

  model.m_feats.resize(feats.size() + 1);
  for ( const auto& kv : feats )
  {
    const string& feat_name = kv.first;
    const set<string>& feat_values = kv.second;
    size_t feat_id = model.get_feat_id(feat_name);

    size_t empty_code = model.m_feats[feat_id].add_key("_");
    assert(0 == empty_code);
    for ( const string& value : feat_values )
    {
      model.m_feats[feat_id].add_key(value);
    }
  }

  return model;
}

} // morph_model
} // deeplima

