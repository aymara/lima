# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# BEGIN FastText stuff

set(HEADER_FILES
    fastText/src/args.h
    #    fastText/src/autotune.h
    fastText/src/densematrix.h
    fastText/src/dictionary.h
    fastText/src/fasttext.h
    fastText/src/loss.h
    fastText/src/matrix.h
    fastText/src/meter.h
    fastText/src/model.h
    fastText/src/productquantizer.h
    fastText/src/quantmatrix.h
    fastText/src/real.h
    fastText/src/utils.h
    fastText/src/vector.h)

set(SOURCE_FILES
    fastText/src/args.cc
    #    fastText/src/autotune.cc
    fastText/src/densematrix.cc
    fastText/src/dictionary.cc
    fastText/src/fasttext.cc
    fastText/src/loss.cc
    fastText/src/matrix.cc
    fastText/src/meter.cc
    fastText/src/model.cc
    fastText/src/productquantizer.cc
    fastText/src/quantmatrix.cc
    fastText/src/utils.cc
    fastText/src/vector.cc)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_library(fasttext-static STATIC ${SOURCE_FILES} ${HEADER_FILES})
  add_library(fasttext-static_pic STATIC ${SOURCE_FILES} ${HEADER_FILES})
  set_target_properties(fasttext-static PROPERTIES OUTPUT_NAME fasttext-lima)
  set_target_properties(fasttext-static_pic PROPERTIES OUTPUT_NAME fasttext_pic-lima
  POSITION_INDEPENDENT_CODE True)
  install (TARGETS fasttext-static
      ARCHIVE DESTINATION lib)
  install (TARGETS fasttext-static_pic
      ARCHIVE DESTINATION lib)

else()
  add_library(fasttext-shared SHARED ${SOURCE_FILES} ${HEADER_FILES})
  set_target_properties(fasttext-shared PROPERTIES OUTPUT_NAME fasttext-lima)

  target_link_libraries(fasttext-shared Threads::Threads)

  install (TARGETS fasttext-shared
      LIBRARY DESTINATION lib)


  add_library(fasttext-static STATIC ${SOURCE_FILES} ${HEADER_FILES})
  set_target_properties(fasttext-static PROPERTIES OUTPUT_NAME fasttext-lima)

  install (TARGETS fasttext-static
      ARCHIVE DESTINATION lib)

endif()

#PUBLIC_HEADER DESTINATION include/fasttext-lima)

### END FastText stuff
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
install(DIRECTORY libtorch/lib/
  DESTINATION lib
  FILES_MATCHING PATTERN "*.dll"
)
file(COPY libtorch/lib/ DESTINATION ${CMAKE_BINARY_DIR}/execEnv/lib/ FILES_MATCHING PATTERN "*.dll")
else()
install(DIRECTORY libtorch/lib/
  DESTINATION lib
  FILES_MATCHING PATTERN "lib*.so*"
)
endif()
