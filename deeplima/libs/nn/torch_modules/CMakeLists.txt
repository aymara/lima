# Copyright 2022 CEA LIST
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
  deep_biaffine_attention_decoder.cpp
  deep_biaffine_attention_label_decoder.cpp
  stanza_models_depparse_model.cpp
)

set(HEADER_LIST
  deep_biaffine_attention_decoder.h
  deep_biaffine_attention_label_decoder.h
  stanza_models_depparse_model.h
)

add_library(torch_modules
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

target_link_libraries(torch_modules
  PRIVATE
  ${TORCH_LIBRARIES}
)

