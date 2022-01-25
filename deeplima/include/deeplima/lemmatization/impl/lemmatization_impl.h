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

#ifndef DEEPLIMA_LEMMATIZATION_IMPL_H
#define DEEPLIMA_LEMMATIZATION_IMPL_H

#include "deeplima/token_type.h"
#include "deeplima/eigen_wrp/embd_dict.h"
#include "deeplima/utils/str_index.h"

#include "helpers/path_resolver.h"

namespace deeplima
{
namespace lemmatization
{

class ILemmatization
{
public:
};

namespace impl {

template <class InferenceEngine, class Vectorizer, class Matrix>
class LemmatizationImpl: public ILemmatization, public InferenceEngine
{
public:

  LemmatizationImpl()
    : m_beam_size(5)
  {}

  LemmatizationImpl(
      size_t threads,
      size_t buffer_size_per_thread
    )
    : m_beam_size(5)
  {
  }

  virtual void load(const std::string& fn, const PathResolver& path_resolver)
  {
    InferenceEngine::load(fn);
  }

  void init(size_t max_input_word_len,
            const std::vector<std::string>& class_names,
            const std::vector<std::vector<std::string>>& class_values)
  {
    auto uint_dicts = InferenceEngine::get_uint_dicts();
    decltype(uint_dicts) enc_uint_dict;
    enc_uint_dict.push_back(uint_dicts[0]);
    m_vectorizer.init(enc_uint_dict, max_input_word_len, 1);

    auto str_dicts = InferenceEngine::get_str_dicts();
    auto lang_morph_model = InferenceEngine::get_morph_model();

    assert(class_names.size() == class_values.size());
    EmbdUInt64FloatHolder enc_feats_dict;
    for (size_t feat_idx = 0; feat_idx < lang_morph_model.get_feats_count(); ++feat_idx)
    {
      const std::vector<std::string>& feat_vec = lang_morph_model.get_feat_vec_ref(feat_idx);
      std::vector<uint64_t> v(feat_vec.size(), 0);

      auto it = std::find(class_names.begin(), class_names.end(), lang_morph_model.get_feat_name(feat_idx));

      int cls_idx = -1;
      if (class_names.end() != it)
      {
        cls_idx = it - class_names.begin();

        assert(class_values[cls_idx].size() == feat_vec.size());
        for (size_t j = 0; j < feat_vec.size(); ++j)
        {
          // TODO: rewrite this
          for (size_t k = 0; k < class_values[cls_idx].size(); ++k)
          {
            if (feat_vec[j] == class_values[cls_idx][k]
                || ("_" == feat_vec[j] && "-" == class_values[cls_idx][k]))
            {
              v[k] = j;
              break;
            }
          }
        }
      }
      else
      {
        std::cerr << "Warning: classifier doesn't provide required feature: \""
                  << lang_morph_model.get_feat_name(feat_idx) << "\"" << std::endl;
        v = { 0 };
      }

      std::shared_ptr<Dict<uint64_t>> dd = std::shared_ptr<Dict<uint64_t>>(new Dict<uint64_t>(v));
      EmbdUInt64Float d;

      d.init(dd, str_dicts[feat_idx].get_tensor().transpose());
      enc_feats_dict.push_back(d);

      m_feat2cls.push_back(cls_idx);
    }
    m_feat_vectorizer.init(enc_feats_dict, 1, enc_feats_dict.size());

    InferenceEngine::init_new_worker(max_input_word_len);
  }

  void predict(const std::u32string& form, const StdMatrix<uint8_t>& classes, size_t idx, std::u32string& target)
  {
    // 1. vectorize
    for (size_t i = 0; i < form.size(); ++i)
    {
      m_vectorizer.set(i, 0, form[i]);
    }

    for (size_t i = 0; i < m_feat2cls.size(); ++i)
    {
      if (m_feat2cls[i] >= 0)
      {
        m_feat_vectorizer.set(0, i, classes.get(idx, m_feat2cls[i]));
      }
      else
      {
        m_feat_vectorizer.set(0, i, 0);
      }
    }

    // 2. run prediction
    std::vector<uint32_t> output(0, form.size() * 2);
    InferenceEngine::predict(0,
                             static_cast<const Vectorizer>(m_vectorizer).get_tensor(),
                             static_cast<const Vectorizer>(m_feat_vectorizer).get_tensor(),
                             form.size(),
                             form.size() * 2,
                             m_beam_size,
                             output,
                             { "output" });

    // 3. generate output
    target.clear();
    output.push_back(0);
    target = std::u32string((char32_t*)(output.data()));
  }

  virtual ~LemmatizationImpl()
  {
    std::cerr << "~LemmatizationImpl" << std::endl;
  }

protected:
  size_t m_beam_size;
  Vectorizer m_vectorizer;
  Vectorizer m_feat_vectorizer;
  std::vector<int> m_feat2cls;
};

} // namespace impl
} // namespace lemmatization
} // namespace deeplima

#endif // DEEPLIMA_LEMMATIZATION_IMPL_H
