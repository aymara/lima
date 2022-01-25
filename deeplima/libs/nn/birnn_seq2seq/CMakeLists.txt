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
  birnn_seq2seq.cpp
  birnn_seq2seq_script_generator.cpp
)

set(HEADER_LIST
  birnn_seq2seq.h
)

add_library(torch_birnn_seq2seq
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

target_link_libraries(torch_birnn_seq2seq
  PRIVATE
  torch_birnn_seq_classifier
  ${TORCH_LIBRARIES}
)

