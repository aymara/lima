// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef TRAIN_SEGMENTATION_H
#define TRAIN_SEGMENTATION_H

#include <string>

#include "conllu/treebank.h"
#include "tasks/segmentation/model/train_params_segmentation.h"

int train_segmentation_model (const deeplima::CoNLLU::Treebank & tb,
                              deeplima::segmentation::train::train_params_segmentation_t & params);

#endif
