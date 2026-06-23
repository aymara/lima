# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../extern/libtorch/include)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../extern/libtorch/include/torch/csrc/api/include)

add_library(lemmatization_inference
  STATIC
  lemmatization_impl.cpp
)

target_link_libraries(lemmatization_inference
  PRIVATE
  ${TORCH_LIBRARIES}
  Eigen3::Eigen)

