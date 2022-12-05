// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <string>
#include <algorithm>
#include <cctype>

#include "conllu/treebank.h"
#include "deeplima/torch_wrp/torch_matrix.h"
#include "static_graph/dict_holder_adapter.h"

#include "tasks/common/word_dict_builder.h"
#include "nn/common/word_seq_vectorizer.h"
#include "deeplima/feat_extractors.h"

#include "../model/birnn_classifier_for_tag.h"
#include "deeplima/fastText_wrp/fastText_wrp.h"
#include "deeplima/utils/split_string.h"

#include "train_tag.h"

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
  UPosFeatExtractor(const std::string&)
  {
  }

  inline static bool needs_preprocessing()
  {
    return false;
  }

  inline void preprocess(const Token&)
  {
  }

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
std::shared_ptr<M> vectorize_gold(const CoNLLU::Annotation& annot, const FeatExtractor& fe, const DictsHolder& tag_dh)
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

  std::shared_ptr<M> out(new M(len, tag_dh.size()));

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

    for (size_t feat_idx = 0; feat_idx < tag_dh.size(); ++feat_idx)
    {
      const std::string& feat_val = fe.feat_value(*it, feat_idx);
      StringDict::key_t idx = dynamic_pointer_cast<StringDict>(tag_dh[feat_idx])->get_idx(feat_val);
      out->set(current_timepoint, feat_idx, idx);
    }

    current_timepoint++;
    if (current_timepoint == std::numeric_limits<uint64_t>::max())
    {
      throw std::overflow_error("Too much words in the dataset.");
    }

    it++;
  }

  return out;
}

typedef WordDictBuilderImpl<CoNLLU::WordLevelAdapter,
                            deeplima::TokenStrFeatExtractor<CoNLLU::WordLevelAdapter::token_t>> WordDictBuilderFromCoNLLU;
typedef WordDictBuilderImpl<CoNLLU::WordLevelAdapter,
                            ConlluFeatExtractor<CoNLLU::WordLevelAdapter::token_t>> TagDictBuilderFromCoNLLU;
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
  ConlluFeatExtractor<CoNLLU::WordLevelAdapter::token_t> feat_extractor
      = tag_dict_builder.preprocess(CoNLLU::WordLevelAdapter(&train_data),
                                    params.m_tasks_string);
  DictsHolder tag_dh = tag_dict_builder.process(CoNLLU::WordLevelAdapter(&train_data),
                                                feat_extractor, 0, "");

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

  shared_ptr<FastTextVectorizerToTorchMatrix> p_embd;
  if (params.m_embeddings_fn.size() > 0)
  {
    try
    {
      p_embd = shared_ptr<FastTextVectorizerToTorchMatrix>(
              new FastTextVectorizerToTorchMatrix(params.m_embeddings_fn)
            );
      assert(nullptr != p_embd.get());
      feat_descr.push_back({ CoNLLUToTorchMatrix::str_feature, "form", p_embd.get() });
    }
    catch (const exception& e)
    {
      cerr << e.what() << endl;
      return -1;
    }
    catch (...)
    {
      cerr << "Something wrong happened while loading \""
           << params.m_embeddings_fn << "\"" << endl;
      return -1;
    }
  }

  shared_ptr<DirectDict<TorchMatrix<float>>> p_eos;
  if (params.m_use_eos)
  {
    if (string::npos != params.m_tasks_string.find("eos"))
    {
      throw std::invalid_argument("Can't use EOS as both input and output");
    }

    p_eos = shared_ptr<DirectDict<TorchMatrix<float>>>(new DirectDict<TorchMatrix<float>>(2));
    assert(nullptr != p_embd.get());
    feat_descr.push_back({ CoNLLUToTorchMatrix::int_feature, "eos", p_eos.get() });
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
      = vectorize_gold<TorchMatrix<int64_t>, ConlluFeatExtractor<CoNLLU::WordLevelAdapter::token_t>>(train_data,
                                                                                                     feat_extractor,
                                                                                                     tag_dh);

  shared_ptr<TorchMatrix<int64_t>> dev_gold
      = vectorize_gold<TorchMatrix<int64_t>, ConlluFeatExtractor<CoNLLU::WordLevelAdapter::token_t>>(dev_data,
                                                                                                     feat_extractor,
                                                                                                     tag_dh);

  if (train_input.first && train_input.second && train_input.first->size() != train_input.second->size())
  {
    cerr << "ERROR: train set length missmatch: "
         << train_input.first->size()
         << " " << train_input.second->size() << endl;
    throw runtime_error("Train set length missmatch");
  }

  if ((train_input.first && train_input.first->size() != train_gold->size())
    || (train_input.second && train_input.second->size() != train_gold->size()))
  {
    cerr << "ERROR: train set length (input != gold): "
         << (train_input.first ? train_input.first->size() : train_input.second->size())
         << " " << train_gold->size() << endl;
    throw runtime_error("Train set length missmatch (input != gold)");
  }

  if (params.m_input_model_name.size() == 0)
  {
    vector<embd_descr_t> embd_descr = vectorizer.get_embd_descr();
    embd_descr.emplace_back("raw", train_input.second->get_tensor().size(1), 0);
    vector<rnn_descr_t> rnn_descr = { rnn_descr_t(params.m_rnn_hidden_dim) /*, rnn_descr_t(32) */ };
    model = BiRnnClassifierForNer(std::move(dh),
                                  embd_descr,
                                  rnn_descr,
                                  feat_extractor.feats(),
                                  std::move(tag_dh),
                                  boost::filesystem::path(params.m_embeddings_fn).stem().string(),
                                  params);
  }

  // cerr << model->get_script() << endl;

  torch::Device device(params.m_device_string);

  train_input.first->to(device);
  train_input.second->to(device);
  train_gold->to(device);

  dev_input.first->to(device);
  dev_input.second->to(device);
  dev_gold->to(device);

  model->to(device);

  double min_perf = 0;

  for (const string& opt_name : utils::split(params.m_optimizers, ','))
  {
    shared_ptr<torch::optim::Optimizer> optimizer;

    if (opt_name == "adam")
    {
      optimizer = make_shared<torch::optim::Adam>(model->parameters(),
                                                  torch::optim::AdamOptions(params.m_learning_rate)
                                                  .weight_decay(params.m_weight_decay)
                                                  .betas({params.m_beta_one, params.m_beta_two}));
    }
    else if (opt_name == "sgd")
    {
      optimizer = make_shared<torch::optim::SGD>(model->parameters(),
                                                 torch::optim::SGDOptions(params.m_learning_rate * 1000)
                                                 .weight_decay(params.m_weight_decay));
    }
    else
    {
      throw runtime_error("Unknown optimizer: " + opt_name);
    }

    model->train(params,
                 feat_extractor.feats(),
                *(train_input.first.get()), *(train_input.second.get()), *(train_gold.get()),
                *(dev_input.first.get()), *(dev_input.second.get()), *(dev_gold.get()),
                *optimizer, min_perf, device);

    std::cerr << "Optimizer " << opt_name << " stopped at " << min_perf << std::endl;
  }

  return 0;
}

} // namespace train
} // namespace tagging
} // namespace deeplima

