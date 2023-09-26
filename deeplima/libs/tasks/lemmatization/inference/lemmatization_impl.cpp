// Copyright 2002-2023 CEA LIST
// SPDX-FileCopyrightText: 2023 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "deeplima/lemmatization/impl/lemmatization_impl.h"

namespace deeplima::lemmatization::impl {


LemmatizationImpl::LemmatizationImpl()
  : m_beam_size(5),
    m_upos_idx(std::numeric_limits<size_t>::max())
{}

LemmatizationImpl::LemmatizationImpl(
    size_t /*threads*/,
    size_t /*buffer_size_per_thread*/
  )
  : m_beam_size(5),
    m_upos_idx(std::numeric_limits<size_t>::max())
{
}


void LemmatizationImpl::load(const std::string& fn, const PathResolver& /*path_resolver*/)
{
  try
  {
    RnnSeq2Seq::load(fn);
  }
  catch (const std::runtime_error& e)
  {
    std::cerr << "LemmatizationImpl failed to load " << fn << ": " << e.what();
    throw;
  }
}

void LemmatizationImpl::init(size_t max_input_word_len,
          const std::vector<std::string>& class_names,
          const std::vector<std::vector<std::string>>& class_values)
 {
  m_upos_idx = std::numeric_limits<size_t>::max();
  auto uint_dicts = RnnSeq2Seq::get_input_uint_dicts();
  decltype(uint_dicts) enc_uint_dict;
  enc_uint_dict.push_back(uint_dicts[0]);
  m_vectorizer.init(enc_uint_dict, max_input_word_len, 1);

  const auto& str_dicts = RnnSeq2Seq::get_input_str_dicts();
  const auto& lang_morph_model = RnnSeq2Seq::get_morph_model();
  m_fixed_upos = std::vector<bool>(32, false); // TODO: find the number of possible UPOS values
  for (auto& idx : RnnSeq2Seq::get_fixed_upos())
  {
    m_fixed_upos[idx] = true;
  }

  assert(class_names.size() == class_values.size());
  EmbdUInt64FloatHolder enc_feats_dict;
  for (size_t feat_idx = 0; feat_idx < lang_morph_model.get_feats_count(); ++feat_idx)
  {
    std::vector<uint64_t> v;
    auto cls_idx = std::numeric_limits<size_t>::max();

    const auto& feat_name = lang_morph_model.get_feat_name(feat_idx);
    auto it = std::find(class_names.begin(), class_names.end(), feat_name);

    if (class_names.end() != it)
    {
      const auto& feat_vec = lang_morph_model.get_feat_vec_ref(feat_idx);
      auto it_diff = it - class_names.begin();
      if (it_diff != std::numeric_limits<ptrdiff_t>::max())
        cls_idx = it_diff;
      assert(cls_idx != std::numeric_limits<uint64_t>::max());
      assert(cls_idx != std::numeric_limits<size_t>::max());

      v.resize(class_values[cls_idx].size(), 0);
      // assert(class_values[cls_idx].size() == feat_vec.size());
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
                << feat_name << "\"" << std::endl;
    }

    auto dd = std::make_shared<Dict<uint64_t>>(v);
    EmbdUInt64Float d;

    d.init(dd, str_dicts[feat_idx].get_tensor().transpose());
    enc_feats_dict.push_back(d);
    m_feat2cls.push_back(cls_idx);
    if (cls_idx != std::numeric_limits<size_t>::max() && cls_idx < class_names.size() && class_names[cls_idx] == "upos")
    {
      m_upos_idx = cls_idx;
    }
  }
  m_feat_vectorizer.init(enc_feats_dict, 1, enc_feats_dict.size());

  if (m_upos_idx == std::numeric_limits<size_t>::max())
  {
    throw std::logic_error("Underlying classifier doesn't provide UPOS.");
  }

  RnnSeq2Seq::init_new_worker(max_input_word_len);
}

bool LemmatizationImpl::is_fixed(std::shared_ptr< StdMatrix<uint8_t> > classes, size_t idx)
{
  assert(m_upos_idx != std::numeric_limits<size_t>::max());
  auto upos = classes->get(idx, m_upos_idx);
  return m_fixed_upos[upos];
}

morph_model::morph_feats_t LemmatizationImpl::get_morph_feats(std::shared_ptr< StdMatrix<uint8_t> > classes, size_t idx) const
{
  const auto& lang_morph_model = RnnSeq2Seq::get_morph_model();
  std::vector<size_t> feats(lang_morph_model.get_feats_count());

  const auto& feat2cls = m_feat2cls;
  const morph_model::morph_feats_t v = lang_morph_model.convert([idx, &feat2cls, &classes](size_t feat_idx) {
    if (feat_idx == std::numeric_limits<size_t>::max())
    {
      throw std::runtime_error(std::string("get_morph_feats wrong feat_idx: max size_t"));
    }
    else if (feat_idx >= feat2cls.size())
    {
      throw std::runtime_error(std::string("get_morph_feats wrong feat_idx: larger than feat2cls size"));
    }
    assert(feat_idx < feat2cls.size() && feat_idx != std::numeric_limits<size_t>::max());
    auto class_idx = feat2cls[feat_idx];
    if (class_idx == std::numeric_limits<size_t>::max())
      return std::numeric_limits<uint64_t>::max();
    assert(class_idx != std::numeric_limits<uint64_t>::max());
    return classes->get(idx, class_idx);
  });

  return v;
}

void LemmatizationImpl::predict(const std::u32string& form,
              std::shared_ptr< StdMatrix<uint8_t> > classes, size_t idx,
              std::u32string& target)
{
  // 1. vectorize
  for (size_t i = 0; i < form.size(); ++i)
  {
    m_vectorizer.set(i, 0, form[i]);
  }

  for (size_t i = 0; i < m_feat2cls.size(); ++i)
  {
    if (m_feat2cls[i] != std::numeric_limits<size_t>::max())
    {
      m_feat_vectorizer.set(0, i, classes->get(idx, m_feat2cls[i]));
    }
    else
    {
      m_feat_vectorizer.set(0, i, 0);
    }
  }

  // 2. run prediction
  std::vector<uint32_t> output(0, form.size() * 2);
  RnnSeq2Seq::predict(0,
                      static_cast<const EmbdVectorizer>(m_vectorizer).get_tensor(),
                      static_cast<const EmbdVectorizer>(m_feat_vectorizer).get_tensor(),
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



} // namespace impl
 // namespace lemmatization
 // namespace deeplima

