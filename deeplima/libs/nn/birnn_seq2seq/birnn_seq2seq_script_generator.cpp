// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <string>
#include <vector>
#include <sstream>

#include "birnn_seq2seq.h"

using namespace std;

namespace deeplima
{
namespace nets
{

string BiRnnSeq2SeqImpl::generate_script(const vector<embd_descr_t>& encoder_embd_descr,
                                         const vector<rnn_descr_t>& encoder_rnn_descr,
                                         const vector<embd_descr_t>& decoder_embd_descr,
                                         const vector<rnn_descr_t>& decoder_rnn_descr,
                                         const vector<embd_descr_t>& cat_embd_descr,
                                         size_t n_output_classes)
{
  stringstream ss;

  // Encoder definitions
  ss << "encoder_embd_dropout = def Dropout prob=0.7" << std::endl;

  size_t encoder_rnn_input_size = 0;
  for (size_t i = 0; i < encoder_embd_descr.size(); i++)
  {
    if (1 == encoder_embd_descr[i].m_type)
    {
      ss << "embd_" << encoder_embd_descr[i].m_name
         << " = def Embedding dict=" << i
         << " dim=" << encoder_embd_descr[i].m_dim << std::endl;
    }
    encoder_rnn_input_size += encoder_embd_descr[i].m_dim;
  }

  ss << std::endl;

  size_t encoder_input_size = encoder_rnn_input_size;
  for (size_t i = 0; i < encoder_rnn_descr.size(); i++)
  {
    ss << "encoder_lstm_" << i
       << " = def LSTM input_size=" << encoder_input_size
       << " hidden_size=" << encoder_rnn_descr[i].m_dim
       << " num_layers=1 bidirectional=true" << std::endl;
    encoder_input_size = encoder_rnn_descr[i].m_dim * 2;
  }

  ss << std::endl;

  // Decoder definitions
  ss << "decoder_embd_dropout = def Dropout prob=0.7" << std::endl;

  size_t decoder_rnn_input_size = 0;
  for (size_t i = 0; i < decoder_embd_descr.size(); i++)
  {
    if (1 == decoder_embd_descr[i].m_type)
    {
      ss << "embd_" << decoder_embd_descr[i].m_name
         << " = def Embedding dict=" << encoder_embd_descr.size() + i
         << " dim=" << decoder_embd_descr[i].m_dim << std::endl;
    }
    decoder_rnn_input_size += decoder_embd_descr[i].m_dim;
  }

  ss << std::endl;

  size_t decoder_input_size = decoder_rnn_input_size;
  for (size_t i = 0; i < decoder_rnn_descr.size(); i++)
  {
    ss << "decoder_lstm_" << i
       << " = def LSTM input_size=" << decoder_input_size
       << " hidden_size=" << decoder_rnn_descr[i].m_dim
       << " num_layers=1 bidirectional=false" << std::endl;
    decoder_input_size = decoder_rnn_descr[i].m_dim;
  }

  ss << std::endl;

  vector<string> output_names = { "output" };

  size_t input_size = decoder_input_size;
  for (size_t i = 0; i < output_names.size(); ++i)
  {
    ss << "fc_" << output_names[i] << " = def Linear input_size=" << input_size
       << " output_size=" << n_output_classes << std::endl;

    ss << std::endl;
  }

  ss << std::endl;

  size_t cat_embd_input_dim = 0;
  for (size_t i = 0; i < cat_embd_descr.size(); i++)
  {
    if (1 == cat_embd_descr[i].m_type)
    {
      ss << "embd_" << cat_embd_descr[i].m_name
         << " = def Embedding dict=" << encoder_embd_descr.size() + decoder_embd_descr.size() + i
         << " dim=" << cat_embd_descr[i].m_dim << std::endl;
    }
    cat_embd_input_dim += cat_embd_descr[i].m_dim;
  }

  ss << std::endl;

  ss << "interm_fc_h0 = def Linear input_size=" << encoder_input_size * 2 + cat_embd_input_dim
     << " output_size=" << decoder_rnn_descr[0].m_dim << std::endl;
  ss << "interm_fc_c0 = def Linear input_size=" << encoder_input_size * 2 + cat_embd_input_dim
     << " output_size=" << decoder_rnn_descr[0].m_dim << std::endl;

  ss << std::endl;

  if (cat_embd_descr.size() > 0)
  {
    ss << "cat_embd_dropout = def Dropout prob=0.7" << std::endl << std::endl;

    ss << "fc_cat2decoder = def Linear input_size=" << cat_embd_input_dim
       << " output_size=" << cat_embd_input_dim << std::endl << std::endl;


    // present categories as initial state of encoder
    ss << "cat_embd_2_encoder_dropout = def Dropout prob=0.7" << std::endl << std::endl;

    ss << "fc_cat2encoder = def Linear input_size=" << cat_embd_input_dim
       << " output_size=" << encoder_input_size << std::endl << std::endl;
  }

  // Args definitions
  for (size_t i = 0; i < encoder_embd_descr.size(); i++)
  {
    ss << encoder_embd_descr[i].m_name << " = def Arg" << std::endl;
  }

  for (size_t i = 0; i < decoder_embd_descr.size(); i++)
  {
    ss << decoder_embd_descr[i].m_name << " = def Arg" << std::endl;
  }

  for (size_t i = 0; i < cat_embd_descr.size(); i++)
  {
    ss << cat_embd_descr[i].m_name << " = def Arg" << std::endl;
  }

  ss << std::endl;

  // Encoder forward pass
  if (cat_embd_descr.size() > 0)
  {
    for (size_t i = 0; i < cat_embd_descr.size(); i++)
    {
      ss << "encoder_input_categories_" << cat_embd_descr[i].m_name
         << " = forward module=embd_" << cat_embd_descr[i].m_name
         << " input=" << cat_embd_descr[i].m_name << std::endl;
    }

    ss << "categories_embd_brut = cat input=";
    for (size_t i = 0; i < cat_embd_descr.size(); i++)
    {
      ss << "encoder_input_categories_" << cat_embd_descr[i].m_name;
      if (i < cat_embd_descr.size() - 1)
      {
        ss << ",";
      }
    }
    ss << " dim=1" << std::endl;

    // for decoder
    ss << "categories_embd = forward module=cat_embd_dropout input=categories_embd_brut" << std::endl;
    ss << std::endl;
    ss << "categories_encoded = forward module=fc_cat2decoder input=categories_embd" << endl;

    // for encoder
    ss << "categories_enc_embd = forward module=cat_embd_2_encoder_dropout input=categories_embd_brut" << std::endl;
    ss << std::endl;
    ss << "encoder_init_state_ = forward module=fc_cat2encoder input=categories_enc_embd" << endl;
    ss << "encoder_init_state = reshape input=encoder_init_state_ dims=2,-1,"
       << encoder_input_size / 2 << endl;
  }

  ss << std::endl;

  for (size_t i = 0; i < encoder_embd_descr.size(); i++)
  {
    if (1 == encoder_embd_descr[i].m_type)
    {
      ss << "encoder_input_" << encoder_embd_descr[i].m_name
         << " = forward module=embd_"
         << encoder_embd_descr[i].m_name
         << " input=" << encoder_embd_descr[i].m_name << std::endl;
    }
  }

  ss << std::endl;

  ss << "encoder_input_brut = cat input=";
  for (size_t i = 0; i < encoder_embd_descr.size(); i++)
  {
    if (0 == encoder_embd_descr[i].m_type)
    {
      ss << encoder_embd_descr[i].m_name;
    }
    else if (1 == encoder_embd_descr[i].m_type)
    {
      ss << "encoder_input_" << encoder_embd_descr[i].m_name;
    }
    if (i < encoder_embd_descr.size() - 1)
    {
      ss << ",";
    }
  }
  ss << " dim=2" << std::endl;

  ss << std::endl;

  ss << "encoder_input = forward module=encoder_embd_dropout input=encoder_input_brut" << std::endl;

  ss << std::endl;

  std::string last_output_name = "encoder_input";
  for (size_t i = 0; i < encoder_rnn_descr.size(); i++)
  {
    ss << "encoder_rnn_out_" << i
       << ",encoder_rnn_h_" << i
       << ",encoder_rnn_c_" << i
       << " = forward module=encoder_lstm_" << i
       << " input=" << last_output_name << ",encoder_init_state,encoder_init_state" << std::endl;
    std::stringstream t;
    t << "encoder_rnn_out_" << i;
    last_output_name = t.str();
  }

  // encoder_rnn_h_X = [2*num_layers, batch_size, enc_hidden_size]
  // encoder_rnn_c_X = [2*num_layers, batch_size, enc_hidden_size]

  // decoder_rnn_h_X = [1*num_layers, batch_size, dec_hidden_size]
  // decoder_rnn_c_X = [1*num_layers, batch_size, dec_hidden_size]

  ss << std::endl;

  ss << "encoder_final_concat = cat input=encoder_rnn_h_0,encoder_rnn_c_0 dim=2";
  ss << std::endl;
  // encoder_final_concat = [2*num_layers, batch_size, 2*enc_hidden_size]

  for (size_t i = 0; i < 2 * encoder_rnn_descr.size(); i++)
  {
    ss << "encoder_final_l" << i;
    if (i < 2 * encoder_rnn_descr.size() - 1)
    {
      ss << ",";
    }
  }
  ss << " = unbind input=encoder_final_concat dim=0" << endl;

  ss << "encoder_final_state_ = cat input=";
  for (size_t i = 0; i < 2 * encoder_rnn_descr.size(); i++)
  {
    ss << "encoder_final_l" << i;
    if (i < 2 * encoder_rnn_descr.size() - 1)
    {
      ss << ",";
    }
  }
  ss << " dim=1" << endl;

  // encoder_final_state = [batch_size, 4*enc_hidden_size]
  if (cat_embd_descr.size() > 0)
  {
    ss << "encoder_final_state = cat input=encoder_final_state_,categories_encoded dim=1" << std::endl << std::endl;
    ss << "decoder_h0_ = forward module=interm_fc_h0 input=encoder_final_state" << endl;
    ss << "decoder_c0_ = forward module=interm_fc_c0 input=encoder_final_state" << endl;
  }
  else
  {
    ss << "decoder_h0_ = forward module=interm_fc_h0 input=encoder_final_state_" << endl;
    ss << "decoder_c0_ = forward module=interm_fc_c0 input=encoder_final_state_" << endl;
  }

  ss << "decoder_h0 = unsqueeze input=decoder_h0_ dim=0" << endl;
  ss << "decoder_c0 = unsqueeze input=decoder_c0_ dim=0" << endl;

  ss << std::endl;

  // Decoder forward pass
  for (size_t i = 0; i < decoder_embd_descr.size(); i++)
  {
    if (1 == decoder_embd_descr[i].m_type)
    {
      ss << "decoder_input_" << decoder_embd_descr[i].m_name
         << " = forward module=embd_"
         << decoder_embd_descr[i].m_name
         << " input=" << decoder_embd_descr[i].m_name << std::endl;
    }
  }

  ss << std::endl;

  ss << "decoder_input_brut = cat input=";
  for (size_t i = 0; i < decoder_embd_descr.size(); i++)
  {
    if (0 == decoder_embd_descr[i].m_type)
    {
      ss << decoder_embd_descr[i].m_name;
    }
    else if (1 == decoder_embd_descr[i].m_type)
    {
      ss << "decoder_input_" << decoder_embd_descr[i].m_name;
    }
    if (i < decoder_embd_descr.size() - 1)
    {
      ss << ",";
    }
  }
  ss << " dim=2" << std::endl;

  ss << std::endl;

  ss << "decoder_input = forward module=decoder_embd_dropout input=decoder_input_brut" << std::endl;

  ss << std::endl;

  last_output_name = "decoder_input";
  for (size_t i = 0; i < decoder_rnn_descr.size(); i++)
  {
    ss << "decoder_rnn_out_" << i
       << ",decoder_rnn_h_" << i
       << ",decoder_rnn_c_" << i
       << " = forward module=decoder_lstm_" << i
       << " input=" << last_output_name
       << ",decoder_h0,decoder_c0" // order: input,h0,c0
       << std::endl;
    std::stringstream t;
    t << "decoder_rnn_out_" << i;
    last_output_name = t.str();
  }

  ss << std::endl;

  for (size_t i = 0; i < output_names.size(); ++i)
  {
    ss << output_names[i] << "_raw = forward module=fc_" << output_names[i]
       << " input=" << last_output_name << std::endl;

    ss << output_names[i] << " = log_softmax input=" << output_names[i] << "_raw" << std::endl;
    ss << endl;
  }

  return ss.str();
}

} // namespace nets
} // namespace deeplima

