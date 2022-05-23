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

