// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <string>

#include "conllu/treebank.h"

int train_segmentation_model(const deeplima::CoNLLU::Treebank& tb,
                             const std::string& model_name,
                             bool train_ss);

