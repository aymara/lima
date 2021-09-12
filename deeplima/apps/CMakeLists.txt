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
)

install(TARGETS deeplima-train-segm
  DESTINATION bin
  COMPONENT runtime
)

############################################################
# Train NER models

add_executable(deeplima-train-ner
  train-ner.cpp
)

target_link_libraries(deeplima-train-ner
  PRIVATE
  train_ner
  conllu
  Boost::program_options
  Boost::filesystem
)

install(TARGETS deeplima-train-ner
  DESTINATION bin
  COMPONENT runtime
)

############################################################
# deeplima (with eigen as inference engine)

add_executable(deeplima-eigen
  deeplima.cpp
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
