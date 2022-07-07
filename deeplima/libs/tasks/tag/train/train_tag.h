// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
