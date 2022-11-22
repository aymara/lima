# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SHORTEN_POR_CORPUS_FOR_SVMLEARN "Use a shortened corpus for SVMTlearn to reduce learning time" OFF)
message("SHORTEN_POR_CORPUS_FOR_SVMLEARN=${SHORTEN_POR_CORPUS_FOR_SVMLEARN}")
option(WITH_ASAN "Enable address sanitizer" OFF)
message("WITH_ASAN=${WITH_ASAN}")
option(WITH_ARCH "Enable architecture optimizations" OFF)
message("WITH_ARCH=${WITH_ARCH}")
option(WITH_DEBUG_MESSAGES "Enable debug messages" OFF)
message("WITH_DEBUG_MESSAGES=${WITH_DEBUG_MESSAGES}")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message("Linux flags")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-omit-frame-pointer")

    if (WITH_ARCH)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic -msse4.2")
    endif()

    # Flags needed for the LIMA plugins mechanism to work: our libs are dynamically loaded by
    # factories, thus their symbols must be known even if not used by the binary
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    # set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-z,defs,--no-as-needed")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,-z,defs,--no-as-needed")

    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-std=c++0x HAVE_STDCPP0X)
    CHECK_CXX_COMPILER_FLAG(-std=c++11 HAVE_STDCPP11)
    CHECK_CXX_COMPILER_FLAG(-std=c++14 HAVE_STDCPP14)
    CHECK_CXX_COMPILER_FLAG(-std=c++17 HAVE_STDCPP17)
    CHECK_CXX_COMPILER_FLAG(-fsanitize=undefined HAVE_SANITIZE_UNDEFINED)
    CHECK_CXX_COMPILER_FLAG(-fno-omit-frame-pointer HAVE_NO_OMIT_FRAME_POINTER)
    if (HAVE_NO_OMIT_FRAME_POINTER)
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer")
    endif()

    if (HAVE_STDCPP17)
      message("C++17 supported ")
      set(CMAKE_CXX_STANDARD 17)
      set(CMAKE_CXX_STANDARD_REQUIRED ON)
      set(CMAKE_CXX_EXTENSIONS OFF)
    else ()
      message("Minimum C++ version NOT supported")
      set(CMAKE_CXX_FLAGS "-DNO_STDCPP0X ${CMAKE_CXX_FLAGS}")
    endif ()
    set(CMAKE_CXX_FLAGS "-W -Wall ${CMAKE_CXX_FLAGS}")

    if (WITH_ASAN)
      # NOTE: check -fsanitize=address flag failed if this flag is not set also
      # for the linker (use CMAKE_REQUIRED_FLAGS to set linker flag for
      # CHECK_CXX_COMPILER_FLAG macro).
      set(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      CHECK_CXX_COMPILER_FLAG(-fsanitize=address HAVE_SANITIZE_ADDRESS)
      set(CMAKE_REQUIRED_FLAGS "${SAFE_CMAKE_REQUIRED_FLAGS}")
      if (HAVE_SANITIZE_ADDRESS)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -O1")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address")
        set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=address")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=address")
      else()
        message(FATAL "ASAN required but -fsanitize=address is not supported")
      endif()
      CHECK_CXX_COMPILER_FLAG(-fsanitize=undefined HAVE_SANITIZE_UNDEFINED)
      if (HAVE_SANITIZE_UNDEFINED)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=undefined")
        set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=undefined")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=undefined")
      else()
        message(FATAL "ASAN required but -fsanitize=undefined is not supported")
      endif()
      CHECK_CXX_COMPILER_FLAG(-fsanitize=leak HAVE_SANITIZE_LEAK)
      if (HAVE_SANITIZE_LEAK)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=leak")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=leak")
        set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=leak")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=leak")
      else()
        message(FATAL "ASAN required but -fsanitize=leak is not supported")
      endif()
      CHECK_CXX_COMPILER_FLAG(-fsanitize=thread HAVE_SANITIZE_THREAD)
      if (HAVE_SANITIZE_THREAD)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=thread")
        set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=thread")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=thread")
      else()
        message(FATAL "ASAN required but -fsanitize=thread is not supported")
      endif()
      CHECK_CXX_COMPILER_FLAG(-fsanitize=undefined HAVE_SANITIZE_UNDEFINED)
      if (HAVE_SANITIZE_UNDEFINED)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=undefined")
        set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=undefined")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=undefined")
      else()
        message(FATAL "ASAN required but -fsanitize=undefined is not supported")
      endif()

    endif ()

  set(DEBUG_FLAGS "-DDEBUG_LP -DDEBUG_CD -DDEBUG_FACTORIES -DDEBUG_LIMA_GUI -DQT_QML_DEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}" )
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${DEBUG_FLAGS}" )
  if (WITH_DEBUG_MESSAGES)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DEBUG_FLAGS}" )
  endif()

  set(LIB_INSTALL_DIR "lib")

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

    message("MacOS flags")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-omit-frame-pointer")

    if (WITH_ARCH)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic -msse4.2")
    endif()

    # Flags needed for the LIMA plugins mechanism to work: our libs are dynamically loaded by
    # factories, thus their symbols must be known even if not used by the binary
#    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")
#    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
#    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-as-needed")

    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-std=c++0x HAVE_STDCPP0X)
    CHECK_CXX_COMPILER_FLAG(-std=c++11 HAVE_STDCPP11)
    CHECK_CXX_COMPILER_FLAG(-std=c++14 HAVE_STDCPP14)
    CHECK_CXX_COMPILER_FLAG(-std=c++17 HAVE_STDCPP17)
    CHECK_CXX_COMPILER_FLAG(-fsanitize=undefined HAVE_SANITIZE_UNDEFINED)
    CHECK_CXX_COMPILER_FLAG(-fno-omit-frame-pointer HAVE_NO_OMIT_FRAME_POINTER)
    if (HAVE_NO_OMIT_FRAME_POINTER)
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer")
    endif()
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_FLAGS "-W -Wall ${CMAKE_CXX_FLAGS}")

    if (WITH_ASAN)
      # NOTE: check -fsanitize=address flag failed if this flag is not set also
      # for the linker (use CMAKE_REQUIRED_FLAGS to set linker flag for
      # CHECK_CXX_COMPILER_FLAG macro).
      set(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      CHECK_CXX_COMPILER_FLAG(-fsanitize=address HAVE_SANITIZE_ADDRESS)
      set(CMAKE_REQUIRED_FLAGS "${SAFE_CMAKE_REQUIRED_FLAGS}")
      if (HAVE_SANITIZE_ADDRESS)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address")
        set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=address")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=address")
      else()
        message(FATAL "ASAN is required but -fsanitize=address is not available")
      endif()
      CHECK_CXX_COMPILER_FLAG(-fsanitize=undefined HAVE_SANITIZE_UNDEFINED)
      if (HAVE_SANITIZE_UNDEFINED)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=undefined")
        set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=undefined")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=undefined")
      else()
        message(FATAL "ASAN is required but -fsanitize=undefined is not available")
      endif()
      CHECK_CXX_COMPILER_FLAG(-fsanitize=leak HAVE_SANITIZE_LEAK)
      if (HAVE_SANITIZE_LEAK)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=leak")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=leak")
        set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=leak")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=leak")
      else()
        message(FATAL "ASAN is required but -fsanitize=leak is not available")
      endif()

    endif ()

  set(DEBUG_FLAGS "-DDEBUG_LP -DDEBUG_CD -DDEBUG_FACTORIES -DDEBUG_LIMA_GUI -DQT_QML_DEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}" )
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${DEBUG_FLAGS}" )
  if (WITH_DEBUG_MESSAGES)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DEBUG_FLAGS}" )
  endif()

    set(LIB_INSTALL_DIR "lib")

else ()
  message("Windows flags")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  add_definitions(-D WIN32)

  # By default, do not warn when built on machines using only VS Express:
  if(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  endif()
  set(CMAKE_C_FLAGS "/EHsc /GR ${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "/EHsc /GR /MP /utf-8 /DNOMINMAX ${CMAKE_CXX_FLAGS}")

  set(DEBUG_FLAGS "/DDEBUG_LP /DDEBUG_CD /DDEBUG_FACTORIES /DDEBUG_LIMA_GUI /DQT_QML_DEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}" )
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${DEBUG_FLAGS}" )
  if (WITH_DEBUG_MESSAGES)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DEBUG_FLAGS}" )
  endif()
  set(LIB_INSTALL_DIR "bin")

  install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    DESTINATION bin
    COMPONENT runtime)

endif ()


