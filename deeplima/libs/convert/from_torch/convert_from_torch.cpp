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

#include "deeplima/eigen_wrp/segmentation_eigen_inference_impl.h"
#include "tasks/segmentation/model/birnn_classifier_for_segmentation.h"

#include "convert_from_torch.h"

using namespace std;
using namespace torch;
using namespace deeplima::convert_from_torch;

namespace deeplima
{
namespace eigen_impl
{

template <class M, class V, class T>
void BiRnnInferenceBase<M, V, T>::convert_dicts_and_embeddings(const nets::BiRnnClassifierImpl& src)
{
  // dicts and embeddings
  const vector<nets::embd_descr_t>& embd_descr  = src.get_embd_descr();
  size_t count_embd_uint = 0, count_embd_str = 0;
  for (size_t i = 0; i < embd_descr.size(); i++)
  {
    if (/*0 == embd_descr[i].m_type*/ embd_descr[i].m_name == "raw" )
    {
      // There is no dictionary for "raw" embeddings
      continue;
    }
    std::shared_ptr<UInt64Dict> sp_uint64_dict
        = std::dynamic_pointer_cast<UInt64Dict, DictBase>(src.get_dicts()[i]);
    std::shared_ptr<StringDict> sp_str_dict
        = std::dynamic_pointer_cast<StringDict, DictBase>(src.get_dicts()[i]);
    std::shared_ptr<Char32Dict> sp_char32_dict
        = std::dynamic_pointer_cast<Char32Dict, DictBase>(src.get_dicts()[i]);

    if (!sp_uint64_dict && !sp_char32_dict && sp_str_dict)
    {
      count_embd_str++;
    }
    else if ((sp_uint64_dict || sp_char32_dict) && !sp_str_dict)
    {
      count_embd_uint++;
    }
    else
    {
      throw runtime_error("Something wrong with dicts.");
    }
  }

  if (count_embd_uint > 0)
  {
    m_uint_dicts.resize(count_embd_uint);
  }

  if (count_embd_str > 0)
  {
    m_str_dicts.resize(count_embd_str);
  }

  for (size_t i = 0; i < embd_descr.size(); i++)
  {
    if (/*0 == embd_descr[i].m_type*/ embd_descr[i].m_name == "raw")
    {
      // There is no dictionary for "raw" embeddings
      continue;
    }

    const string module_name = "embd_" + embd_descr[i].m_name;
    nn::Embedding m = src.get_module_by_name(module_name);

    std::shared_ptr<UInt64Dict> sp_uint64_dict = std::dynamic_pointer_cast<UInt64Dict, DictBase>(src.get_dicts()[i]);
    std::shared_ptr<StringDict> sp_str_dict = std::dynamic_pointer_cast<StringDict, DictBase>(src.get_dicts()[i]);
    std::shared_ptr<Char32Dict> sp_char32_dict = std::dynamic_pointer_cast<Char32Dict, DictBase>(src.get_dicts()[i]);

    assert(sp_uint64_dict || sp_char32_dict || sp_str_dict);
    if (sp_uint64_dict)
    {
      convert_module_from_torch(m, src.get_dicts()[i], m_uint_dicts[i]);
    }
    else if (sp_char32_dict)
    {
      convert_module_from_torch(m, src.get_dicts()[i], m_uint_dicts[i]);
    }
    else if (sp_str_dict)
    {
      convert_module_from_torch(m, src.get_dicts()[i], m_str_dicts[i]);
    }
  }
}

} // namespace eigen_impl

using namespace eigen_impl;

namespace segmentation
{
namespace eigen_impl
{

template class BiRnnEigenInferenceForSegmentation<Eigen::MatrixXf, Eigen::VectorXf, float>;

template <class M, class V, class T>
void BiRnnEigenInferenceForSegmentation<M, V, T>::convert_from_torch(const std::string& fn)
{
  train::BiRnnClassifierForSegmentationImpl src;
  torch::load(src, fn, torch::Device(torch::kCPU));

  // ngram_descr
  m_ngram_gescr = src.get_ngram_descr();

  // dicts and embeddings
  Parent::convert_dicts_and_embeddings(src);

  // torch modules
  Parent::m_lstm.reserve(src.get_layers_lstm().size());
  for (size_t i = 0; i < src.get_layers_lstm().size(); i++)
  {
    const std::string name = src.get_module_name(i, "lstm");
    Parent::m_lstm_idx[name] = i;

    const nn::LSTM& m = src.get_layers_lstm()[i];
    Parent::m_lstm.emplace_back(params_bilstm_t<M, V>());
    params_bilstm_t<M, V>& layer = Parent::m_lstm.back();

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
  p->linear.push_back(Parent::m_linear[0]);
  Parent::m_wb.resize(1);

  for (const auto& l : Parent::m_linear)
  {
    m_classes.push_back(vector<string>(l.bias.rows(), ""));
  }
}

} // namespace eigen_impl
} // namespace segmentation
} // namespace deeplima

