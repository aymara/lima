// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef SRC_INCLUDE_SEGMENTATION_TRAIN_CHAR_DICT_BUILDER_H
#define SRC_INCLUDE_SEGMENTATION_TRAIN_CHAR_DICT_BUILDER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "static_graph/dict_base.h"
#include "static_graph/dict.h"
#include "deeplima/segmentation/impl/char_ngram.h"

namespace deeplima
{
namespace segmentation
{
namespace train
{

template <class InputEncoder>
class DictionaryBuilderImpl
{
public:
  DictionaryBuilderImpl(const std::vector<impl::ngram_descr_t>& ngram_descr)
    : m_input_encoder(ngram_descr),
      m_unk(~0)
  {
    assert(sizeof(uint64_t) >= sizeof(m_input_encoder.get_feat(0)));
  }

  inline uint64_t get_num_features() const
  {
    assert(m_input_encoder.size() < std::numeric_limits<int64_t>::max());
    return m_input_encoder.size();
  }

  DictsHolder process(const std::string& text,
                      uint32_t min_ipm,
                      uint64_t& char_counter)
  {
    std::vector<std::unordered_map<uint64_t, uint64_t>> temp_dicts;
    temp_dicts.resize(m_input_encoder.size());

    char_counter = 0;
    read_string(text, char_counter, temp_dicts);

    DictsHolder dicts;
    dicts.resize(temp_dicts.size());
    for (size_t i = 0; i < temp_dicts.size(); i++)
    {
      if (m_input_encoder.allow_unk(i))
      {
        dicts[i] = std::make_shared<UInt64Dict>(m_unk,
                                                          temp_dicts[i].begin(), temp_dicts[i].end(),
                                                          [char_counter, min_ipm](uint64_t c){
                                                            return ipm(c, char_counter) > min_ipm;
                                                          });
      }
      else
      {
        dicts[i] = std::make_shared<UInt64Dict>(temp_dicts[i].begin(), temp_dicts[i].end());
      }
    }

    // std::cerr << "dicts[0].size() == " << dicts[0]->size() << std::endl;
    //std::cout << dicts[0].to_string();

    return dicts;
  }

  void process(const std::wstring& text, uint32_t min_ipm)
  {
    std::vector<std::unordered_map<uint64_t, uint64_t>> temp_dicts;
    temp_dicts.resize(m_input_encoder.size());

    std::unordered_map<uint64_t, uint64_t> char_dict;
    for (size_t i = 0; i < text.size(); i++)
    {
      uint64_t v = (uint64_t)(text[i]);
      auto it = char_dict.find(v);
      if (char_dict.end() == it)
      {
        char_dict[v] = 1;
      }
      else
      {
        it->second++;
      }
    }

    uint64_t total = text.size();
    DictsHolder dicts;
    dicts.resize(temp_dicts.size());
    dicts[0] = std::make_shared<UInt64Dict>(m_unk, char_dict,
                                            [total, min_ipm](uint64_t c){
                                                       return ipm(c, total) > min_ipm;
                                                     });

    // std::cerr << "dicts[0].size() == " << dicts[0]->size() << std::endl;
  }

protected:

  inline static float ipm(uint64_t count, uint64_t total)
  {
    return float(count * 1000000) / total;
  }

  void read_string(const std::string& text,
                   uint64_t& char_counter,
                   std::vector<std::unordered_map<uint64_t, uint64_t>>& temp_dicts)
  {
    uint32_t pos = 0;
    char_counter = 0;

    while (! m_input_encoder.ready_to_generate())
    {
      m_input_encoder.warmup((const uint8_t*)text.data(), &pos, text.size());
    }

    while (size_t(pos) < text.size())
    {
      if (m_input_encoder.parse((const uint8_t*)text.data(), &pos, text.size()) > 0)
      {
        handle_timepoint(char_counter, temp_dicts);
      }
    }

    char final_spaces[] = " ";
    for (size_t i = 0; i < m_input_encoder.get_lookahead(); i++)
    {
      uint32_t pos = 0;
      if (m_input_encoder.parse((uint8_t*)final_spaces, &pos, 1) > 0)
      {
        handle_timepoint(char_counter, temp_dicts);
      }
      else
      {
        throw std::runtime_error("Something wrong.");
      }
    }
  }

  inline void handle_timepoint(uint64_t& char_counter,
                               std::vector<std::unordered_map<uint64_t, uint64_t>>& temp_dicts)
  {
    for (size_t i = 0; i < m_input_encoder.size(); i++)
    {
      uint64_t v = m_input_encoder.get_feat(i);
      assert(v != m_unk);
      auto it = temp_dicts[i].find(v);
      if (temp_dicts[i].end() == it)
      {
        temp_dicts[i][v] = 1;
      }
      else
      {
        it->second++;
      }
    }

    if (char_counter == std::numeric_limits<uint64_t>::max())
    {
      throw std::overflow_error("Too much characters in training set.");
    }

    char_counter++;
  }

  InputEncoder m_input_encoder;
  uint64_t m_unk;
};

} // namespace train
} // namespace segmentation
} // namespace deeplima

#endif
