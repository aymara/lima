// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <string>
#include <unordered_map>
#include <algorithm>

#include "conllu/treebank.h"

#include "train_lemmatization.h"

#include "morph_model/morph_model_builder.h"

#include "deeplima/utils/str_index.h"
#include "static_graph/dict.h"
#include "deeplima/torch_wrp/torch_matrix.h"
#include "tasks/lemmatization/model/seq2seq_for_lemmatization.h"

#include <boost/functional/hash.hpp>
#include "deeplima/utils/backport.h"
#include "deeplima/utils/split_string.h"

using namespace std;
using namespace deeplima::morph_model;
using namespace deeplima::nets;

namespace deeplima
{
namespace lemmatization
{
namespace train
{

struct form_morph_t
{
  StringIndex::idx_t m_str_id;
  morph_feats_t m_feats;

  form_morph_t()
    : m_str_id(-1), m_feats(0) { }
  form_morph_t(const form_morph_t& other)
    : m_str_id(other.m_str_id), m_feats(other.m_feats) { }
  form_morph_t(StringIndex::idx_t str_id, morph_feats_t feats)
    : m_str_id(str_id), m_feats(feats) { }
};

inline bool operator==(const form_morph_t& a, const form_morph_t& b)
{
  return (a.m_str_id == b.m_str_id) && (a.m_feats == b.m_feats);
}

struct form_morph_hash
{
  std::size_t operator()(form_morph_t const& a) const noexcept
  {
    size_t h = std::hash<StringIndex::idx_t>{}(a.m_str_id);
    boost::hash_combine(h, a.m_feats.toBaseType());
    return h;
  }
};

typedef unordered_map<form_morph_t, unordered_map<StringIndex::idx_t, size_t>, form_morph_hash> form2lemma_t;

const char32_t START = 0x10FFFE;
const char32_t EOS = 0x10FFFF; // End of Supplementary Private Use Area-B

DictsHolder build_char_dicts(const form2lemma_t& form2lemma, StringIndex& str_idx)
{
  DictsHolder d;

  unordered_map<char32_t, uint64_t> temp_encoder_dict;
  unordered_map<char32_t, uint64_t> temp_decoder_dict;

  for ( const auto& kv : form2lemma )
  {
    const form_morph_t& enc_form = kv.first;
    const unordered_map<StringIndex::idx_t, size_t>& dec_forms = kv.second;

    const u32string& form = str_idx.get_ustr(enc_form.m_str_id);
    for (const char32_t ch : form)
    {
      temp_encoder_dict[ch] += 1;
    }

    for ( const auto& dec_kv : dec_forms )
    {
      const u32string& lemma = str_idx.get_ustr(dec_kv.first);
      for (const char32_t ch : lemma)
      {
        temp_decoder_dict[ch] += 1;
      }
    }
  }

  d.resize(2);
  d[0] = shared_ptr<Char32Dict>(new Char32Dict(0, EOS,
                                 temp_encoder_dict.begin(), temp_encoder_dict.end(),
                                 [](uint64_t c) {
                                     return c > 1;
                                  })
                                );

  d[1] = shared_ptr<Char32Dict>(new Char32Dict(0, EOS, START,
                                 temp_decoder_dict.begin(), temp_decoder_dict.end(),
                                 [](uint64_t c) {
                                     return c > 1;
                                  })
                                );

  return d;
}

set<morph_model::feat_base_t> find_fixed(const morph_model::morph_model_t& lang_morph_model,
                const form2lemma_t& form2lemma,
                /*const*/ StringIndex& str_idx // TODO: str_idx must be const here
                )
{
  set<morph_model::feat_base_t> fixed_upos, non_fixed_upos;
  for ( const auto& kv : form2lemma )
  {
    const form_morph_t& form = kv.first;
    const unordered_map<StringIndex::idx_t, size_t>& dec_forms = kv.second;

    assert(!dec_forms.empty());
    if (dec_forms.size() > 1 || kv.first.m_str_id != dec_forms.begin()->first)
    {
      morph_model::feat_base_t upos = lang_morph_model.decode_upos(form.m_feats);
      non_fixed_upos.insert(upos);
      if (lang_morph_model.decode_upos_to_str(morph_model::morph_feats_t(upos)) == "PUNCT"
          || lang_morph_model.decode_upos_to_str(morph_model::morph_feats_t(upos)) == "X"
          || lang_morph_model.decode_upos_to_str(morph_model::morph_feats_t(upos)) == "SYM")
      {
        cout << str_idx.get_str(form.m_str_id)
             << " -> " << str_idx.get_str(dec_forms.begin()->first) << endl;
      }
    }
  }

  for ( const auto& kv : form2lemma )
  {
    const form_morph_t& form = kv.first;
    const unordered_map<StringIndex::idx_t, size_t>& dec_forms = kv.second;

    if (1 == dec_forms.size() && kv.first.m_str_id == dec_forms.begin()->first)
    {
      morph_model::feat_base_t upos = lang_morph_model.decode_upos(form.m_feats);
      if (non_fixed_upos.end() == non_fixed_upos.find(upos))
      {
        fixed_upos.insert(upos);
      }
    }
  }

  for ( const auto k : fixed_upos )
  {
    cout << "Fixed UPOS: " << lang_morph_model.decode_upos_to_str(morph_model::morph_feats_t(k)) << endl;
  }
  cout << endl;

  return fixed_upos;
}

void vectorize_dataset(const morph_model::morph_model_t& lang_morph_model,
                       const form2lemma_t& form2lemma,
                       /*const*/ StringIndex& str_idx, // TODO: str_idx must be const here
                       const DictsHolder dh,
                       uint32_t max_len,
                       vector<TorchMatrix<int64_t>>& v_seq_input,
                       vector<vector<TorchMatrix<int64_t>>>& v_cat_input,
                       vector<TorchMatrix<int64_t>>& v_gold)
{
  map<u32string::size_type, uint32_t> n_samples; // for each form len
  map<u32string::size_type, u32string::size_type> flen2llen; // form len -> max lemma len;
  for ( const auto& kv : form2lemma )
  {
    const u32string& form = str_idx.get_ustr(kv.first.m_str_id);
    const unordered_map<StringIndex::idx_t, size_t>& dec_forms = kv.second;
    const u32string& lemma = str_idx.get_ustr(dec_forms.begin()->first);

    if (1 == dec_forms.size() && form.size() < max_len && lemma.size() < max_len)
    {
      n_samples[form.size()]++;
      flen2llen[form.size()] = max(flen2llen[form.size()], lemma.size());
    }
  }

  v_seq_input.reserve(flen2llen.size());
  v_cat_input.reserve(flen2llen.size());
  v_gold.reserve(flen2llen.size());

  for ( const auto& fl : flen2llen )
  {
    uint32_t form_len = fl.first, max_lemma_len = fl.second;
    TorchMatrix<int64_t> seq_input, gold;
    vector<TorchMatrix<int64_t>> cat_input(lang_morph_model.get_feats_count());

    for (size_t feat_idx = 0; feat_idx < lang_morph_model.get_feats_count(); ++feat_idx)
    {
      // 1 x n_samples
      cat_input[feat_idx].init(1, n_samples[form_len]);
    }

    // max_len x n_samples (x 1 (one feature))
    seq_input.init(form_len /*+ 1*/, n_samples[form_len]); // max_len
    gold.init(max_lemma_len + 1, n_samples[form_len]); // max_len + STOP

    Char32Dict* p_enc_dict = dynamic_cast<Char32Dict*>(dh[0].get());
    Char32Dict* p_dec_dict = dynamic_cast<Char32Dict*>(dh[1].get());

    int64_t sample_no = 0;
    for ( const auto& kv : form2lemma )
    {
      const form_morph_t& enc_form = kv.first;
      const u32string& form = str_idx.get_ustr(enc_form.m_str_id);

      const unordered_map<StringIndex::idx_t, size_t>& dec_forms = kv.second;
      const u32string& lemma = str_idx.get_ustr(dec_forms.begin()->first);

      if (1 == kv.second.size() && form_len == form.size() && lemma.size() <= max_lemma_len)
      {
        // input sequence
        for (size_t char_no = 0; char_no < form.size(); char_no++)
        {
          char32_t ch = form[char_no];
          seq_input.set(char_no, sample_no, p_enc_dict->get_idx(ch));
        }
        //seq_input.set(form.size(), sample_no, p_enc_dict->get_idx(EOS));

        // gold output sequence
        size_t char_no = 0;
        for (; char_no < lemma.size(); char_no++)
        {
          char32_t ch = lemma[char_no];
          gold.set(char_no, sample_no, p_dec_dict->get_idx(ch));
        }
        for (; char_no < max_lemma_len + 1; char_no++)
        {
          gold.set(char_no, sample_no, p_dec_dict->get_idx(EOS));
        }

        // input categories
        /*std::cerr << str_idx.get_str(enc_form.m_str_id) << " "
                  << lang_morph_model.to_string(enc_form.m_feats) << std::endl;*/
        for (size_t feat_idx = 0; feat_idx < lang_morph_model.get_feats_count(); ++feat_idx)
        {
          auto feat_value = lang_morph_model.decode_feat(enc_form.m_feats, feat_idx);
          //std::cerr << " " << feat_value;
          cat_input[feat_idx].set(0, sample_no, feat_value);
        }
        //std::cerr << std::endl;

        sample_no++;
      }
    }

    v_seq_input.emplace_back(seq_input);
    v_cat_input.emplace_back(cat_input);
    v_gold.emplace_back(gold);
  }
}

int train_lemmatization(const train_params_lemmatization_t& params)
{
  // Load data sets
  CoNLLU::Annotation train_data, dev_data;
  train_data.load(params.m_train_set_fn);
  dev_data.load(params.m_dev_set_fn);

  Seq2SeqLemmatizer model(nullptr);
  morph_model::morph_model_t lang_morph_model;
  DictsHolder dh;

  if (params.m_input_model_name.size() > 0)
  {
    model = Seq2SeqLemmatizer();
    model->load(params.m_input_model_name);
    lang_morph_model = model->get_morph_model();
  }
  else
  {
    lang_morph_model = morph_model::morph_model_builder::build(train_data, dev_data);
  }

  {
    // Serialization test
    string t1 = lang_morph_model.to_string();
    cerr << t1 << endl;
    morph_model::morph_model_t m2(t1);
    string t2 = m2.to_string();
    if (t1 != t2)
    {
      cerr << t2 << endl;
      throw;
    }
    // End of serialization test
  }

  StringIndex str_idx;

  form2lemma_t form2lemma, dev_form2lemma;
  for (auto it = train_data.words_begin(); it != train_data.words_end(); it++)
  {
    const CoNLLU::CoNLLULine& line = train_data.get_line((*it).m_line_idx);
    if (line.is_foreign() || line.is_typo())
    {
      continue;
    }
    const string& form = line.form();
    const string& lemma = line.lemma();
    StringIndex::idx_t form_id = str_idx.get_idx(form);
    StringIndex::idx_t lemma_id = str_idx.get_idx(lemma);

    morph_model::morph_feats_t feats = lang_morph_model.convert(line.upos(), line.feats());
    form2lemma[form_morph_t(form_id, feats)][lemma_id] += 1;
  }

  if (params.m_input_model_name.size() > 0)
  {
    dh = model->get_dicts();
  }
  else
  {
    dh = build_char_dicts(form2lemma, str_idx);
  }

  for (auto it = dev_data.words_begin(); it != dev_data.words_end(); it++)
  {
    const CoNLLU::CoNLLULine& line = dev_data.get_line((*it).m_line_idx);
    if (line.is_foreign() || line.is_typo())
    {
      continue;
    }
    const string& form = line.form();
    const string& lemma = line.lemma();
    StringIndex::idx_t form_id = str_idx.get_idx(form);
    StringIndex::idx_t lemma_id = str_idx.get_idx(lemma);
    u32string temp = str_idx.get_ustr(form_id);
    temp = str_idx.get_ustr(lemma_id);

    morph_model::morph_feats_t feats = lang_morph_model.convert(line.upos(), line.feats());
    form_morph_t k = form_morph_t(form_id, feats);
    if (form2lemma.end() != form2lemma.find(k))
    {
      continue;
    }
    dev_form2lemma[k][lemma_id] += 1;
  }

  set<morph_model::feat_base_t> fixed_upos = find_fixed(lang_morph_model, form2lemma, str_idx);

  // Remove fixed UPOS
  auto is_fixed_pos = [&fixed_upos = static_cast<const set<morph_model::feat_base_t>&>(fixed_upos),
      &lang_morph_model = static_cast<morph_model::morph_model_t&>(lang_morph_model)](form2lemma_t::const_reference v){
    const form_morph_t& form = v.first;
    feat_base_t upos = lang_morph_model.decode_upos(form.m_feats);
    return fixed_upos.end() != fixed_upos.find(upos);
  };

  cerr << "Fixed tokens removed from train set: " << utils::backport::erase_if(form2lemma, is_fixed_pos) << endl;
  cerr << "Fixed tokens removed from   dev set: " << utils::backport::erase_if(dev_form2lemma, is_fixed_pos) << endl;

  torch::Device device(params.m_device_string);

  vector<TorchMatrix<int64_t>> train_input_seq, train_gold;
  vector<TorchMatrix<int64_t>> dev_input_seq, dev_gold;
  vector<vector<TorchMatrix<int64_t>>> train_input_cat, dev_input_cat;
  vectorize_dataset(lang_morph_model, form2lemma, str_idx, dh, 31, train_input_seq, train_input_cat, train_gold);
  vectorize_dataset(lang_morph_model, dev_form2lemma, str_idx, dh, 31, dev_input_seq, dev_input_cat, dev_gold);

  for (auto& t : train_input_seq)
      t.to(device);
  for (auto& t : dev_input_seq)
      t.to(device);

  for (auto& t : train_gold)
      t.to(device);
  for (auto& t : dev_gold)
      t.to(device);

  for (auto& v : train_input_cat)
      for (auto& t : v)
          t.to(device);
  for (auto& v : dev_input_cat)
      for (auto& t : v)
          t.to(device);

  if (model.is_empty())
  {
    vector<embd_descr_t> encoder_embd_descr = { embd_descr_t("enc_chars", params.m_encoder_embd_dim) };
    vector<embd_descr_t> decoder_embd_descr = { embd_descr_t("dec_chars", params.m_decoder_embd_dim) };
    vector<rnn_descr_t> encoder_rnn_descr = { rnn_descr_t(params.m_encoder_rnn_hidden_dim) };
    vector<rnn_descr_t> decoder_rnn_descr = { rnn_descr_t(params.m_decoder_rnn_hidden_dim) };
    vector<embd_descr_t> cat_embd_descr;
    for (size_t feat_idx = 0; feat_idx < lang_morph_model.get_feats_count(); ++feat_idx)
    {
      cat_embd_descr.emplace_back(embd_descr_t("cat_" + lang_morph_model.get_feat_name(feat_idx), 8));
      const std::vector<std::string>& values = lang_morph_model.get_feat_vec_ref(feat_idx);
      dh.emplace_back(shared_ptr<StringDict>(new StringDict(values)));
    }
    string str_fixed_upos = utils::join(fixed_upos.begin(),
                                        fixed_upos.end(),
                                        [&lang_morph_model = static_cast<morph_model::morph_model_t&>(lang_morph_model)]
                                        (set<morph_model::feat_base_t>::const_reference v){
      return lang_morph_model.decode_upos_to_str(v);
    });

    model = Seq2SeqLemmatizer(std::move(dh), lang_morph_model,
                              encoder_embd_descr, encoder_rnn_descr,
                              decoder_embd_descr, decoder_rnn_descr,
                              cat_embd_descr, str_fixed_upos);
  }

  std::cerr << model->get_script() << std::endl;

  torch::optim::Adam optimizer(model->parameters(),
                               torch::optim::AdamOptions(params.m_learning_rate)
                               .weight_decay(params.m_weight_decay));

  model->to(device);

  model->train(params, train_input_seq, train_input_cat, train_gold, dev_input_seq, dev_input_cat, dev_gold, optimizer, device);

  return 0;
}

} // namespace train
} // namespace lemmatization
} // namespace deeplima

