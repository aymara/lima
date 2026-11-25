// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT


#include "deeplima/segmentation/impl/utf8_reader.h"
#include "deeplima/segmentation/impl/segmentation_decoder.h"

#include "deeplima/torch_wrp/torch_matrix.h"
#include "static_graph/dict_holder_adapter.h"

#include "tasks/segmentation/model/birnn_classifier_for_segmentation.h"

#include "char_dict_builder.h"
#include "char_seq_vectorizer.h"
#include "train_segmentation.h"

#include <c10/util/Exception.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <memory>
#include <vector>

using namespace std;

using namespace deeplima::segmentation;
using namespace deeplima::segmentation::train;
using namespace deeplima::segmentation::impl;
using namespace deeplima::nets;
using namespace deeplima;

template <class M>
std::shared_ptr<M> vectorize_gold(const CoNLLU::Annotation& annot, int64_t len, bool eos, bool mwt)
{
  auto out = std::make_shared<M>(len, 1);

  const auto& tokens = annot.get_tokens();
  size_t p = 0;
  for (size_t i = 0; i < tokens.size(); i++)
  {
    const auto& t = tokens[i];
    while (p < t.m_pos)
    {
      out->set(p, 0, segm_tag_t::X);
      p++;
    }

    if (t.m_len == 0)
    {
      throw std::runtime_error("vectorize_gold length should be 0");
    }

    // A surface token that is a multiword token (e.g. "du") gets the MWT
    // variant of its end tag so the model learns to predict, in context, which
    // tokens should be expanded into sub-words.
    const bool is_mwt = mwt && t.is_multiword();

    if (t.m_len == 1)
    {
      if (eos && t.eos())
      {
        out->set(p, 0, is_mwt ? segm_tag_t::S_EOS_MWT : segm_tag_t::S_EOS);
      }
      else
      {
        out->set(p, 0, is_mwt ? segm_tag_t::S_MWT : segm_tag_t::S);
      }
      p++;
      continue;
    }

    out->set(p, 0, segm_tag_t::B);
    p++;
    while (p < t.m_pos + t.m_len - 1)
    {
      out->set(p, 0, segm_tag_t::I);
      p++;
    }
    if (eos && t.eos())
    {
      out->set(p, 0, is_mwt ? segm_tag_t::E_EOS_MWT : segm_tag_t::E_EOS);
    }
    else
    {
      out->set(p, 0, is_mwt ? segm_tag_t::E_MWT : segm_tag_t::E);
    }

    p++;
  }

  return out;
}

typedef DictionaryBuilderImpl< CharNgramEncoder< Utf8Reader<> > > DictionaryBuilder;
typedef CharSeqVectorizerImpl< CharNgramEncoder< Utf8Reader<> >, TorchMatrix<int64_t>,
                               DictHolderAdapter< UInt64Dict, TorchMatrix<int64_t> > > Utf8CharSeqToTorchMatrix;

int train_segmentation_model(const CoNLLU::Treebank& tb,
                             deeplima::segmentation::train::train_params_segmentation_t &params,
                             int gpuid)
{
  std::vector<ngram_descr_t> ngram_descr = {
                                        { 0,  1, ngram_descr_t::char_ngram },
                                        { 0,  2, ngram_descr_t::char_ngram },
                                        { -1, 3, ngram_descr_t::char_ngram },
                                        { 0,  1, ngram_descr_t::type_ngram },
                                        { 0,  2, ngram_descr_t::type_ngram },
                                        { -1, 3, ngram_descr_t::type_ngram },
                                        { 0,  1, ngram_descr_t::script_ngram } };

  const size_t c = params.m_char_embd_dim;
  std::vector<embd_descr_t> embd_descr = { { "char1gram", c ? c : 2 }, { "char2gram", c ? c : 3 },
                                           { "char3gram", c ? c : 4 },
                                           { "class1gram", 2 }, { "class2gram", 2 }, { "class3gram", 2 },
                                           { "scriptchange", 1 } };

  // The lexicon feature: words of an external list ending at each character
  std::shared_ptr<SegmLexicon> lexicon;
  if (!params.m_lexicon_fn.empty())
  {
    lexicon = std::make_shared<SegmLexicon>(params.m_lexicon_fn);
    std::cerr << "Lexicon feature: " << lexicon->size() << " words from " << params.m_lexicon_fn
              << std::endl;
    ngram_descr.emplace_back(0, 1, ngram_descr_t::lexicon_ngram);
    embd_descr.emplace_back("lexicon", 4);
    // Beside the model, where SegmentationImpl::load finds it
    std::ifstream src(params.m_lexicon_fn, std::ios::binary);
    std::ofstream dst(params.m_output_model_name + ".pt.lex", std::ios::binary);
    dst << src.rdbuf();
    if (!dst)
    {
      throw std::runtime_error("Can't write " + params.m_output_model_name + ".pt.lex");
    }
  }

  const CoNLLU::Document& train_doc = tb.get_doc("train");
  uint64_t train_char_counter = 0;

  DictionaryBuilder dict_builder(ngram_descr);
  dict_builder.set_lexicon(lexicon.get());
  auto dicts = dict_builder.process(train_doc.get_original_text(), params.m_min_ipm,
                                    train_char_counter);

  if (train_char_counter >= std::numeric_limits<int64_t>::max())
  {
    throw std::overflow_error("Too much characters in training set.");
  }

  Utf8CharSeqToTorchMatrix vectorizer(ngram_descr);
  vectorizer.set_lexicon(lexicon.get());
  vectorizer.set_dicts(dicts);
  auto train_input = vectorizer.process(train_doc.get_original_text(),
                                        (int64_t)train_char_counter);

  const auto& dev_doc = tb.get_doc("dev");
  auto dev_input = vectorizer.process(dev_doc.get_original_text(), dev_doc.get_text().size() + 1);

  auto train_gold = vectorize_gold<TorchMatrix<int64_t>>(tb.get_annot("train"), (int64_t)train_char_counter,
                                                         params.train_ss, params.train_mwt);

  auto dev_gold = vectorize_gold<TorchMatrix<int64_t>>(tb.get_annot("dev"), dev_doc.get_text().size() + 1,
                                                       params.train_ss, params.train_mwt);

  std::vector<rnn_descr_t> rnn_descr = { rnn_descr_t( params.m_rnn_hidden_dim ) };


  // Class count: 5 (tokenization only), 7 (+ sentence segmentation), or 11
  // (+ the four multiword-token end-tag variants). MWT implies sentence seg.
  const uint32_t num_classes = params.train_mwt ? 11 : (params.train_ss ? 7 : 5);

  BiRnnClassifierForSegmentation model(std::move(dicts),
                                       ngram_descr,
                                       embd_descr,
                                       rnn_descr,
                                       "tokens",
                                       num_classes,
                                       params.m_input_dropout_prob);

  // Sentence ends: their loss weight, their F1 at evaluation, the epoch choice
  if (params.train_ss)
  {
    std::vector<int64_t> eos = { segm_tag_t::E_EOS, segm_tag_t::S_EOS };
    if (params.train_mwt)
    {
      eos.push_back(segm_tag_t::E_EOS_MWT);
      eos.push_back(segm_tag_t::S_EOS_MWT);
    }
    model->set_eos_classes(eos, params.m_select_eos);
    if (params.m_eos_weight != 1.0f)
    {
      std::vector<float> weights(num_classes, 1.0f);
      for (int64_t k : eos)
      {
        weights[k] = params.m_eos_weight;
      }
      model->set_class_weights(weights);
    }
  }
  model->set_schedule(params.m_lr_decay, params.m_max_epochs_without_improvement);

  torch::optim::Adam optimizer(model->parameters(),
                               torch::optim::AdamOptions(params.m_learning_rate)
                                  .weight_decay(params.m_weight_decay)
                                  .betas({params.m_beta_one, params.m_beta_two}));

  std::string dev = "cpu";
  if (gpuid >= 0)
  {
    std::ostringstream oss;
    oss << "cuda:" << gpuid;
    dev = oss.str();
  }
  torch::Device device(dev);

  train_input->to(device);
  train_gold->to(device);

  dev_input->to(device);
  dev_gold->to(device);

  model->to(device);

  try
  {
    model->train(params.m_max_epochs, params.m_batch_size, params.m_sequence_length, { "tokens" },
                *(train_input.get()), *(train_gold.get()),
                *(dev_input.get()), *(dev_gold.get()),
                optimizer, params.m_output_model_name, device);
  }
  catch (const c10::Error& e)
  {
    std::cerr << "Exception in model training: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

