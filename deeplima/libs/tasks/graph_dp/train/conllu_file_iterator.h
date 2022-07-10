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

#ifndef DEEPLIMA_LIBS_TASKS_GRAPH_DP_CONLLU_FILE_ITERATOR_H
#define DEEPLIMA_LIBS_TASKS_GRAPH_DP_CONLLU_FILE_ITERATOR_H

#include "tasks/graph_dp/model/iterable_dataset.h"
#include "conllu/treebank.h"
#include "deeplima/fastText_wrp/fastText_wrp.h"
#include "nn/common/word_seq_vectorizer.h"
#include "nn/common/layer_descr.h"
#include "deeplima/feat_extractors.h"

namespace deeplima
{
namespace graph_dp
{
namespace train
{

class CoNLLUDataSet : public IterableDataSet
{
public:

  CoNLLUDataSet(const CoNLLU::Annotation& annot, size_t batch_size, std::shared_ptr<FeatureVectorizerBase<>> p_embd)
    : m_batch_size(batch_size),
      m_annot(annot),
      m_feat_vectorizers({ p_embd })
  {
    for (auto p_embd : m_feat_vectorizers)
    {
      m_feat_descr.push_back({ CoNLLUToTorchMatrix::str_feature, "form", p_embd.get() });
    }
  }

  void init();

  std::vector<nets::embd_descr_t> get_embd_descr();

  class Iterator : public BatchIterator
  {
  private:
    Iterator(const CoNLLUDataSet& dataset)
      : m_dataset(dataset),
        m_batch_size(0),
        m_current_bucket(0),
        m_batch_start_offset(0)
    {
    }

  public:
    virtual void set_batch_size(size_t batch_size);
    virtual void start_epoch();
    virtual bool end();
    virtual const Batch next_batch();

  private:
    const CoNLLUDataSet& m_dataset;
    size_t m_batch_size;

    size_t m_current_bucket;
    size_t m_batch_start_offset;

    friend class CoNLLUDataSet;
  };

  virtual std::shared_ptr<BatchIterator> get_iterator() const;

private:
  const size_t m_batch_size;
  const CoNLLU::Annotation& m_annot;
  std::vector<std::shared_ptr<FeatureVectorizerBase<>>> m_feat_vectorizers;

  // Input features
  typedef nets::WordSeqVectorizerImpl<CoNLLU::WordLevelAdapter,
                                      deeplima::TokenStrFeatExtractor<CoNLLU::WordLevelAdapter::token_t>,
                                      deeplima::TokenUIntFeatExtractor<CoNLLU::WordLevelAdapter::token_t>,
                                      TorchMatrix<int64_t>,
                                      TorchMatrix<float> > CoNLLUToTorchMatrix;

  std::vector<CoNLLUToTorchMatrix::feature_descr_t> m_feat_descr;
  std::vector<CoNLLUToTorchMatrix::embeddable_feature_descr_t> m_embd_feat_descr;

  // Vectorized inputs
  std::vector<size_t> m_bucket_keys;
  std::map<size_t, CoNLLUToTorchMatrix::vectorization_t> m_input_buckets;

  // Vectorized gold values
  std::map<size_t, std::shared_ptr<TorchMatrix<int64_t>>> m_gold_buckets;

  //void load_embeddings();
  void vectorize();
  size_t vectorize_bucket(size_t len, const std::vector<size_t>& sents, const CoNLLUToTorchMatrix& vectorizer);
  void vectorize_bucket_gold(const CoNLLU::BoundedWordLevelAdapter& src,
                             TorchMatrix<int64_t>& dst,
                             uint64_t timepoint);
};

} // train
} // graph_dp
} // deeplima

#endif // DEEPLIMA_LIBS_TASKS_GRAPH_DP_CONLLU_FILE_ITERATOR_H