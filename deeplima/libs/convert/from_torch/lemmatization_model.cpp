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

#include "deeplima/eigen_wrp/lemmatization_eigen_inference_impl.h"
#include "tasks/lemmatization/model/seq2seq_for_lemmatization.h"

#include "convert_from_torch.h"

using namespace std;
using namespace torch;
using namespace deeplima::convert_from_torch;
using namespace deeplima::eigen_impl;

namespace deeplima
{
namespace lemmatization
{
namespace eigen_impl
{

template class BiRnnSeq2SeqEigenInferenceForLemmatization<Eigen::MatrixXf, Eigen::VectorXf, float>;

template <class M, class V, class T>
void BiRnnSeq2SeqEigenInferenceForLemmatization<M, V, T>::convert_from_torch(const std::string& fn)
{
  train::Seq2SeqLemmatizerImpl src;
  torch::load(src, fn, torch::Device(torch::kCPU));

  // dicts and embeddings
  Parent::convert_dicts_and_embeddings(src);

  m_morph_model = src.get_morph_model();

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

  // temp: create exec plan
  // Encoder
  Parent::m_ops.push_back(new Op_BiLSTM<M, V, T>());
  Parent::m_params.push_back(new params_bilstm_t<M, V>());
  params_bilstm_t<M, V> *p_enc = static_cast<params_bilstm_t<M, V>*>(Parent::m_params.back());
  *p_enc = Parent::m_lstm[Parent::m_lstm_idx["encoder_lstm_0"]];

  // Linear for decoder init state H
  Parent::m_ops.push_back(new Op_Linear<M, V, T>());
  Parent::m_params.push_back(new params_linear_t<M, V>());
  params_linear_t<M, V> *p_fc_h0 = static_cast<params_linear_t<M, V>*>(Parent::m_params.back());
  *p_fc_h0 = Parent::m_linear[Parent::m_linear_idx["interm_fc_h0"]];

  // Linear for decoder init state C
  Parent::m_ops.push_back(new Op_Linear<M, V, T>());
  Parent::m_params.push_back(new params_linear_t<M, V>());
  params_linear_t<M, V> *p_fc_c0 = static_cast<params_linear_t<M, V>*>(Parent::m_params.back());
  *p_fc_c0 = Parent::m_linear[Parent::m_linear_idx["interm_fc_c0"]];

  // Linear for features encoder
  Parent::m_ops.push_back(new Op_Linear<M, V, T>());
  Parent::m_params.push_back(new params_linear_t<M, V>());
  params_linear_t<M, V> *p_fc_feats = static_cast<params_linear_t<M, V>*>(Parent::m_params.back());
  *p_fc_feats = Parent::m_linear[Parent::m_linear_idx["fc_cat2decoder"]];

  // Decoder
  Parent::m_ops.push_back(new Op_LSTM_Beam_Decoder<M, V, T>());
  Parent::m_params.push_back(new params_lstm_beam_decoder_t<M, V>());
  params_lstm_beam_decoder_t<M, V> *p_dec = static_cast<params_lstm_beam_decoder_t<M, V>*>(Parent::m_params.back());
  p_dec->bilstm = Parent::m_lstm[Parent::m_lstm_idx["decoder_lstm_0"]];
  p_dec->linear.push_back(Parent::m_linear[Parent::m_linear_idx["fc_output"]]);

  Parent::m_wb.resize(5);

  // tags
  cerr << "TAGS:" << endl;
  for ( const auto& it : src.get_tags() )
  {
    cerr << "\t" << it.first << " = " << it.second << endl;
  }
  cerr << endl;
}

} // namespace eigen_impl
} // namespace tagging
} // namespace deeplima

