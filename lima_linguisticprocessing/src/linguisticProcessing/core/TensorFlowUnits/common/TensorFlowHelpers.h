/*
    Copyright 2002-2019 CEA LIST

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
