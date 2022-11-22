# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_executable(deeplima-train-segm
  train-segm.cpp
)

target_link_libraries(deeplima-train-segm
  PRIVATE
  conllu
  Boost::program_options
  Boost::filesystem
  ICU::uc
  train_segmentation
  version
)

install(TARGETS deeplima-train-segm
  DESTINATION bin
  COMPONENT runtime
)

############################################################
# Train tagging models

add_executable(deeplima-train-tag
  train-tag.cpp
)

target_link_libraries(deeplima-train-tag
  PRIVATE
  train_tag
  conllu
  Boost::program_options
  Boost::filesystem
  version
)

install(TARGETS deeplima-train-tag
  DESTINATION bin
  COMPONENT runtime
)

############################################################
# Train lemmatization models

add_executable(deeplima-train-lemmatization
  train-lemmatization.cpp
)

target_link_libraries(deeplima-train-lemmatization
  PRIVATE
  train_lemmatization
  conllu
  Boost::program_options
  Boost::filesystem
  version
)

install(TARGETS deeplima-train-lemmatization
  DESTINATION bin
  COMPONENT runtime
)

############################################################
# Train dependency parsing models

add_executable(deeplima-train-dp
  train-dp.cpp
)

target_link_libraries(deeplima-train-dp
  PRIVATE
  train_graph_dp
  conllu
  Boost::program_options
  Boost::filesystem
  version
)

install(TARGETS deeplima-train-dp
  DESTINATION bin
  COMPONENT runtime
)

############################################################
# deeplima (with eigen as inference engine)

add_executable(deeplima-eigen
  deeplima.cpp
)

set_target_properties(deeplima-eigen
  PROPERTIES
  OUTPUT_NAME deeplima
)

target_compile_definitions(deeplima-eigen
  PUBLIC
  DEEPLIMA_INFERENCE_ENGINE=IE_EIGEN
)

target_link_libraries(deeplima-eigen
  PRIVATE
  Boost::program_options
  ICU::uc
  Eigen3::Eigen
  convert_from_torch
  ${TORCH_LIBRARIES}
  fasttext-static
  version
  utils
)

install(TARGETS deeplima-eigen
  DESTINATION bin
  COMPONENT runtime
)

############################################################
# deeplima (with libtorch as inference engine)

#add_executable(deeplima-torch
#  deeplima.cpp
#)

#target_compile_definitions(deeplima-torch
#  PUBLIC
#  DEEPLIMA_INFERENCE_ENGINE=IE_TORCH
#)

#target_link_libraries(deeplima-torch
#  PRIVATE
#  Boost::program_options
#  ICU::uc
#  birnn_classifier_for_segmentation
#  ${TORCH_LIBRARIES}
#  fasttext-static
#)
