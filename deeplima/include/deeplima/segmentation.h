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

#ifndef DEEPLIMA_SEGMENTATION_H
#define DEEPLIMA_SEGMENTATION_H

#include "config.h"

#include "segmentation/impl/utf8_reader.h"
//#include "segmentation/impl/utf16_reader.h"
#include "segmentation/impl/char_ngram_encoder.h"
#include "segmentation/impl/segmentation_decoder.h"

#include "nets/birnn_seq_cls.h"

#if DEEPLIMA_INFERENCE_ENGINE == IE_TORCH

#include "torch_wrp/dict_vectorizer.h"
#include "tasks/segmentation/model/birnn_classifier_for_segmentation.h"
#include "static_graph/dict.h"

#endif

#if DEEPLIMA_INFERENCE_ENGINE == IE_EIGEN

#include "eigen_wrp/eigen_matrix.h"
#include "eigen_wrp/embd_dict.h"
#include "eigen_wrp/dict_embd_vectorizer.h"
#include "eigen_wrp/segmentation_eigen_inference_impl.h"

#endif

#include "segmentation/impl/segmentation_impl.h"
#include "segmentation/impl/segmentation_wrapper.h"

namespace deeplima
{
namespace segmentation
{

namespace impl
{
  typedef CharNgramEncoder< Utf8Reader<> > CharNgramEncoderFromUtf8;
  //typedef CharNgramEncoder< Utf16Reader<> > CharNgramEncoderFromUtf16;
}

#if DEEPLIMA_INFERENCE_ENGINE == IE_TORCH

namespace torch_impl
{
  typedef impl::SegmentationInferenceWrapper<train::BiRnnClassifierForSegmentationImpl> Model;
  typedef DictVectorizer<DictsHolder, UInt64Dict, TorchMatrix<int64_t>> EmbdVectorizer;
}

#elif DEEPLIMA_INFERENCE_ENGINE == IE_EIGEN

namespace eigen_impl
{
  typedef impl::SegmentationInferenceWrapper<BiRnnEigenInferenceForSegmentationF> Model;
  typedef DictEmbdVectorizer<EmbdUInt64FloatHolder, EmbdUInt64Float, eigen_wrp::EigenMatrixXf> EmbdVectorizer;
}

#else
#error Unknown inference engine
#endif

namespace impl
{
#if DEEPLIMA_INFERENCE_ENGINE == IE_TORCH

  typedef torch_impl::Model Model;
  typedef torch_impl::EmbdVectorizer EmbdVectorizer;

#elif DEEPLIMA_INFERENCE_ENGINE == IE_EIGEN

  typedef eigen_impl::Model Model;
  typedef eigen_impl::EmbdVectorizer EmbdVectorizer;

#else
#error Unknown inference engine
#endif

  typedef RnnSequenceClassifier<Model, EmbdVectorizer> SegmentationClassifier;

  typedef impl::SegmentationImpl< SegmentationClassifier,
                                  impl::CharNgramEncoderFromUtf8,
                                  impl::SegmentationDecoder<SegmentationClassifier::OutputMatrix> > SegmentationModuleUtf8;

  //typedef impl::SegmentationImpl< SegmentationClassifier,
  //                                impl::CharNgramEncoderFromUtf16,
  //                                impl::SegmentationDecoder<SegmentationClassifier::OutputMatrix> > SegmentationModuleUtf16;
}

typedef impl::SegmentationModuleUtf8 Segmentation;

}
}

#endif