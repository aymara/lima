// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "static_graph.h"
#include "dict.h"

using torch::indexing::Slice;
using namespace torch;
using namespace std;

namespace deeplima
{
namespace nets
{

StaticGraphImpl::StaticGraphImpl(const DictsHolder& dicts, const string& script)
  : m_dicts(dicts),
    m_script(script)
{
  parse_script(m_script);
  init_rnns();
}

StaticGraphImpl::StaticGraphImpl(DictsHolder&& dicts, const std::string& script)
  : m_dicts(std::move(dicts)),
    m_script(script)
{
  parse_script(m_script);
  init_rnns();
}

void StaticGraphImpl::load(serialize::InputArchive& archive)
{
  c10::IValue val;
  archive.read("model_script", val);
  m_script = *(val.toString().get());

  // Load dicts
  c10::IValue ival_types_of_dicts;
  archive.read("types_of_dicts", ival_types_of_dicts);
  const c10::List<c10::IValue>& types_of_dicts = ival_types_of_dicts.toList();

  m_dicts.resize(types_of_dicts.size());
  for (size_t i = 0; i < m_dicts.size(); i++)
  {
    std::ostringstream dict_id;
    dict_id << "dict_" << i;

    if (!types_of_dicts.get(i).isString())
    {
      throw;
    }
    const std::string& dict_type = types_of_dicts.get(i).toStringRef();

    c10::IValue v;
    if (archive.try_read(dict_id.str(), v))
    {
      if (!v.isList())
      {
        throw;
      }

      if (dict_type == UInt64Dict::class_id())
      {
        m_dicts[i] = shared_ptr<UInt64Dict>(new UInt64Dict());
      }
      else if (dict_type == WstringDict::class_id())
      {
        m_dicts[i] = shared_ptr<WstringDict>(new WstringDict());
      }
      else if (dict_type == StringDict::class_id())
      {
        m_dicts[i] = shared_ptr<StringDict>(new StringDict());
      }
      else if (dict_type == Char32Dict::class_id())
      {
        m_dicts[i] = shared_ptr<Char32Dict>(new Char32Dict());
      }
      else
      {
        throw runtime_error("Unknown dict type.");
      }

      m_dicts[i]->fromIValue(v);
    }
  }

  parse_script(m_script); // calls "register_module"

  // Load tags
  c10::IValue v;
  if (archive.try_read("tags", v))
  {
    if (!v.isGenericDict())
    {
      throw runtime_error("\"tags\" must be a dict of strings.");
    }

    c10::Dict<c10::IValue, c10::IValue> d = v.toGenericDict();
    for ( const auto &it : d )
    {
      if (!it.key().isString())
      {
        cerr << "ERROR: keys in \"tags\" dict must be strings" << endl;
      }
      if (!it.value().isString())
      {
        cerr << "ERROR: values in \"tags\" dict must be strings" << endl;
      }

      if (m_tags.end() != m_tags.find(it.key().toStringRef()))
      {
        throw runtime_error("Duplicated tags in the model");
      }

      m_tags[it.key().toStringRef()] = it.value().toStringRef();
    }
  }

  torch::nn::Module::load(archive);
}

void StaticGraphImpl::save(serialize::OutputArchive& archive) const
{
  archive.write("model_script", m_script);

  torch::nn::Module::save(archive);

  // Save dicts
  c10::List<std::string> types_of_dicts;
  for (size_t i = 0; i < m_dicts.size(); i++)
  {
    //cerr << i << "\t" << m_dicts[i]->get_class_id() << std::endl;
    types_of_dicts.push_back(m_dicts[i]->get_class_id());
  }

  archive.write("types_of_dicts", types_of_dicts);

  for (size_t i = 0; i < m_dicts.size(); i++)
  {
    ostringstream s;
    s << "dict_" << i;
    archive.write(s.str(), m_dicts[i]->toIValue());
  }

  // Save tags
  c10::Dict<string, string> temp_tags;
  for ( const auto &it : m_tags )
  {
    temp_tags.insert(it.first, it.second);
  }

  archive.write("tags", temp_tags);
}

void StaticGraphImpl::set_tags(const map<string, string>& tags)
{
  for ( const auto &it : tags)
  {
    m_tags.insert({ it.first, it.second });
  }
}

void StaticGraphImpl::pretty_dump(ostream &stream) const
{
  size_t counter = 0;
  for ( const auto& m : modules(false) )
  {
    stream << m->name() << endl;
    for ( const auto& t : m->parameters() )
    {
      int64_t c = 1;
      for ( int64_t v : t.sizes() )
      {
        c *= v;
      }
      stream << "\t" << t.name() << " " << c << " " << t.sizes() << endl;
      counter += c;
    }
  }
  stream << "Total parameters = " << counter << endl;
}

void StaticGraphImpl::to(torch::Device device, bool non_blocking)
{
  for (auto m : m_embedding) m->to(device);
  for (auto m : m_lstm) m->to(device);
  for (auto m : m_linear) m->to(device);
  for (auto m : m_dropout) m->to(device);
  for (auto m : m_deep_biaffine_attention_decoder) m->to(device);
  torch::nn::Module::to(device, non_blocking);
  cerr << "StaticGraphImpl::to( " << device << " )" << std::endl;
}

void StaticGraphImpl::parse_script(const string& script)
{
  cerr << script << endl;
  stringstream ss(script);
  string line;

  while (getline(ss, line))
  {
    if (line.size() == 0)
    {
      continue;
    }
    step_descr_t step = parse_script_line(line);
    //cerr << line << endl;

    if (step.m_type == step_descr_t::def)
    {
      continue;
    }

    // generate op
    op_t op;
    op.m_descr = line;

    size_t known_tensors = m_tensor_name_to_idx.size();
    for ( const string& name : step.m_names )
    {
      if (m_tensor_name_to_idx.end() != m_tensor_name_to_idx.find(name))
      {
        throw;
      }
      m_tensor_name_to_idx[name] = known_tensors;
      op.m_outputs.push_back(m_tensor_name_to_idx[name]);
      known_tensors += 1;
    }

    vector<string> input_tensor_names = parse_list(step.m_args["input"], ',');
    for ( const string& name : input_tensor_names )
    {
      auto it = m_tensor_name_to_idx.find(name);
      if (m_tensor_name_to_idx.end() == it)
      {
        throw;
      }
      op.m_inputs.push_back(it->second);
    }

    switch (step.m_type)
    {
      case step_descr_t::forward:
        {
          string module_name = get_option<string>(step.m_args, "module");
          auto it = m_modules.find(module_name);
          if (m_modules.end() == it)
          {
            throw;
          }
          module_ref_t module_ref = it->second;
          switch (module_ref.m_type)
          {
            case module_type_t::embedding:
              {
                torch::nn::Embedding& m = m_embedding[module_ref.m_idx];
                vector<size_t>& inputs = op.m_inputs;
                vector<size_t>& outputs = op.m_outputs;

                if (inputs.size() != 1 || outputs.size() != 1)
                {
                  throw;
                }

                op.m_fn = [&m, inputs, outputs](context_t& ctx)
                {
                  auto out = m->forward(ctx.m_tensors[inputs[0]]);
                  //cerr << ctx.m_tensors[inputs[0]] << endl;
                  //cerr << out << endl;
                  ctx.m_tensors[outputs[0]] = out;
                };
              }
              break;

            case module_type_t::lstm:
              {
                torch::nn::LSTM& m = m_lstm[module_ref.m_idx];
                const vector<size_t>& inputs = op.m_inputs;
                const vector<size_t>& outputs = op.m_outputs;

                if (inputs.size() < 1)
                {
                  throw;
                }

                op.m_fn = [&m, inputs, outputs](context_t& ctx)
                {
                  //cerr << ctx.m_tensors[inputs[0]].sizes() << endl;
                  torch::optional<std::tuple<torch::Tensor, torch::Tensor>> h0_and_c0; // (h0, c0)
                  if (inputs.size() > 1)
                  {
                    if (3 == inputs.size())
                    {
                      h0_and_c0 = { ctx.m_tensors[inputs[1]], ctx.m_tensors[inputs[2]] };
                    }
                    else
                    {
                      throw;
                    }
                  }
                  auto out = m->forward(ctx.m_tensors[inputs[0]], h0_and_c0);
                  //cerr << ctx.m_tensors[inputs[0]] << endl;
                  if (outputs.size() > 0)
                  {
                    ctx.m_tensors[outputs[0]] = std::get<0>(out);
                    //cerr << ctx.m_tensors[outputs[0]] << endl;
                    if (outputs.size() > 1)
                    {
                      ctx.m_tensors[outputs[1]] = std::get<0>(std::get<1>(out));
                      if (outputs.size() > 2)
                      {
                        ctx.m_tensors[outputs[2]] = std::get<1>(std::get<1>(out));
                        if (outputs.size() > 3)
                        {
                          throw;
                        }
                      }
                    }
                  }
                };
              }
              break;

            case module_type_t::linear:
              {
                torch::nn::Linear& m = m_linear[module_ref.m_idx];
                vector<size_t>& inputs = op.m_inputs;
                vector<size_t>& outputs = op.m_outputs;

                if (inputs.size() != 1 || outputs.size() != 1)
                {
                  throw;
                }

                op.m_fn = [&m, inputs, outputs](context_t& ctx)
                {
                  auto out = m->forward(ctx.m_tensors[inputs[0]]);
                  ctx.m_tensors[outputs[0]] = out;
                };
              }
              break;

            case module_type_t::dropout:
              {
                torch::nn::Dropout& m = m_dropout[module_ref.m_idx];
                vector<size_t>& inputs = op.m_inputs;
                vector<size_t>& outputs = op.m_outputs;

                if (inputs.size() != 1 || outputs.size() != 1)
                {
                  throw;
                }

                op.m_fn = [&m, inputs, outputs](context_t& ctx)
                {
                  auto out = m->forward(ctx.m_tensors[inputs[0]]);
                  //cerr << ctx.m_tensors[inputs[0]] << endl;
                  //cerr << out << endl;
                  ctx.m_tensors[outputs[0]] = out;
                };
              }
              break;

            case module_type_t::deep_biaffine_attention_decoder:
              {
                torch_modules::DeepBiaffineAttentionDecoder& m
                    = m_deep_biaffine_attention_decoder[module_ref.m_idx];
                vector<size_t>& inputs = op.m_inputs;
                vector<size_t>& outputs = op.m_outputs;

                if (inputs.size() != 1 || outputs.size() != 1)
                {
                  throw;
                }

                op.m_fn = [&m, inputs, outputs](context_t& ctx)
                {
                  auto out = m->forward(ctx.m_tensors[inputs[0]]);
                  ctx.m_tensors[outputs[0]] = out;
                };
              }
              break;

            default:
              throw;
          }
        }
        break;

      case step_descr_t::cat:
        {
          vector<size_t>& inputs = op.m_inputs;
          vector<size_t>& outputs = op.m_outputs;
          int64_t dim = atoi(step.m_args["dim"].c_str());

          if (inputs.size() == 0 || outputs.size() != 1)
          {
            throw;
          }

          op.m_fn = [inputs, outputs, dim](context_t& ctx)
          {
            std::vector<torch::Tensor> input_tensors;
            input_tensors.resize(inputs.size());
            for (size_t i = 0; i < inputs.size(); i++)
            {
              input_tensors[i] = ctx.m_tensors[inputs[i]];
              //cerr << input_tensors[i].sizes() << std::endl;
            }
            auto out = torch::cat(input_tensors, dim);
            ctx.m_tensors[outputs[0]] = out;
          };
        }
        break;

      case step_descr_t::reshape:
        {
          vector<size_t>& inputs = op.m_inputs;
          vector<size_t>& outputs = op.m_outputs;
          const auto shape = step.m_iargs["dims"];

          if (inputs.size() == 0 || outputs.size() != 1)
          {
            throw;
          }

          op.m_fn = [inputs, outputs, shape](context_t& ctx)
          {
            //cerr << "reshape arg: " << ctx.m_tensors[inputs[0]].sizes() << endl;
            auto out = torch::reshape(ctx.m_tensors[inputs[0]], shape);
            //cerr << "reshape res: " << out.sizes() << endl;
            ctx.m_tensors[outputs[0]] = out;
          };
        }
        break;

      case step_descr_t::unbind:
        {
          vector<size_t>& inputs = op.m_inputs;
          vector<size_t>& outputs = op.m_outputs;
          int64_t dim = atoi(step.m_args["dim"].c_str());

          if (inputs.size() != 1)
          {
            throw;
          }

          op.m_fn = [inputs, outputs, dim](context_t& ctx)
          {
            //cerr << "unbind arg: " << ctx.m_tensors[inputs[0]].sizes() << endl;
            auto out = torch::unbind(ctx.m_tensors[inputs[0]], dim);
            //cerr << "unbind res: " << out.sizes() << endl;
            if (outputs.size() != out.size())
            {
              throw;
            }
            for (size_t j = 0; j < out.size(); ++j)
            {
              ctx.m_tensors[outputs[j]] = out[j];
            }
          };
        }
        break;

      case step_descr_t::unsqueeze:
        {
          vector<size_t>& inputs = op.m_inputs;
          vector<size_t>& outputs = op.m_outputs;
          int64_t dim = atoi(step.m_args["dim"].c_str());

          if (inputs.size() != 1 || outputs.size() != 1)
          {
            throw;
          }

          op.m_fn = [inputs, outputs, dim](context_t& ctx)
          {
            auto out = torch::unsqueeze(ctx.m_tensors[inputs[0]], dim);
            ctx.m_tensors[outputs[0]] = out;
          };
        }
        break;

      case step_descr_t::log_softmax:
        {
          vector<size_t>& inputs = op.m_inputs;
          vector<size_t>& outputs = op.m_outputs;
          int64_t dim = 2;
          if (step.m_iargs.end() != step.m_iargs.find("dim"))
          {
            dim = step.m_iargs.find("dim")->second[0];
          }

          if (inputs.size() != 1 || outputs.size() != 1)
          {
            throw;
          }

          op.m_fn = [inputs, outputs, dim](context_t& ctx)
          {
            auto in = ctx.m_tensors[inputs[0]];
            ctx.m_tensors[outputs[0]] = torch::nn::functional::log_softmax(in, dim);
          };
        }
        break;

      case step_descr_t::sigmoid:
        {
          vector<size_t>& inputs = op.m_inputs;
          vector<size_t>& outputs = op.m_outputs;

          if (inputs.size() != 1 || outputs.size() != 1)
          {
            throw;
          }

          op.m_fn = [inputs, outputs](context_t& ctx)
          {
            ctx.m_tensors[outputs[0]] = ctx.m_tensors[inputs[0]].sigmoid();
          };
        }
        break;

      default:
        throw;
    }

    m_ops.push_back(op);

    for ( size_t out_idx : op.m_outputs )
    {
      if (m_outidx_to_opidx.end() != m_outidx_to_opidx.find(out_idx))
      {
        throw;
      }
      m_outidx_to_opidx[out_idx] = m_ops.size() - 1;
    }
  }
  //cerr << "Done!" << endl;
}

map<string, string> StaticGraphImpl::parse_options(istringstream& ss)
{
  map<string, string> opts;

  string s;
  while (ss >> s)
  {
    string::size_type p = s.find('=');
    if (string::npos == p)
    {
      throw;
    }
    string k = s.substr(0, p);
    string v = s.substr(p+1);
    if (opts.end() != opts.find(k))
    {
      throw;
    }
    opts[k] = v;
  }

  return opts;
}

vector<string> StaticGraphImpl::parse_list(string& str, char sep)
{
  vector<string> l;

  string::size_type prev = 0;
  string::size_type next = str.find(sep, prev);
  while (next != string::npos)
  {
    l.push_back(str.substr(prev, next - prev));
    prev = next + 1;
    next = str.find(sep, prev);
  }

  l.push_back(str.substr(prev, next));

  return l;
}

StaticGraphImpl::step_descr_t StaticGraphImpl::parse_script_line(const std::string& line)
{
  istringstream ss(line);
  StaticGraphImpl::step_descr_t step;

  string names_list;
  ss >> names_list;
  step.m_names = parse_list(names_list, ',');

  string type;
  ss >> type;

  if (type == "=")
  {
    ss >> type;
  }

  if (type == "def")
  {
    if (step.m_names.size() != 1)
    {
      throw;
    }
    // Name def Class arg1=Value1 arg2=Value2 ...
    step.m_type = step_descr_t::step_type_t::def;
    string cls;

    ss >> cls;

    map<string, string> opts = parse_options(ss);

    if (cls == "Embedding")
    {
      create_submodule_Embedding(names_list, opts);
    }
    else if (cls == "LSTM")
    {
      create_submodule_LSTM(names_list, opts);
    }
    else if (cls == "Linear")
    {
      create_submodule_Linear(names_list, opts);
    }
    else if (cls == "Dropout")
    {
      create_submodule_Dropout(names_list, opts);
    }
    else if (cls == "DeepBiaffineAttentionDecoder")
    {
      create_submodule_DeepBiaffineAttentionDecoder(names_list, opts);
    }
    else if (cls == "Arg")
    {
      create_arg(step.m_names, opts);
    }
    else
    {
      throw;
    }
  }
  else if (type == "cat")
  {
    // Name = cat Input1,Input2
    step.m_type = step_descr_t::cat;
    step.m_args = parse_options(ss);
  }
  else if (type == "forward")
  {
    //
    step.m_type = step_descr_t::forward;
    step.m_args = parse_options(ss);
  }
  else if (type == "log_softmax")
  {
    //
    step.m_type = step_descr_t::log_softmax;
    step.m_args = parse_options(ss);
    auto it = step.m_args.find("dim");
    if (step.m_args.end() != it)
    {
      step.m_iargs["dim"] = parse_iargs(it->second);
    }
  }
  else if (type == "sigmoid")
  {
    //
    step.m_type = step_descr_t::sigmoid;
    step.m_args = parse_options(ss);
  }
  else if (type == "reshape")
  {
    //
    step.m_type = step_descr_t::reshape;
    step.m_args = parse_options(ss);
    auto it = step.m_args.find("dims");
    if (step.m_args.end() == it)
    {
      throw;
    }
    step.m_iargs["dims"] = parse_iargs(it->second);
  }
  else if (type == "unbind")
  {
    // Out1,Out2,... = unbind Input
    step.m_type = step_descr_t::unbind;
    step.m_args = parse_options(ss);
  }
  else if (type == "unsqueeze")
  {
    // Out1,Out2,... = unsqueeze Input
    step.m_type = step_descr_t::unsqueeze;
    step.m_args = parse_options(ss);
  }
  else
  {
    throw;
  }

  return step;
}

vector<int64_t> StaticGraphImpl::parse_iargs(const string& str)
{
  vector<int64_t> rv;
  istringstream ss(str);

  for (int64_t i; ss >> i;)
  {
    rv.push_back(i);
    if (ss.peek() == ',')
    {
      ss.ignore();
    }
  }

  return rv;
}

void StaticGraphImpl::create_arg(const std::vector<std::string>& names, const std::map<std::string, std::string>& /*opts*/)
{
  for ( const string& name : names )
  {
    if (m_args.cend() != m_args.find(name))
    {
      throw;
    }

    if (m_tensor_name_to_idx.cend() != m_tensor_name_to_idx.find(name))
    {
      throw;
    }

    m_args.insert(name);
    size_t idx = m_tensor_name_to_idx.size();
    m_tensor_name_to_idx[name] = idx;
  }
}

void StaticGraphImpl::create_submodule_Embedding(const std::string& name, const std::map<std::string, std::string>& opts)
{
  // Required options. It must throw an exception if they aren't available
  int64_t dict_idx = get_option<int64_t>(opts, "dict");
  int64_t dim = get_option<int64_t>(opts, "dim");

  torch::nn::Embedding m(m_dicts[dict_idx]->size(), dim);
  m_embedding.push_back(m);
  m_modules[name] = module_ref_t(module_type_t::embedding, m_embedding.size() - 1);
  m->pretty_print(cerr);
  cerr << endl;
  for ( const auto& t : m->parameters())
  {
    cerr << t.sizes() << endl;
    //cerr << "itemsize = " << t.type().typeMeta().itemsize() << endl;
  }
  cerr << endl;

  register_module(name, m);
}

void StaticGraphImpl::create_submodule_Dropout(const std::string& name, const std::map<std::string, std::string>& opts)
{
  // Required options. It must throw an exception if they aren't available
  float prob = get_option<float>(opts, "prob");

  torch::nn::Dropout m(prob);
  m_dropout.push_back(m);
  m_modules[name] = module_ref_t(module_type_t::dropout, m_dropout.size() - 1);

  m->pretty_print(cerr);
  cerr << endl;
  for ( const auto& t : m->parameters())
  {
    cerr << t.sizes() << endl;
  }
  cerr << endl;

  register_module(name, m);
}

void StaticGraphImpl::create_submodule_Linear(const std::string& name, const std::map<std::string, std::string>& opts)
{
  // Required options. It must throw an exception if they aren't available
  int64_t input_size = get_option<int64_t>(opts, "input_size");
  int64_t output_size = get_option<int64_t>(opts, "output_size");

  torch::nn::Linear m(input_size, output_size);
  m_linear.push_back(m);
  m_modules[name] = module_ref_t(module_type_t::linear, m_linear.size() - 1);

  m->pretty_print(cerr);
  cerr << endl;
  for ( const auto& t : m->parameters())
  {
    cerr << t.sizes() << endl;
  }
  cerr << endl;

  register_module(name, m);
}

void StaticGraphImpl::create_submodule_LSTM(const std::string& name, const std::map<std::string, std::string>& opts)
{
  int64_t input_size = get_option<int64_t>(opts, "input_size");
  int64_t hidden_size = get_option<int64_t>(opts, "hidden_size");

  torch::nn::LSTMOptions lstm_options(input_size, hidden_size);
  set<string> consumed_options({ "input_size", "hidden_size" });
  // input_size=6 hidden_size=4 num_layers=2 batch_first=true bidirectional=true dropout
  for (const auto& kv: opts)
  {
    if (consumed_options.cend() != consumed_options.find(kv.first))
    {
      continue;
    }
    if (kv.first == "num_layers")
    {
      lstm_options.num_layers(get_option<int64_t>(opts, "num_layers"));
    }
    else if (kv.first == "batch_first")
    {
      lstm_options.batch_first(get_bool_option(opts, "batch_first"));
    }
    else if (kv.first == "bidirectional")
    {
      lstm_options.bidirectional(get_bool_option(opts, "bidirectional"));
    }
    else if (kv.first == "dropout")
    {
      lstm_options.dropout(get_option<double>(opts, "dropout"));
    }
    else
    {
      throw;
    }
    consumed_options.insert(kv.first);
  }

  if (consumed_options.size() != opts.size())
  {
    throw;
  }

  torch::nn::LSTM m(lstm_options);

  m_lstm.push_back(m);
  m_modules[name] = module_ref_t(module_type_t::lstm, m_lstm.size() - 1);

  m->pretty_print(cerr);
  cerr << endl;

  register_module(name, m);
}

void StaticGraphImpl::create_submodule_DeepBiaffineAttentionDecoder(const string& name, const map<string, string>& opts)
{
  int64_t input_dim = get_option<int64_t>(opts, "input_dim");
  int64_t hidden_arc_dim = get_option<int64_t>(opts, "hidden_arc_dim");
  bool input_includes_root = get_bool_option(opts, "input_includes_root");

  torch_modules::DeepBiaffineAttentionDecoder m(input_dim, hidden_arc_dim, input_includes_root);
  m_deep_biaffine_attention_decoder.push_back(m);
  m_modules[name] = module_ref_t(module_type_t::deep_biaffine_attention_decoder, m_deep_biaffine_attention_decoder.size() - 1);
  register_module(name, m);
}

void StaticGraphImpl::init_rnns()
{
  for (torch::nn::LSTM &m : m_lstm)
  {
    OrderedDict<string, Tensor> params = m->named_parameters();
    OrderedDict<string, Tensor>::Iterator it = params.begin();
    while (params.end() != it)
    {
      OrderedDict<string, Tensor>::Item& item = *it;
      if (item.key().substr(0, 9) == "weight_hh")
      {
        Tensor& t = item.value();
        assert(t.size(0) % 4 == 0);
        int width = t.size(0) / 4;

        for (int i = 0; i < 4; i++){
          auto a = item.value().index({ Slice(width * i, width * (i+1)), Slice() });
          torch::nn::init::orthogonal_(a);
        }
      }
      it++;
    }
  }
}

} // namespace nets
} // namespace deeplima

