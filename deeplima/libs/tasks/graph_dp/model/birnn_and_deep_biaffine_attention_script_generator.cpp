/*
    Copyright 2022 CEA LIST

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

#include <sstream>
#include <cassert>

#include "birnn_and_deep_biaffine_attention.h"

using namespace std;
using namespace deeplima::nets;

namespace deeplima
{
namespace graph_dp
{
namespace train
{

string BiRnnAndDeepBiaffineAttentionImpl::generate_script(const vector<embd_descr_t>& embd_descr,
                                                          const vector<rnn_descr_t>& rnn_descr,
                                                          const vector<deep_biaffine_attention_descr_t>& decoder_descr,
                                                          const vector<std::string>& /*output_names*/,
                                                          bool input_includes_root/*,
                                                          const vector<uint32_t>& classes*/)
{
  stringstream ss;

  ss << "input_dropout = def Dropout prob=0.3" << std::endl;
  //ss << "decoder_dropout = def Dropout prob=0.1" << std::endl;

  size_t rnn_input_size = 0;
  for (size_t i = 0; i < embd_descr.size(); i++)
  {
    if (1 == embd_descr[i].m_type)
    {
      ss << "embd_" << embd_descr[i].m_name << " = def Embedding dict=" << i
         << " dim=" << embd_descr[i].m_dim << std::endl;
    }
    rnn_input_size += embd_descr[i].m_dim;
  }

  ss << std::endl;

  size_t input_size = rnn_input_size;
  for (size_t i = 0; i < rnn_descr.size(); i++)
  {
    ss << "lstm_" << i << " = def LSTM input_size=" << input_size
       << " hidden_size=" << rnn_descr[i].m_dim << " num_layers=1 bidirectional=true" << std::endl;
    input_size = rnn_descr[i].m_dim * 2;
  }

  ss << std::endl;

  assert(decoder_descr.size() == 1);
  ss << "decoder_0 = def DeepBiaffineAttentionDecoder input_dim=" << input_size
     << " hidden_arc_dim=" << decoder_descr[0].m_arc_dim
     << " input_includes_root=" << (input_includes_root ? "true" : "false")
     << endl;

  ss << std::endl;

  /*for (size_t i = 0; i < output_names.size(); ++i)
  {
    ss << "fc_" << output_names[i] << " = def Linear input_size=" << input_size
       << " output_size=" << classes[i] << std::endl;

    ss << std::endl;
  }*/

  for (size_t i = 0; i < embd_descr.size(); i++)
  {
    ss << embd_descr[i].m_name << " = def Arg" << std::endl;
  }

  ss << std::endl;

  for (size_t i = 0; i < embd_descr.size(); i++)
  {
    if (1 == embd_descr[i].m_type)
    {
      ss << "input_" << embd_descr[i].m_name << " = forward module=embd_"
         << embd_descr[i].m_name
         << " input=" << embd_descr[i].m_name << std::endl;
    }
  }

  ss << std::endl;

  ss << "input_brut = cat input=";
  for (size_t i = 0; i < embd_descr.size(); i++)
  {
    if (0 == embd_descr[i].m_type)
    {
      ss << embd_descr[i].m_name;
    }
    else if (1 == embd_descr[i].m_type)
    {
      ss << "input_" << embd_descr[i].m_name;
    }
    if (i < embd_descr.size() - 1)
    {
      ss << ",";
    }
  }
  ss << " dim=2" << std::endl;

  ss << std::endl;

  ss << "input = forward module=input_dropout input=input_brut" << std::endl;

  ss << std::endl;

  std::string last_output_name = "input";
  for (size_t i = 0; i < rnn_descr.size(); i++)
  {
    ss << "rnn_out_" << i << " = forward module=lstm_" << i
       << " input=" << last_output_name << std::endl;
    std::stringstream t;
    t << "rnn_out_" << i;
    last_output_name = t.str();
  }

  ss << std::endl;

  //ss << "decoder_input = forward module=decoder_dropout input=" << last_output_name << endl;

  ss << "arc_raw = forward module=decoder_0 input=" << last_output_name << endl;
  ss << "arc = log_softmax input=arc_raw dim=2" << endl;

  /*for (size_t i = 0; i < output_names.size(); ++i)
  {
    ss << output_names[i] << "_raw = forward module=fc_" << output_names[i]
       << " input=" << last_output_name << std::endl;

    ss << output_names[i] << " = log_softmax input=" << output_names[i] << "_raw" << std::endl;
    ss << endl;
  }*/

  return ss.str();
}

} // namespace train
} // namespace graph_dp
} // namespace deeplima

