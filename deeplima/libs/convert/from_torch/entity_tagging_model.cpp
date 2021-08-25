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

#include "deeplima/eigen_wrp/tagging_eigen_inference_impl.h"
#include "tasks/ner/model/birnn_classifier_for_ner.h"

#include "convert_from_torch.h"

using namespace std;
using namespace torch;
using namespace deeplima::convert_from_torch;
using namespace deeplima::eigen_impl;

namespace deeplima
{
namespace tagging
{
namespace eigen_impl
{

void convert_classes(const DictsHolder& src, vector<vector<string>>& classes);

template class BiRnnEigenInferenceForTagging<Eigen::MatrixXf, Eigen::VectorXf, float>;

template <class M, class V, class T>
void BiRnnEigenInferenceForTagging<M, V, T>::convert_from_torch(const std::string& fn)
{
  train::BiRnnClassifierForNerImpl src;
  torch::load(src, fn, torch::Device(torch::kCPU));

  // dicts and embeddings
  Parent::convert_dicts_and_embeddings(src);
  m_embd_fn.push_back(src.get_embd_fn(0));
  assert(m_embd_fn[0].size() > 0);

  // classes
  convert_classes(src.get_classes(), m_classes);
  m_class_names = src.get_class_names();

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
  Parent::m_ops.push_back(new Op_BiLSTM_Dense_ArgMax<M, V, T>());
  Parent::m_params.push_back(new params_bilstm_dense_argmax_t<M, V>());
  params_bilstm_dense_argmax_t<M, V> *p = static_cast<params_bilstm_dense_argmax_t<M, V>*>(Parent::m_params.back());
  p->bilstm = Parent::m_lstm[0];
  for (size_t i = 0; i < Parent::m_linear.size(); ++i)
  {
    p->linear.push_back(Parent::m_linear[i]);
  }
  Parent::m_wb.resize(1);
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
} // namespace tagging
} // namespace deeplima

