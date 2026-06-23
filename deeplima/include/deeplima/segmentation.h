// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SEGMENTATION_H
#define DEEPLIMA_SEGMENTATION_H

#include "config.h"

#include "segmentation/impl/utf8_reader.h"
//#include "segmentation/impl/utf16_reader.h"
#include "segmentation/impl/char_ngram_encoder.h"
#include "segmentation/impl/segmentation_decoder.h"

#include "nets/birnn_seq_cls.h"

#include "eigen_wrp/eigen_matrix.h"
#include "eigen_wrp/embd_dict.h"
#include "eigen_wrp/dict_embd_vectorizer.h"
#include "eigen_wrp/segmentation_eigen_inference_impl.h"

#include "segmentation/impl/segmentation_impl.h"
#include "segmentation/impl/segmentation_wrapper.h"


#endif
