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

#include <string>
#include <algorithm>
#include <cctype>

#include "conllu/treebank.h"
#include "deeplima/torch_wrp/torch_matrix.h"
#include "static_graph/dict_holder_adapter.h"

#include "word_dict_builder.h"
#include "word_seq_vectorizer.h"
#include "deeplima/feat_extractors.h"

#include "../model/birnn_classifier_for_ner.h"
#include "deeplima/fastText_wrp/fastText_wrp.h"

#include "train_ner.h"

using namespace std;
using namespace deeplima::nets;

namespace deeplima
{
namespace tagging
{
namespace train
{

template <typename Token>
class UPosFeatExtractor
{
public:
  inline static std::string feat_value(const Token& token, size_t feat_no)
  {
    switch (feat_no)
    {
    case 0:
      return token.upos();
    default:
      throw runtime_error("Unknown feature id.");
    }
  }

  inline static size_t size()
  {
    return 1;
  }
};

template <class M, class FeatExtractor>
std::shared_ptr<M> vectorize_gold(const CoNLLU::Annotation& annot, const shared_ptr<StringDict> dict)
{
  CoNLLU::WordLevelAdapter src(&annot);
  int64_t len = 0;
  typename CoNLLU::WordLevelAdapter::const_iterator i = src.begin();
  while (src.end() != i)
  {
    if ((*i).is_word())
    {
      len++;
    }
    i++;
  }

  std::shared_ptr<M> out(new M(len, 1));

  typename CoNLLU::WordLevelAdapter::const_iterator it = src.begin();
  uint64_t current_timepoint = 0;
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
    const std::string& feat_val = FeatExtractor::feat_value(*it, 0);
    StringDict::key_t idx = dict->get_idx(feat_val);
    out->set(current_timepoint, 0, idx);

    current_timepoint++;
    if (current_timepoint == std::numeric_limits<int64_t>::max())
    {
      throw std::overflow_error("Too much words in the dataset.");
    }

    it++;
  }

  return out;
}

typedef WordDictBuilderImpl<CoNLLU::WordLevelAdapter, deeplima::TokenStrFeatExtractor<CoNLLU::WordLevelAdapter::token_t>> WordDictBuilderFromCoNLLU;
typedef WordDictBuilderImpl<CoNLLU::WordLevelAdapter, UPosFeatExtractor<CoNLLU::WordLevelAdapter::token_t>> TagDictBuilderFromCoNLLU;
typedef WordSeqVectorizerImpl<CoNLLU::WordLevelAdapter,
                              deeplima::TokenStrFeatExtractor<CoNLLU::WordLevelAdapter::token_t>,
                              deeplima::TokenUIntFeatExtractor<CoNLLU::WordLevelAdapter::token_t>,
                              TorchMatrix<int64_t>,
                              TorchMatrix<float> > CoNLLUToTorchMatrix;

typedef FastTextVectorizer<TorchMatrix<float>> FastTextVectorizerToTorchMatrix;

int train_entity_tagger(const train_params_tagging_t& params)
{
  // Load data sets
  CoNLLU::Annotation train_data, dev_data;
  train_data.load(params.m_train_set_fn);
  dev_data.load(params.m_dev_set_fn);

  // Find target classes
  TagDictBuilderFromCoNLLU tag_dict_builder;
  DictsHolder tag_dh = tag_dict_builder.process(CoNLLU::WordLevelAdapter(&train_data), 0, "");

  DictsHolder dh;
  BiRnnClassifierForNer model(nullptr);

  if (params.m_input_model_name.size() > 0)
  {
    model = BiRnnClassifierForNer();
    model->load(params.m_input_model_name);
  }
  else
  {
    if (params.m_trainable_embeddings_dim > 0)
    {
      WordDictBuilderFromCoNLLU dict_builder;

      dh = dict_builder.process(CoNLLU::WordLevelAdapter(&train_data),
                                params.m_trainable_embeddings_cutoff_freq);
      dh.erase(dh.begin()); // cased forms aren't needed
    }
  }

  // Input features
  vector<CoNLLUToTorchMatrix::feature_descr_t> feat_descr;

  if (params.m_embeddings_fn.size() > 0)
  {
    shared_ptr<FastTextVectorizerToTorchMatrix> p
        = shared_ptr<FastTextVectorizerToTorchMatrix>(
            new FastTextVectorizerToTorchMatrix(params.m_embeddings_fn)
          );
    assert(nullptr != p.get());
    feat_descr.push_back({ CoNLLUToTorchMatrix::str_feature, "form", p });
  }

  if (params.m_use_eos)
  {
    shared_ptr<DirectDict<TorchMatrix<float>>> p
        = shared_ptr<DirectDict<TorchMatrix<float>>>(new DirectDict<TorchMatrix<float>>(2));
    assert(nullptr != p.get());
    feat_descr.push_back({ CoNLLUToTorchMatrix::int_feature, "eos", p });
  }

  vector<CoNLLUToTorchMatrix::embeddable_feature_descr_t> embd_feat_descr;
  if (params.m_input_model_name.size() > 0)
  {
    embd_feat_descr.push_back({
                                CoNLLUToTorchMatrix::str_feature,
                                "lc(form)",
                                int(model->get_embd_descr()[0].m_dim),
                                model->get_dicts()[0]
                              });
  }
  else if (params.m_trainable_embeddings_dim > 0)
  {
    assert(dh[0]->size() > 1);
    embd_feat_descr.push_back({
                                CoNLLUToTorchMatrix::str_feature,
                                "lc(form)",
                                int(params.m_trainable_embeddings_dim),
                                dh[0]
                              });
  }

  assert(feat_descr.size() > 0 || embd_feat_descr.size() > 0);

  CoNLLUToTorchMatrix vectorizer(feat_descr, embd_feat_descr);

  CoNLLUToTorchMatrix::vectorization_t train_input
      = vectorizer.process(CoNLLU::WordLevelAdapter(&train_data));
  CoNLLUToTorchMatrix::vectorization_t dev_input
      = vectorizer.process(CoNLLU::WordLevelAdapter(&dev_data));

  shared_ptr<TorchMatrix<int64_t>> train_gold
      = vectorize_gold<TorchMatrix<int64_t>, UPosFeatExtractor<CoNLLU::WordLevelAdapter::token_t>>(train_data,
                                                                dynamic_pointer_cast<StringDict>(tag_dh[0]));

  shared_ptr<TorchMatrix<int64_t>> dev_gold
      = vectorize_gold<TorchMatrix<int64_t>, UPosFeatExtractor<CoNLLU::WordLevelAdapter::token_t>>(dev_data,
                                                                dynamic_pointer_cast<StringDict>(tag_dh[0]));

  if (train_input.first->size() != train_input.second->size())
  {
    cerr << "ERROR: train set length missmatch: "
         << train_input.first->size()
         << " " << train_input.second->size() << endl;
    throw runtime_error("Train set length missmatch");
  }

  if (train_input.first->size() != train_gold->size())
  {
    cerr << "ERROR: train set length (input != gold): "
         << train_input.first->size()
         << " " << train_gold->size() << endl;
    throw runtime_error("Train set length missmatch (input != gold)");
  }

  if (params.m_input_model_name.size() == 0)
  {
    vector<embd_descr_t> embd_descr = vectorizer.get_embd_descr();
    embd_descr.emplace_back("raw", train_input.second->get_tensor().size(1), 0);
    vector<rnn_descr_t> rnn_descr = { rnn_descr_t(params.m_rnn_hidden_dim) };
    model = BiRnnClassifierForNer(std::move(dh),
                                  embd_descr,
                                  rnn_descr,
                                  "tagging",
                                  std::move(tag_dh),
                                  params.m_embeddings_fn);
  }

  cerr << model->get_script() << endl;

  torch::optim::Adam optimizer(model->parameters(),
                               torch::optim::AdamOptions(params.m_learning_rate)
                               .weight_decay(params.m_weight_decay));

  torch::Device device(params.m_device_string);

  train_input.first->to(device);
  train_input.second->to(device);
  train_gold->to(device);

  dev_input.first->to(device);
  dev_input.second->to(device);
  dev_gold->to(device);

  model->to(device);

  model->train(params.m_max_epochs,
               params.m_batch_size,
               params.m_sequence_length,
               "tagging",
              *(train_input.first.get()), *(train_input.second.get()), *(train_gold.get()),
              *(dev_input.first.get()), *(dev_input.second.get()), *(dev_gold.get()),
              optimizer, params.m_output_model_name, device);


  return 0;
}

} // namespace train
} // namespace tagging
} // namespace deeplima

