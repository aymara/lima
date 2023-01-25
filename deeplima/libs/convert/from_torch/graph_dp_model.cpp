// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "deeplima/eigen_wrp/graph_dp_eigen_inference_impl.h"
#include "tasks/graph_dp/model/birnn_and_deep_biaffine_attention.h"

#include "convert_from_torch.h"

using namespace std;
using namespace torch;
using namespace deeplima::convert_from_torch;
using namespace deeplima::eigen_impl;

namespace deeplima
{
namespace graph_dp
{
namespace eigen_impl
{

void convert_classes(const DictsHolder& src, vector<vector<string>>& classes);

template class BiRnnAndDeepBiaffineAttentionEigenInference<Eigen::MatrixXf, Eigen::VectorXf, float>;

template <class M, class V, class T>
void BiRnnAndDeepBiaffineAttentionEigenInference<M, V, T>::convert_from_torch(const std::string& fn)
{
  train::BiRnnAndDeepBiaffineAttentionImpl src;
  torch::load(src, fn, torch::Device(torch::kCPU));

  // dicts and embeddings
  Parent::convert_dicts_and_embeddings(src);
  m_embd_fn.push_back(src.get_embd_fn(0));
  assert(m_embd_fn[0].size() > 0);

  Parent::m_input_str_dicts_names = src.get_input_class_names();
  Parent::m_output_str_dicts_names = src.get_output_class_names();

  // torch modules
  Parent::m_lstm.reserve(src.get_layers_lstm().size());
  for (size_t i = 0; i < src.get_layers_lstm().size(); i++)
  {
    const std::string name = src.get_module_name(i, "lstm");
    Parent::m_lstm_idx[name] = i;

    const nn::LSTM& m = src.get_layers_lstm()[i];
    Parent::m_lstm.emplace_back(typename Parent::params_bilstm_spec_t());
    typename Parent::params_bilstm_spec_t& layer = Parent::m_lstm.back();

    convert_module_from_torch(m, layer);
  }

  Parent::m_multi_bilstm.emplace_back(std::make_shared<typename Parent::params_multilayer_bilstm_spec_t>(Parent::m_lstm));
  Parent::m_multi_bilstm_idx["encoder"] = 0;

  Parent::m_linear.reserve(src.get_layers_linear().size());
  for (size_t i = 0; i < src.get_layers_linear().size(); i++)
  {
    const std::string name = src.get_module_name(i, "linear");
    Parent::m_linear_idx[name] = i;

    const nn::Linear& m = src.get_layers_linear()[i];
    Parent::m_linear.emplace_back(params_linear_t<M, V>());
    params_linear_t<M, V>& layer = Parent::m_linear.back();

    convert_module_from_torch(m, layer);
  }

  m_deep_biaffine_attn_decoder.reserve(src.get_layers_deep_biaffine_attn_decoder().size());

  for (size_t i = 0; i < src.get_layers_deep_biaffine_attn_decoder().size(); i++)
  {
    const std::string name = src.get_module_name(i, "deep_biaffine_attention_decoder");
    m_deep_biaffine_attn_decoder_idx[name] = i;

    const deeplima::nets::torch_modules::DeepBiaffineAttentionDecoder& m
        = src.get_layers_deep_biaffine_attn_decoder()[i];

    m_deep_biaffine_attn_decoder.emplace_back(std::make_shared<params_deep_biaffine_attn_decoder_t<M, V>>());
    auto& layer = *m_deep_biaffine_attn_decoder.back().get();

    convert_module_from_torch(m, layer);
  }

  // temp: create exec plan
  Parent::m_ops.push_back(std::make_shared<Op_BiLSTM<M, V, T>>());
  Parent::m_params.push_back(Parent::m_multi_bilstm[0]);

  Parent::m_ops.push_back(std::make_shared<Op_DeepBiaffineAttnDecoder<M, V, T>>());
  Parent::m_params.push_back(m_deep_biaffine_attn_decoder[0]);

  Parent::m_wb.resize(2);

  // tags
  // cerr << "TAGS:" << endl;
  // for ( const auto& it : src.get_tags() )
  // {
  //   cerr << "\t" << it.first << " = " << it.second << endl;
  // }
  // cerr << endl;
}

void convert_classes(const DictsHolder& src, vector<vector<string>>& classes)
{
  classes.resize(src.size());
  for (size_t i = 0; i < classes.size(); ++i)
  {
    shared_ptr<StringDict> d = dynamic_pointer_cast<StringDict, DictBase>(src[i]);
    classes[i].reserve(d->size());
    for (size_t j = 0; j < d->size(); ++j)
    {
      classes[i].push_back(d->get_value(j));
    }
  }
}

} // namespace eigen_impl
} // namespace graph_dp
} // namespace deeplima

