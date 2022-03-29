# Copyright 2021 CEA LIST
#
# This file is part of LIMA.
#
# LIMA is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LIMA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with LIMA.  If not, see <https://www.gnu.org/licenses/>

set(SOURCE_LIST
  static_graph.cpp
  dict.cpp
)

set(HEADER_LIST
  static_graph.h
  dict_base.h
  dict.h
)

add_library(static_graph
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

target_link_libraries(static_graph
  PRIVATE
  ${TORCH_LIBRARIES}
)


