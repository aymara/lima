// Copyright 2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_TASKS_GRAPH_DP_TRAIN_GRAPH_DP_H
#define DEEPLIMA_LIBS_TASKS_GRAPH_DP_TRAIN_GRAPH_DP_H

#include <string>

#include "tasks/graph_dp/model/train_params_graph_dp.h"

namespace deeplima
{
namespace graph_dp
{
namespace train
{

int train_graph_dp(const train_params_graph_dp_t& params);

} // namespace train
} // namespace tagging
} // namespace deeplima

#endif
