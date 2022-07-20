// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_MORPH_MODEL_BUILDER_H
#define DEEPLIMA_LIBS_MORPH_MODEL_BUILDER_H

#include <string>
#include <vector>

#include "conllu/treebank.h"

#include "morph_model.h"

namespace deeplima
{
namespace morph_model
{

class morph_model_builder
{
public:
  static morph_model_t build(const CoNLLU::Annotation& annotation1, const CoNLLU::Annotation& annotation2);
};

} // morph_model
} // deeplima

#endif

