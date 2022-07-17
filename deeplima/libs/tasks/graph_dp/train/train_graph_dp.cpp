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

#include "../model/birnn_and_deep_biaffine_attention.h"

#include "nn/common/word_seq_vectorizer.h"
#include "deeplima/feat_extractors.h"
#include "deeplima/fastText_wrp/fastText_wrp.h"

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
      p_embd = shared_ptr<FastTextVectorizerToTorchMatrix>(
              new FastTextVectorizerToTorchMatrix(params.m_embeddings_fn)
            );
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

  DictsHolder dh;

  CoNLLUDataSet train_iterator(train_data, params.m_batch_size, { p_embd }, params.m_input_includes_root);
  train_iterator.init();

  CoNLLUDataSet dev_iterator(dev_data, params.m_batch_size, { p_embd }, params.m_input_includes_root);
  dev_iterator.init();

  BiRnnAndDeepBiaffineAttention model(nullptr);

  if (params.m_input_model_name.size() == 0)
  {
    vector<embd_descr_t> embd_descr = train_iterator.get_embd_descr();
    embd_descr.emplace_back("raw", p_embd->dim(), 0);
    vector<rnn_descr_t> rnn_descr;
    rnn_descr.reserve(params.m_rnn_hidden_dims.size());
    for (size_t d : params.m_rnn_hidden_dims)
    {
      rnn_descr.push_back(rnn_descr_t(d));
    }

    vector<deep_biaffine_attention_descr_t> decoder_descr = { deep_biaffine_attention_descr_t(128) };

    DictsHolder tag_dh;
    model = BiRnnAndDeepBiaffineAttention(std::move(dh),
                                  embd_descr,
                                  rnn_descr,
                                  decoder_descr,
                                  utils::split(params.m_tasks_string, ','),
                                  std::move(tag_dh),
                                  boost::filesystem::path(params.m_embeddings_fn).stem().string(),
                                  params.m_input_includes_root);
  }

  cerr << model->get_script() << endl;

  torch::optim::Adam optimizer(model->parameters(),
                               torch::optim::AdamOptions(params.m_learning_rate)
                               .weight_decay(params.m_weight_decay));

  torch::Device device(params.m_device_string);

  model->to(device);

  model->train(params, { "arc" },
               train_iterator, dev_iterator,
               optimizer, device);

  cerr << "Uhu!" << endl;


  return -1;
}

} // train
} // graph_dp
} // deeplima
