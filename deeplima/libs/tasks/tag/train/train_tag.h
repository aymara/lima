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

#ifndef DEEPLIMA_LIBS_TASKS_NER_TRAIN_TRAIN_NER_H
#define DEEPLIMA_LIBS_TASKS_NER_TRAIN_TRAIN_NER_H

#include <string>

#include "tasks/tag/model/train_params_tagging.h"

namespace deeplima
{
namespace tagging
{
namespace train
{

int train_entity_tagger(const train_params_tagging_t& params);

} // namespace train
} // namespace tagging
} // namespace deeplima

#endif
