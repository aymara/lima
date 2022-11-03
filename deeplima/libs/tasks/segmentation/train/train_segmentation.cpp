// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <vector>
#include <iostream>

#include "deeplima/segmentation/impl/utf8_reader.h"
#include "deeplima/segmentation/impl/segmentation_decoder.h"

#include "deeplima/torch_wrp/torch_matrix.h"
#include "static_graph/dict_holder_adapter.h"

#include "tasks/segmentation/model/birnn_classifier_for_segmentation.h"

#include "char_dict_builder.h"
#include "char_seq_vectorizer.h"
#include "train_segmentation.h"

using namespace std;

using namespace deeplima::segmentation;
using namespace deeplima::segmentation::train;
using namespace deeplima::segmentation::impl;
using namespace deeplima::nets;
using namespace deeplima;

template <class M>
std::shared_ptr<M> vectorize_gold(const CoNLLU::Annotation& annot, int64_t len, bool eos)
{
  std::shared_ptr<M> out(new M(len, 1));

  auto tokens = annot.get_tokens();
  int64_t p = 0;
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
      throw;
    }

    if (t.m_len == 1)
    {
      if (eos && t.eos())
      {
        out->set(p, 0, segm_tag_t::S_EOS);
      }
      else
      {
        out->set(p, 0, segm_tag_t::S);
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
      out->set(p, 0, segm_tag_t::E_EOS);
    }
    else
    {
      out->set(p, 0, segm_tag_t::E);
    }

    p++;
  }

  return out;
}

typedef DictionaryBuilderImpl< CharNgramEncoder< Utf8Reader<> > > DictionaryBuilder;
typedef CharSeqVectorizerImpl< CharNgramEncoder< Utf8Reader<> >, TorchMatrix<int64_t>,
                               DictHolderAdapter< UInt64Dict, TorchMatrix<int64_t> > > Utf8CharSeqToTorchMatrix;

int train_segmentation_model(const CoNLLU::Treebank& tb, const string& model_name, bool train_ss)
{
  vector<ngram_descr_t> ngram_descr = { { 0,  1, ngram_descr_t::char_ngram },
                                        { 0,  2, ngram_descr_t::char_ngram },
                                        { -1, 3, ngram_descr_t::char_ngram },
                                        { 0,  1, ngram_descr_t::type_ngram },
                                        { 0,  2, ngram_descr_t::type_ngram },
                                        { -1, 3, ngram_descr_t::type_ngram },
                                        { 0,  1, ngram_descr_t::script_ngram } };

  const CoNLLU::Document& train_doc = tb.get_doc("train");
  uint64_t train_char_counter = 0;

  DictionaryBuilder dict_builder(ngram_descr);
  DictsHolder dicts
      = dict_builder.process(train_doc.get_original_text(), 100, train_char_counter);

  if (train_char_counter >= std::numeric_limits<int64_t>::max())
  {
    throw std::overflow_error("Too much characters in training set.");
  }

  Utf8CharSeqToTorchMatrix vectorizer(ngram_descr);
  vectorizer.set_dicts(dicts);
  shared_ptr<TorchMatrix<int64_t>> train_input = vectorizer.process(train_doc.get_original_text(),
                                                                    (int64_t)train_char_counter);

  const CoNLLU::Document& dev_doc = tb.get_doc("dev");
  shared_ptr<TorchMatrix<int64_t>> dev_input = vectorizer.process(dev_doc.get_original_text(),
                                                                  dev_doc.get_text().size() + 1);

  shared_ptr<TorchMatrix<int64_t>> train_gold
      = vectorize_gold<TorchMatrix<int64_t>>(tb.get_annot("train"),
                                             (int64_t)train_char_counter, train_ss);

  shared_ptr<TorchMatrix<int64_t>> dev_gold
      = vectorize_gold<TorchMatrix<int64_t>>(tb.get_annot("dev"),
                                             dev_doc.get_text().size() + 1, train_ss);

  std::vector<embd_descr_t> embd_descr = { { "char1gram", 2 }, { "char2gram", 3 }, { "char3gram", 4 },
                                           { "class1gram", 2 }, { "class2gram", 2 }, { "class3gram", 2 },
                                           { "scriptchange", 1 } };

  std::vector<rnn_descr_t> rnn_descr = { rnn_descr_t( 8 ) };
  BiRnnClassifierForSegmentation model(std::move(dicts), ngram_descr, embd_descr,
                                       rnn_descr, "tokens", train_ss ? 7 : 5, 0.1);

  const double learning_rate = 0.001;
  torch::optim::Adam optimizer(model->parameters(), torch::optim::AdamOptions(learning_rate));

  torch::Device device("cuda");

  train_input->to(device);
  train_gold->to(device);

  dev_input->to(device);
  dev_gold->to(device);

  model->to(device);

  model->train(300, 4, 256, { "tokens" },
              *(train_input.get()), *(train_gold.get()),
              *(dev_input.get()), *(dev_gold.get()),
              optimizer, model_name, device);

  return 0;
}

