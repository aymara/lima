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

#include <sstream>

#include "birnn_seq_classifier.h"

using namespace std;

namespace deeplima
{
namespace nets
{

string BiRnnClassifierImpl::generate_script(const std::vector<embd_descr_t>& embd_descr,
                                            const std::vector<rnn_descr_t>& rnn_descr,
                                            const string& output_name,
                                            size_t num_classes)
{
  assert(num_classes > 0);

  stringstream ss;

  ss << "input_dropout = def Dropout prob=0.7" << std::endl;

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

  ss << "fc_" << output_name << " = def Linear input_size=" << input_size
     << " output_size=" << num_classes << std::endl;

  ss << std::endl;

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

  ss << output_name << "_raw = forward module=fc_" << output_name
     << " input=" << last_output_name << std::endl;

  ss << output_name << " = log_softmax input=" << output_name << "_raw" << std::endl;

  return ss.str();
}

} // namespace nets
} // namespace deeplima

