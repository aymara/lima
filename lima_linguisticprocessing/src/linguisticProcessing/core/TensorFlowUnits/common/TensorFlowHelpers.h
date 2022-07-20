// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_TENSORFLOWUNITS_TENSORFLOWHELPERS_H
#define LIMA_TENSORFLOWUNITS_TENSORFLOWHELPERS_H

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace Common
{

#define init_1d_tensor(t, v) \
{ \
  for (int64 i = 0; i < t.dimension(0); ++i) \
    t(i) = v; \
}

#define init_2d_tensor(t, v) \
{ \
  for (int64 i = 0; i < t.dimension(0); ++i) \
    for (int64 j = 0; j < t.dimension(1); ++j) \
      t(i, j) = v; \
}

#define init_3d_tensor(t, v) \
{ \
  for (int64 i = 0; i < t.dimension(0); ++i) \
    for (int64 j = 0; j < t.dimension(1); ++j) \
      for (int64 k = 0; k < t.dimension(2); ++k) \
	t(i, j, k) = v; \
}

}
}
}
}

#endif
