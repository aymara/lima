// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_STATIC_GRAPH_STATIC_GRAPH_H
#define DEEPLIMA_SRC_STATIC_GRAPH_STATIC_GRAPH_H

#include <vector>
#include <functional>

#include <torch/torch.h>
#include <torch/serialize/archive.h>

#include "dict_base.h"
#include "nn/torch_modules/deep_biaffine_attention_decoder.h"

namespace deeplima
{
namespace nets
{

class StaticGraphImpl : public torch::nn::Module
{
  struct step_descr_t
  {
    enum step_type_t
    {
      unknown = 0,
      def = 1,
      forward = 2,
      cat = 3,
      log_softmax = 4,
      reshape = 5,
      unbind = 6,
      unsqueeze = 7,
      sigmoid = 8,
      max_step_type
    };

    step_type_t m_type;
    std::vector<std::string> m_names;
    std::map<std::string, std::string> m_args;
    std::map<std::string, std::vector<int64_t>> m_iargs;

    explicit step_descr_t(step_type_t type = unknown, const std::vector<std::string>& names = {}, const std::map<std::string, std::string>& args = {})
      : m_type(type), m_names(names), m_args(args)
    {
      if (m_type >= max_step_type)
      {
        throw std::runtime_error("Error in static graph");
      }
    }
  };

  struct context_t
  {
    std::vector<torch::Tensor> m_tensors;
    explicit context_t(size_t num)
    {
      m_tensors.resize(num);
    }
  };

  struct op_t
  {
    enum op_type_t
    {
      unknown = 1,
      forward = 2,
      max_op_type
    };

    op_t(op_type_t type = unknown)
      : m_type(type)
    {
      if (m_type >= max_op_type)
      {
        throw std::runtime_error("Error in static graph");
      }
    }

    op_type_t m_type;
    std::vector<size_t> m_outputs; // identifiers of target context's tensors (this op is going to create them)
    std::vector<size_t> m_inputs;  // identifiers of input context's tensors (this op expects they are created)
    std::function<void(context_t&)> m_fn;
    std::string m_descr;
  };

  enum module_type_t
  {
    unknown = 0,
    embedding = 1,
    lstm = 2,
    linear = 3,
    dropout = 4,
    deep_biaffine_attention_decoder = 5,
    max_module_type
  };

public:
  StaticGraphImpl(const DictsHolder& dicts, const std::string& script);
  StaticGraphImpl(DictsHolder&& dicts, const std::string& script);
  StaticGraphImpl() { }

  virtual void load(torch::serialize::InputArchive& archive) override;
  virtual void save(torch::serialize::OutputArchive& archive) const override;
  virtual void set_tags(const std::map<std::string, std::string>& tags);
  virtual const std::map<std::string, std::string>& get_tags() const
  {
    return m_tags;
  }
  virtual bool has_tag(const std::string& key) const
  {
    return m_tags.end() != m_tags.find(key);
  }

  virtual void to(torch::Device device, bool non_blocking=false) override;

  std::map<std::string, torch::Tensor> forward(
      const std::map<std::string, torch::Tensor>& inputs,
      const std::string& output_name
      )
  {
    std::vector<std::string> output_names = { output_name };
    return forward(inputs, output_names.cbegin(), output_names.cend());
  }

  std::map<std::string, torch::Tensor> forward(
      const std::map<std::string, torch::Tensor>& inputs,
      const std::vector<std::string>& output_names
      )
  {
    return forward(inputs, output_names.cbegin(), output_names.cend());
  }

  template< class InputIt >
  std::map<std::string, torch::Tensor> forward(
      const std::map<std::string, torch::Tensor>& inputs,
      InputIt outputs_begin,
      InputIt outputs_end
      )
  {
    assert(inputs.size() > 0);
    assert(outputs_begin != outputs_end);

    std::string key = get_exec_plan_key(inputs, outputs_begin, outputs_end);
    if (m_exec_plans.end() == m_exec_plans.find(key))
    {
      prepare_exec_plan(inputs, outputs_begin, outputs_end);
    }

    const std::vector<size_t> plan = m_exec_plans[key];
    context_t ctx(m_tensor_name_to_idx.size());
    for ( const auto& kv : inputs )
    {
      if (m_tensor_name_to_idx.end() == m_tensor_name_to_idx.find(kv.first))
      {
        throw std::runtime_error("Error in static graph");
      }
      size_t idx = m_tensor_name_to_idx[kv.first];
      if (idx >= ctx.m_tensors.size())
      {
        throw std::runtime_error("Error in static graph");
      }
      ctx.m_tensors[idx] = kv.second;
    }

    //size_t total_time = 0;
    for (size_t i = 0; i < plan.size(); i++)
    {
      size_t op_idx = plan[i];
      //chrono::steady_clock::time_point begin = chrono::steady_clock::now();
      m_ops[op_idx].m_fn(ctx);
      //chrono::steady_clock::time_point end = chrono::steady_clock::now();
      //cerr << "\"" << m_ops[op_idx].m_descr << "\" "
      //     << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << "[ms]" << endl;
      //total_time += chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    }
    //cerr << "Total calculations: " << total_time << "\r";

    std::map<std::string, torch::Tensor> rv;
    for (auto it = outputs_begin; it != outputs_end; ++it)
    {
      const std::string& str = *it;
      if (m_tensor_name_to_idx.end() == m_tensor_name_to_idx.find(str))
      {
        throw std::runtime_error("Error in static graph");
      }
      size_t idx = m_tensor_name_to_idx[str];
      if (idx >= ctx.m_tensors.size())
      {
        throw std::runtime_error("Error in static graph");
      }
      rv[str] = ctx.m_tensors[idx];

      //cout << "Output [" << idx << "].size() == " << ctx.m_tensors[idx].sizes() << endl;
    }

    return rv;
  }

  virtual void pretty_dump(std::ostream &stream) const;

  const DictsHolder& get_dicts() const
  {
    return m_dicts;
  }

protected:
  virtual void parse_script(const std::string& script);
  virtual step_descr_t parse_script_line(const std::string& line);
  virtual std::map<std::string, std::string> parse_options(std::istringstream& ss);
  virtual std::vector<std::string> parse_list(std::string& str, char sep);
  virtual std::vector<int64_t> parse_iargs(const std::string& str);

  std::map<std::string, std::vector<size_t>> m_exec_plans;

  template< class InputIt >
  void prepare_exec_plan(
      const std::map<std::string, torch::Tensor>& inputs,
      InputIt outputs_begin,
      InputIt outputs_end
      )
  {
    assert(inputs.size() > 0);
    assert(outputs_begin != outputs_end);

    std::string key = get_exec_plan_key(inputs, outputs_begin, outputs_end);
    if (m_exec_plans.end() != m_exec_plans.find(key))
    {
        throw std::runtime_error("Error in static graph");
    }

    std::set<size_t> given_inputs_idx;
    for ( const auto& kv : inputs )
    {
      size_t in_idx = m_tensor_name_to_idx[kv.first];
      given_inputs_idx.insert(in_idx);
    }

    std::vector<size_t> temp;
    temp.reserve(m_ops.size() * 2);

    std::list<size_t> required;
    for (auto it = outputs_begin; it != outputs_end; ++it)
    {
      const std::string& tensor_name = *it;
      size_t idx = m_tensor_name_to_idx[tensor_name];
      required.push_back(idx);
    }

    while (! required.empty())
    {
      size_t out_idx = required.front();
      required.pop_front();
      size_t op_idx = m_outidx_to_opidx[out_idx];
      temp.push_back(op_idx);

      const op_t& op = m_ops[op_idx];
      for ( size_t in_idx : op.m_inputs )
      {
        if (given_inputs_idx.end() == given_inputs_idx.find(in_idx))
        {
          required.push_back(in_idx);
          // given_inputs_idx.insert(in_idx);
        }
      }
    }

    std::set<size_t> planned_ops;
    std::vector<size_t> plan;
    plan.reserve(temp.size());

    for (int32_t i = temp.size() - 1; i >= 0; i--)
    {
      if (planned_ops.end() != planned_ops.find(temp[i]))
      {
        continue;
      }
      plan.push_back(temp[i]);
      planned_ops.insert(temp[i]);

      //cout << m_ops[temp[i]].m_descr << endl;
    }

    m_exec_plans[key] = plan;
  }

  template< class InputIt >
  std::string get_exec_plan_key(
      const std::map<std::string, torch::Tensor>& inputs,
      InputIt outputs_begin,
      InputIt outputs_end
      )
  {
    std::string k;
    for ( const auto& kv : inputs )
    {
      if (k.size() > 0)
      {
        k += " ";
      }
      k += kv.first;
    }

    k += " -> ";

    for (auto it = outputs_begin; it != outputs_end; ++it)
    {
      k += *it;
    }

    return k;
  }

  template <class T>
  T get_option(const std::map<std::string, std::string>& opts, const std::string& name)
  {
    const auto it = opts.find(name);
    if (opts.cend() == it)
    {
        throw std::runtime_error("Error in static graph");
    }
    T v;
    std::istringstream ss(it->second);
    ss >> v;
    std::string s;
    if (ss >> s)
    {
        throw std::runtime_error("Error in static graph");
    }

    return v;
  }

  bool get_bool_option(const std::map<std::string, std::string>& opts, const std::string& name)
  {
    const auto it = opts.find(name);
    if (opts.cend() == it)
    {
        throw std::runtime_error("Error in static graph");
    }

    if (it->second == "true")
    {
      return true;
    }
    else if (it->second == "false")
    {
      return false;
    }
    throw std::runtime_error("Error in static graph");
  }

  virtual void create_arg(const std::vector<std::string>& names, const std::map<std::string, std::string>& opts);

  virtual void create_submodule_Embedding(const std::string& name, const std::map<std::string, std::string>& opts);
  virtual void create_submodule_LSTM(const std::string& name, const std::map<std::string, std::string>& opts);
  virtual void create_submodule_Linear(const std::string& name, const std::map<std::string, std::string>& opts);
  virtual void create_submodule_Dropout(const std::string& name, const std::map<std::string, std::string>& opts);
  virtual void create_submodule_DeepBiaffineAttentionDecoder(const std::string& name, const std::map<std::string, std::string>& opts);

  DictsHolder m_dicts;
  std::string m_script;
  std::map<std::string, std::string> m_tags;

  std::vector<torch::nn::Embedding> m_embedding;
  std::vector<torch::nn::LSTM> m_lstm;
  std::vector<torch::nn::Linear> m_linear;
  std::vector<torch::nn::Dropout> m_dropout;
  std::vector<deeplima::nets::torch_modules::DeepBiaffineAttentionDecoder> m_deep_biaffine_attention_decoder;

public:

  const std::string& get_script() const
  {
    return m_script;
  }

  // Introspection for Torch -> Eigen converter
  const std::vector<torch::nn::LSTM>& get_layers_lstm() const
  {
    return m_lstm;
  }

  const std::vector<torch::nn::Linear>& get_layers_linear() const
  {
    return m_linear;
  }

  const std::vector<torch::nn::Embedding>& get_layers_embedding() const
  {
    return m_embedding;
  }

  const std::vector<torch::nn::Dropout>& get_layers_dropout() const
  {
    return m_dropout;
  }

  const std::vector<deeplima::nets::torch_modules::DeepBiaffineAttentionDecoder>& get_layers_deep_biaffine_attn_decoder() const
  {
    return m_deep_biaffine_attention_decoder;
  }

  torch::nn::Embedding get_module_by_name(const std::string& name) const
  {
    const auto it = m_modules.find(name);
    if (m_modules.end() == it)
    {
      throw std::runtime_error("Unknown module name");
    }
    const module_ref_t& mr = it->second;
    assert(module_type_t::embedding == mr.m_type);
    return m_embedding[mr.m_idx];
  }

  const std::string get_module_name(size_t idx, const std::string& type) const
  {
    module_type_t t = unknown;
    if (type == "lstm")
    {
      t = lstm;
    }
    else if (type == "embedding")
    {
      t = embedding;
    }
    else if (type == "linear")
    {
      t = linear;
    }
    else if (type == "dropout")
    {
      t = dropout;
    }
    else if (type == "deep_biaffine_attention_decoder")
    {
      t = deep_biaffine_attention_decoder;
    }
    else
    {
      throw std::runtime_error("Unknown module type");
    }

    for ( const auto& p : m_modules )
    {
      if (idx == p.second.m_idx && t == p.second.m_type)
      {
        return p.first;
      }
    }

    return std::string("");
  }

protected:
  virtual void init_rnns();

  struct module_ref_t
  {
    module_type_t m_type;
    size_t m_idx;

    explicit module_ref_t(module_type_t type = module_type_t::unknown, size_t idx = 0)
      : m_type(type), m_idx(idx)
    {
      if (m_type >= module_type_t::max_module_type)
      {
        throw std::runtime_error("Error in static graph");
      }
    }
  };

  std::map<std::string, module_ref_t> m_modules;
  std::set<std::string> m_args;
  std::map<std::string, size_t> m_tensor_name_to_idx;
  std::vector<op_t> m_ops;
  std::map<size_t, size_t> m_outidx_to_opidx; // output tensor's idx -> op idx
};

TORCH_MODULE(StaticGraph);

}
}

#endif
