// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "../model/birnn_and_deep_biaffine_attention.h"

#include "nn/common/word_seq_vectorizer.h"
#include "deeplima/feat_extractors.h"
#include "deeplima/fastText_wrp/fastText_wrp.h"
#include "tasks/common/word_dict_builder.h"

#include "train_graph_dp.h"
#include "conllu_file_iterator.h"

using namespace std;
using namespace deeplima::nets;

namespace deeplima
{
namespace graph_dp
{
namespace train
{

typedef WordDictBuilderImpl<CoNLLU::WordLevelAdapter,
                            ConlluFeatExtractor<CoNLLU::WordLevelAdapter::token_t>> TagDictBuilderFromCoNLLU;

typedef FastTextVectorizer<TorchMatrix<float>> FastTextVectorizerToTorchMatrix;

int train_graph_dp(const train_params_graph_dp_t& params)
{
  // Load data sets
  CoNLLU::Annotation train_data, dev_data;
  train_data.load(params.m_train_set_fn);
  dev_data.load(params.m_dev_set_fn);

  shared_ptr<FastTextVectorizerToTorchMatrix> p_embd;
  if (params.m_embeddings_fn.size() > 0)
  {
    try
    {
      p_embd = std::make_shared<FastTextVectorizerToTorchMatrix>(params.m_embeddings_fn);
      assert(nullptr != p_embd.get());
      //feat_descr.push_back({ CoNLLUToTorchMatrix::str_feature, "form", p_embd.get() });
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

  TagDictBuilderFromCoNLLU tag_dict_builder;
  // TODO retrieve tagger tasks list from model or config
  // Currently the list below (form,upos…) is hard-coded while the tagging model can be trained with various tasks.
  // This list should be saved somewhere and retrieved here later on
  ConlluFeatExtractor<CoNLLU::WordLevelAdapter::token_t> feat_extractor
      = tag_dict_builder.preprocess(CoNLLU::WordLevelAdapter(&train_data),
                                    "*form,upos,feats,xpos,-Typo,-Foreign");
  DictsHolder tag_dh = tag_dict_builder.process(CoNLLU::WordLevelAdapter(&train_data),
                                                feat_extractor, 0, "");

  DictsHolder dh;

  // Build the deprel (syntactic relation label) dictionary from the training
  // data. Shared with the dev set so class ids are consistent. Class id == the
  // dict size at insertion time (stable, 0-based). This mapping will also be
  // needed by the label decoder and saved with the model.
  auto deprel2id = std::make_shared<std::map<std::string, int64_t>>();
  {
    CoNLLU::WordLevelAdapter adapter(&train_data);
    CoNLLU::WordLevelAdapter::const_iterator it = adapter.begin();
    while (adapter.end() != it)
    {
      if ((*it).is_word())
      {
        const std::string& rel = (*it).deprel();
        if (deprel2id->find(rel) == deprel2id->end())
        {
          int64_t next_id = static_cast<int64_t>(deprel2id->size());
          (*deprel2id)[rel] = next_id;
        }
      }
      it++;
    }
  }
  std::cerr << "Built deprel dictionary: " << deprel2id->size() << " labels" << std::endl;

  CoNLLUDataSet train_iterator(train_data,
                               params.m_batch_size,
                               feat_extractor,
                               tag_dh,
                               { p_embd },
                               params.m_input_includes_root,
                               deprel2id);
  train_iterator.init();

  CoNLLUDataSet dev_iterator(dev_data,
                             params.m_batch_size,
                             feat_extractor,
                             tag_dh,
                             { p_embd },
                             params.m_input_includes_root,
                             deprel2id);
  dev_iterator.init();

  BiRnnAndDeepBiaffineAttention model(nullptr);

  if (params.m_input_model_name.size() == 0)
  {
    vector<embd_descr_t> embd_descr = train_iterator.get_embd_descr();
    embd_descr.emplace(embd_descr.begin(), "raw", p_embd->dim(), 0);
    vector<rnn_descr_t> rnn_descr;
    rnn_descr.reserve(params.m_rnn_hidden_dims.size());
    for (size_t d : params.m_rnn_hidden_dims)
    {
      rnn_descr.push_back(rnn_descr_t(d));
    }

    vector<deep_biaffine_attention_descr_t> decoder_descr = { deep_biaffine_attention_descr_t(128) };

    // 1st arg = the dict holder used to size the input embeddings. The generated
    // script numbers each embedding by its position in embd_descr, so this holder
    // must be PARALLEL to embd_descr: a placeholder for the "raw" feature at index
    // 0 (it has no Embedding) followed by the morph-feature dicts in the same
    // order get_embd_descr() returns them. Passing the full tag dict holder here
    // misaligned the indices whenever a feature was filtered out for having an
    // empty dict, reading the wrong (often empty) dict and crashing.
    // (2nd/6th arg = the output classes; a separate object, so no double-move.)
    DictsHolder feat_dicts = train_iterator.get_embd_feature_dicts();
    DictsHolder input_dicts;
    if (!feat_dicts.empty())
    {
      input_dicts.push_back(feat_dicts.front()); // placeholder for "raw" (slot 0, unused)
      for (const auto& d : feat_dicts)
      {
        input_dicts.push_back(d);
      }
    }
    model = BiRnnAndDeepBiaffineAttention(std::move(input_dicts),
                                  embd_descr,
                                  rnn_descr,
                                  decoder_descr,
                                  utils::split(params.m_tasks_string, ','),
                                  std::move(tag_dh),
                                  boost::filesystem::path(params.m_embeddings_fn).stem().string(),
                                  params.m_input_includes_root,
                                  static_cast<int64_t>(deprel2id->size()));
  }
  else
  {
    model = BiRnnAndDeepBiaffineAttention();
    model->load(params.m_input_model_name);
  }

  // cerr << model->get_script() << endl;

  torch::optim::Adam optimizer(model->parameters(),
                               torch::optim::AdamOptions(params.m_learning_rate)
                               .weight_decay(params.m_weight_decay));

  torch::Device device(params.m_device_string);

  model->to(device);

  double min_perf = 0;

  for (const string& opt_name : utils::split(params.m_optimizers, ','))
  {
    shared_ptr<torch::optim::Optimizer> optimizer;

    if (opt_name == "adam")
    {
      optimizer = make_shared<torch::optim::Adam>(model->parameters(),
                                                  torch::optim::AdamOptions(params.m_learning_rate)
                                                  .weight_decay(params.m_weight_decay));
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

    model->train(params, { "arc" },
                 train_iterator, dev_iterator,
                 *optimizer, min_perf, device);

    std::cerr << "Optimizer " << opt_name << " stopped at " << min_perf << std::endl;
  }

  return 0;
}

} // train
} // graph_dp
} // deeplima
