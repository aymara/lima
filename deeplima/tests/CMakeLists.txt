# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT
# Unit test for the biaffine label decoder. Built only in standalone deeplima
# builds (see the guard around add_subdirectory(tests) in ../CMakeLists.txt).
add_executable(test_label_decoder test_label_decoder.cpp)
target_link_libraries(test_label_decoder PRIVATE torch_modules ${TORCH_LIBRARIES})
add_test(NAME test_label_decoder COMMAND test_label_decoder)
