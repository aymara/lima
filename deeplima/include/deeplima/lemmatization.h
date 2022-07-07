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

#ifndef DEEPLIMA_LEMMATIZATION_H
#define DEEPLIMA_LEMMATIZATION_H

#include "config.h"

#include "nets/birnn_seq2seq.h"

#if DEEPLIMA_INFERENCE_ENGINE == IE_TORCH

#endif

#if DEEPLIMA_INFERENCE_ENGINE == IE_EIGEN

#include "eigen_wrp/eigen_matrix.h"
#include "eigen_wrp/lemmatization_eigen_inference_impl.h"

#endif

#include "lemmatization/impl/lemmatization_impl.h"
#include "lemmatization/impl/lemmatization_wrapper.h"

namespace deeplima
{
namespace lemmatization
{

#if DEEPLIMA_INFERENCE_ENGINE == IE_TORCH

namespace torch_impl
{
} // namespace torch_impl

#elif DEEPLIMA_INFERENCE_ENGINE == IE_EIGEN

namespace eigen_impl
{
  typedef impl::LemmatizationInferenceWrapper<BiRnnSeq2SeqEigenInferenceForLemmatizationF> Model;
} // namespace eigen_impl

#else
#error Unknown inference engine
#endif

namespace impl
{
#if DEEPLIMA_INFERENCE_ENGINE == IE_TORCH

#elif DEEPLIMA_INFERENCE_ENGINE == IE_EIGEN

  typedef eigen_impl::Model Model;

  typedef eigen_wrp::EigenMatrixXf BaseMatrix;

#else
#error Unknown inference engine
#endif

  typedef RnnSeq2Seq<Model, BaseMatrix> Lemmatizer;
} // namespace impl
} // namespace lemmatization
} // namespace deeplima

#endif // DEEPLIMA_LEMMATIZATION_H
