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

#include "conllu_file_iterator.h"
#include "deeplima/torch_wrp/torch_matrix.h"

#include <iostream>
#include <random>

using namespace std;
using namespace torch;
using torch::indexing::Slice;

namespace deeplima
{
namespace graph_dp
{
namespace train
{

CoNLLUDataSet::CoNLLUDataSet(const CoNLLU::Annotation& annot,
                             size_t batch_size,
                             const ConlluFeatExtractor<CoNLLU::WordLevelAdapter::token_t>& feat_extractor,
                             const DictsHolder& morph_tag_dh,
                             shared_ptr<FeatureVectorizerBase<>> p_embd,
                             bool add_root)
  : m_add_root(add_root),
    m_batch_size(batch_size),
    m_morph_tag_dh(morph_tag_dh),
    m_annot(annot),
    m_feat_vectorizers({ p_embd }),
    m_feat_extractor(feat_extractor)
{
  for (auto p_embd : m_feat_vectorizers)
  {
    m_feat_descr.push_back({ CoNLLUToTorchMatrix::str_feature, "form", p_embd.get() });
  }

  for (size_t i = 0; i < m_morph_tag_dh.size(); ++i)
  {
    if (morph_tag_dh[i]->size() > 0)
    {
      m_embd_feat_descr.push_back({
                                    CoNLLUToTorchMatrix::str_feature,
                                    m_feat_extractor.feats()[i],
                                    8,
                                    morph_tag_dh[i]
                                  });
    }
  }
}

void CoNLLUDataSet::init()
{
  //load_embeddings();
  vectorize();
}

std::vector<nets::embd_descr_t> CoNLLUDataSet::get_embd_descr()
{
  assert(m_feat_descr.size() > 0 || m_embd_feat_descr.size() > 0);
  CoNLLUToTorchMatrix vectorizer(m_feat_descr, m_embd_feat_descr, m_feat_extractor);
  return vectorizer.get_embd_descr();
}

/*typedef FastTextVectorizer<TorchMatrix<float>> FastTextVectorizerToTorchMatrix;

void CoNLLUFileIterator::load_embeddings()
{
  for (const string& fn : m_embd_file_names)
  {
    try
    {
      shared_ptr<FastTextVectorizerToTorchMatrix> p_embd
          = shared_ptr<FastTextVectorizerToTorchMatrix>(
              new FastTextVectorizerToTorchMatrix(fn)
            );
      assert(nullptr != p_embd.get());

      m_feat_vectorizers.push_back(p_embd);
      m_feat_descr.push_back({ CoNLLUToTorchMatrix::str_feature, "form", p_embd.get() });
    }
    catch (const exception& e)
    {
      cerr << "CoNLLUFileIterator::load_embeddings: " << e.what() << endl;
      throw e;
    }
    catch (...)
    {
      throw runtime_error("CoNLLUFileIterator::load_embeddings: something wrong happened while loading \"" + fn + "\"");
    }
  }
}*/

void CoNLLUDataSet::vectorize()
{
  assert(m_feat_descr.size() > 0 || m_embd_feat_descr.size() > 0);
  CoNLLUToTorchMatrix vectorizer(m_feat_descr, m_embd_feat_descr, m_feat_extractor);

  // sort sentences by length (bucketing)
  vector<vector<size_t>> sent_by_len;
  sent_by_len.resize(256);

  for (size_t i = 0; i < m_annot.get_num_sentences(); i++)
  {
    const CoNLLU::Sentence& sent = m_annot.get_sentence(i);
    size_t len = sent.calc_num_of_words(m_annot);
    assert(len > 0);
    if (len < 4)
    {
      // nothing to train :)
      continue;
    }
    if (len > sent_by_len.size() - 1)
    {
      sent_by_len.resize(len + 1);
    }
    sent_by_len[len].push_back(i);
  }

  m_bucket_keys.reserve(sent_by_len.size());
  for (size_t i = 0; i < sent_by_len.size(); i++)
  {
    const vector<size_t>& sents = sent_by_len[i];
    if (sents.empty())
    {
      continue;
    }

    size_t k = vectorize_bucket(i, sents, vectorizer);
    m_bucket_keys.push_back(k);
  }
}

size_t CoNLLUDataSet::vectorize_bucket(size_t len, const vector<size_t>& sents, const CoNLLUToTorchMatrix& vectorizer)
{
  uint64_t timepoint = 0;
  uint64_t timepoints_per_sentence = m_add_root ? len + 1 : len;
  uint64_t total_timepoints = timepoints_per_sentence * sents.size();
  m_input_buckets[timepoints_per_sentence] = vectorizer.init_dst(total_timepoints);
  m_gold_buckets[timepoints_per_sentence]
      = std::shared_ptr<TorchMatrix<int64_t>>(new TorchMatrix<int64_t>(total_timepoints, 1));

  CoNLLU::Annotation m_root_annot;
  stringstream root_line;
  root_line << "1\t<ROOT>\t<ROOT>\tROOT\t_\t_\t0\troot\t0:root\t_\n";
  m_root_annot.load(root_line);
  CoNLLU::BoundedWordLevelAdapter root_generator(&m_root_annot, 0, 1);

  for (size_t i = 0; i < sents.size(); i++)
  {
    const CoNLLU::Sentence& sent = m_annot.get_sentence(sents[i]);

    CoNLLU::BoundedWordLevelAdapter adapter(&m_annot, sent.get_first_word_idx(), len);

    if (m_add_root)
    {
      vectorizer.process(root_generator, m_input_buckets[timepoints_per_sentence], timepoint);
    }

    vectorizer.process(adapter, m_input_buckets[timepoints_per_sentence], m_add_root ? timepoint+1 : timepoint);
    vectorize_bucket_gold(adapter, *(m_gold_buckets[timepoints_per_sentence].get()), timepoint);

    timepoint += timepoints_per_sentence;
  }

  return timepoints_per_sentence;
}

void CoNLLUDataSet::vectorize_bucket_gold(const CoNLLU::BoundedWordLevelAdapter& src,
                                               TorchMatrix<int64_t>& dst,
                                               uint64_t timepoint)
{
  typename CoNLLU::WordLevelAdapter::const_iterator it = src.begin();
  uint64_t current_timepoint = timepoint;
  while (src.end() != it)
  {
    while(!(*it).is_word() && src.end() != it)
    {
      it++;
    }

    if (src.end() == it)
    {
      break;
    }

    const CoNLLU::idx_t& head = (*it).head();
    const std::string& gold_rel = (*it).deprel();
    assert(head.is_real_word());

    const CoNLLU::idx_t idx = (*it).idx();
    if (m_add_root && 1 == idx._first)
    {
      dst.set(current_timepoint, 0, 0);
      current_timepoint++;
    }

    dst.set(current_timepoint, 0, head._first);

    /*if (head._first == 0)
    {
      dst.set(current_timepoint, 0, idx._first - 1);
    }
    else
    {
      dst.set(current_timepoint, 0, head._first - 1);
    }*/

    current_timepoint++;
    if (current_timepoint == std::numeric_limits<int64_t>::max())
    {
      throw std::overflow_error("Too much words in the dataset.");
    }

    it++;
  }
}

shared_ptr<BatchIterator> CoNLLUDataSet::get_iterator() const
{
  return shared_ptr<Iterator>(new Iterator(*this));
}

void CoNLLUDataSet::Iterator::set_batch_size(int64_t batch_size)
{
  m_batch_size = batch_size;
}

void CoNLLUDataSet::Iterator::start_epoch()
{
  if (0 == m_batch_size)
  {
    throw;
  }
  m_current_bucket = 0;
  m_iter_counter = 0;
}

bool CoNLLUDataSet::Iterator::end()
{
  return m_current_bucket >= m_dataset.m_bucket_keys.size();
}

const CoNLLUDataSet::Iterator::Batch CoNLLUDataSet::Iterator::next_batch()
{
  size_t k = m_dataset.m_bucket_keys[m_current_bucket];

  auto it_input = m_dataset.m_input_buckets.find(k);
  assert(m_dataset.m_input_buckets.end() != it_input);
  const CoNLLUToTorchMatrix::vectorization_t& input = it_input->second;

  auto it_gold = m_dataset.m_gold_buckets.find(k);
  assert(m_dataset.m_gold_buckets.end() != it_gold);
  const TorchMatrix<int64_t>& gold_bucket = *(it_gold->second);
  //cerr << "gold_bucket.sizes() == " << gold_bucket.get_tensor().sizes() << endl;
  //cerr << "gold_bucket.get_tensor().size(0) == " << gold_bucket.get_tensor().size(0) << endl;

  int64_t seq_len = it_input->first;

  //cerr << "input.first->get_tensor().sizes() == " << input.first->get_tensor().sizes() << endl;
  //cerr << "input.second->get_tensor().sizes() == " << input.second->get_tensor().sizes() << endl;

  int64_t batch_size = m_batch_size;
  if (-1 == batch_size)
  {
    assert(0 == input.first->get_tensor().size(0) % seq_len);
    batch_size = input.first->get_tensor().size(0) / seq_len; // batch == full bucket
  }

  //std::cerr << gold_bucket.get_tensor().sizes() << std::endl;

  std::random_device r;
  std::default_random_engine e1(r());
  int64_t max_start_offset = ( gold_bucket.get_tensor().size(0) / seq_len ) - batch_size;
  if (max_start_offset < 0)
  {
    max_start_offset = 0;
  }
  std::uniform_int_distribution<int> uniform_dist(0, max_start_offset);
  int64_t batch_start_offset = uniform_dist(e1);

  if (0 == batch_start_offset &&
      seq_len * (batch_start_offset + batch_size) > gold_bucket.get_tensor().size(0))
  {
    m_current_bucket++;
    return CoNLLUDataSet::Iterator::Batch();
  }

  /*std::cerr << "seq_len = " << seq_len
            << " iter " << m_iter_counter
            << " from " << batch_start_offset << " until "
            << gold_bucket.get_tensor().size(0) / seq_len << std::endl;*/
  const torch::Tensor trainable
      = input.first->get_tensor().index({ Slice(seq_len * batch_start_offset,
                                                seq_len * (batch_start_offset + batch_size)),
                                          Slice() }).reshape({ batch_size, seq_len, -1 }).transpose(0, 1);
  const torch::Tensor frozen
      = input.second->get_tensor().index({ Slice(seq_len * batch_start_offset,
                                                 seq_len * (batch_start_offset + batch_size)),
                                           Slice() }).reshape({ batch_size, seq_len, -1 }).transpose(0, 1);

  //cerr << "trainable.sizes() == " << trainable.sizes() << endl;
  //cerr << "frozen.sizes() == " << frozen.sizes() << endl;

  const torch::Tensor gold
      = gold_bucket.get_tensor().index({ Slice(seq_len * batch_start_offset,
                                               seq_len * (batch_start_offset + batch_size)),
                                         Slice() }).reshape({ batch_size, seq_len, -1 });//.transpose(0, 1);
  const CoNLLUDataSet::Iterator::Batch batch(trainable, frozen, gold, 1);

  // advance pointers
  if (m_batch_size >= 0)
  {
    m_iter_counter++;
    //std::cerr << gold_bucket.get_tensor().size(0) << std::endl;
    if (m_iter_counter * m_batch_size > gold_bucket.get_tensor().size(0) / seq_len)
    {
      m_current_bucket++;
      m_iter_counter = 0;
    }
  }
  else
  {
    m_current_bucket++;
    m_iter_counter = 0;
  }

  return batch;
}

} // train
} // graph_dp
} // deeplima
