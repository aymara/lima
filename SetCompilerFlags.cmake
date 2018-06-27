#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SHORTEN_POR_CORPUS_FOR_SVMLEARN "Use a shortened corpus for SVMTlearn to reduce learning time" OFF)
message("SHORTEN_POR_CORPUS_FOR_SVMLEARN=${SHORTEN_POR_CORPUS_FOR_SVMLEARN}")
option(WITH_ASAN "Enable address sanitizer" OFF)
message("WITH_ASAN=${WITH_ASAN}")
option(WITH_ARCH "Enable architecture optimizations" OFF)
message("WITH_ARCH=${WITH_ARCH}")


if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
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
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-z,defs,--no-as-needed")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-z,defs,--no-as-needed")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,-z,defs,--no-as-needed")

    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-std=c++0x HAVE_STDCPP0X)
    CHECK_CXX_COMPILER_FLAG(-std=c++11 HAVE_STDCPP11)
    CHECK_CXX_COMPILER_FLAG(-fsanitize=undefined HAVE_SANITIZE_UNDEFINED)
    CHECK_CXX_COMPILER_FLAG(-fno-omit-frame-pointer HAVE_NO_OMIT_FRAME_POINTER)
    if (HAVE_NO_OMIT_FRAME_POINTER)
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer")
    endif()

    if (HAVE_STDCPP11)
      message("C++11 supported")
      set(CMAKE_CXX_FLAGS "-std=c++11 -DBOOST_NO_HASH ${CMAKE_CXX_FLAGS}")
      CHECK_CXX_COMPILER_FLAG(-Wsuggest-override HAVE_SUGGEST_OVERRIDE)
      if (HAVE_SUGGEST_OVERRIDE)
#       set(CMAKE_CXX_FLAGS "-Wsuggest-override -Werror=suggest-override ${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS "-Wsuggest-override ${CMAKE_CXX_FLAGS}")
      endif()
    elseif (HAVE_STDCPP0X)
      message("C++0x supported")
      set(CMAKE_CXX_FLAGS "-std=c++0x -DBOOST_NO_HASH ${CMAKE_CXX_FLAGS}")
    else ()
      message("C++0x NOT supported")
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
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
      endif()
      CHECK_CXX_COMPILER_FLAG(-fsanitize=undefined HAVE_SANITIZE_UNDEFINED)
      if (HAVE_SANITIZE_UNDEFINED)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
      endif()
      CHECK_CXX_COMPILER_FLAG(-fsanitize=leak HAVE_SANITIZE_LEAK)
      if (HAVE_SANITIZE_LEAK)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=leak")
      endif()

    endif ()

    set(LIB_INSTALL_DIR "lib")
else ()
  message("Windows flags")
  add_definitions(-D WIN32)
  
  # By default, do not warn when built on machines using only VS Express:
  if(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  endif()
  set(CMAKE_C_FLAGS "/EHsc /GR ${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "/EHsc /GR /MP /utf-8 ${CMAKE_CXX_FLAGS}")

  set(LIB_INSTALL_DIR "bin")

  install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    DESTINATION bin
    COMPONENT Libraries)

endif ()


