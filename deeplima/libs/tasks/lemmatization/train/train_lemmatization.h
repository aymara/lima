// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_TASKS_LEMMATIZATION_TRAIN_TRAIN_LEMMATIZATION_H
#define DEEPLIMA_LIBS_TASKS_LEMMATIZATION_TRAIN_TRAIN_LEMMATIZATION_H

#include <string>

#include "tasks/lemmatization/model/train_params_lemmatization.h"

namespace deeplima
{
namespace lemmatization
{
namespace train
{

int train_lemmatization(const train_params_lemmatization_t& params);

} // namespace train
} // namespace lemmatization
} // namespace deeplima

#endif

