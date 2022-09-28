# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# Locates the tensorFlow library and include directories.

include(FindPackageHandleStandardArgs)
unset(TENSORFLOW_FOUND)

find_path(TensorFlow_INCLUDE_DIR
        NAMES
        tensorflow/core
        tensorflow/cc
        third_party
        HINTS
        ${TF_SOURCES_PATH})
find_library(TensorFlow_LIBRARY
        NAMES
        tensorflow-for-lima
        HINTS
        /usr/lib
        /usr/local/lib)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
get_filename_component(TensorFlow_LIBRARY_Folder ${TensorFlow_LIBRARY} DIRECTORY)
set(TensorFlow_LIBRARY_DYN ${TensorFlow_LIBRARY_Folder}/../bin/tensorflow-for-lima.dll)
endif()

# set TensorFlow_FOUND
find_package_handle_standard_args(TensorFlow DEFAULT_MSG TensorFlow_INCLUDE_DIR TensorFlow_LIBRARY)

# set external variables for usage in CMakeLists.txt
if(TENSORFLOW_FOUND)
    set(TensorFlow_INCLUDE_DIRS ${TensorFlow_INCLUDE_DIR})
    set(TensorFlow_LIBRARIES ${TensorFlow_LIBRARY})
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(TensorFlow_LIBRARIES_DYN ${TensorFlow_LIBRARY_DYN})
    message("tensorflow dll: ${TensorFlow_LIBRARIES_DYN}")
    endif()
endif()

# hide locals from GUI
mark_as_advanced(TensorFlow_INCLUDE_DIR TensorFlow_LIBRARY)
