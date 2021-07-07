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

#ifndef DEEPLIMA_SRC_STATIC_GRAPH_STATIC_GRAPH_H
#define DEEPLIMA_SRC_STATIC_GRAPH_STATIC_GRAPH_H

#include <vector>
#include <functional>

#include <torch/torch.h>
#include <torch/serialize/archive.h>

#include "dict_base.h"

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
      max_step_type
    };

    step_type_t m_type;
    std::vector<std::string> m_names;
    std::map<std::string, std::string> m_args;

    explicit step_descr_t(step_type_t type = unknown, const std::vector<std::string>& names = {}, const std::map<std::string, std::string>& args = {})
      : m_type(type), m_names(names), m_args(args)
    {
      if (m_type >= max_step_type)
      {
        throw;
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
        throw;
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
    max_module_type
  };

public:
  StaticGraphImpl(const DictsHolder& dicts, const std::string& script);
  StaticGraphImpl(DictsHolder&& dicts, const std::string& script);
  StaticGraphImpl() { }

  virtual void load(torch::serialize::InputArchive& archive);
  virtual void save(torch::serialize::OutputArchive& archive) const;

  virtual std::map<std::string, torch::Tensor> forward(
      const std::map<std::string, torch::Tensor>& inputs,
      const std::set<std::string>& outputs
      );

  /*std::shared_ptr<DictBase> get_dict(size_t idx)
  {
    return m_dict[idx];
  }*/

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

  std::map<std::string, std::vector<size_t>> m_exec_plans;
  virtual void prepare_exec_plan(const std::map<std::string, torch::Tensor>& inputs,
                                 const std::set<std::string>& outputs);
  virtual std::string get_exec_plan_key(const std::map<std::string, torch::Tensor>& inputs,
                                        const std::set<std::string>& outputs);

  template <class T> T get_option(const std::map<std::string, std::string>& opts, const std::string& name)
  {
    const auto it = opts.find(name);
    if (opts.cend() == it)
    {
      throw;
    }
    T v;
    std::istringstream ss(it->second);
    ss >> v;
    std::string s;
    if (ss >> s)
    {
      throw;
    }

    return v;
  }

  bool get_bool_option(const std::map<std::string, std::string>& opts, const std::string& name)
  {
    const auto it = opts.find(name);
    if (opts.cend() == it)
    {
      throw;
    }

    if (it->second == "true")
    {
      return true;
    }
    else if (it->second == "false")
    {
      return false;
    }
    throw;
  }

  virtual void create_arg(const std::vector<std::string>& names, const std::map<std::string, std::string>& opts);

  virtual void create_submodule_Embedding(const std::string& name, const std::map<std::string, std::string>& opts);
  virtual void create_submodule_LSTM(const std::string& name, const std::map<std::string, std::string>& opts);
  virtual void create_submodule_Linear(const std::string& name, const std::map<std::string, std::string>& opts);
  virtual void create_submodule_Dropout(const std::string& name, const std::map<std::string, std::string>& opts);

  DictsHolder m_dicts;
  std::string m_script;

  std::vector<torch::nn::Embedding> m_embedding;
  std::vector<torch::nn::LSTM> m_lstm;
  std::vector<torch::nn::Linear> m_linear;
  std::vector<torch::nn::Dropout> m_dropout;

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
        throw;
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
