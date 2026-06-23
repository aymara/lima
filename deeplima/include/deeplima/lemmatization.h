// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

namespace impl
{

} // namespace impl
} // namespace lemmatization
} // namespace deeplima

#endif // DEEPLIMA_LEMMATIZATION_H
