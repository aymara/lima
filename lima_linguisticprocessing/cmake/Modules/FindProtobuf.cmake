include(FindPackageHandleStandardArgs)
include(Protobuf_VERSION)
unset(PROTOBUF_FOUND)

find_path(Protobuf_INCLUDE_DIR
        NAMES
        protobuf
        HINTS
        ${Protobuf_INSTALL_DIR}/)

find_package_handle_standard_args(Protobuf DEFAULT_MSG Protobuf_INCLUDE_DIR)

# set external variables for usage in CMakeLists.txt
if(PROTOBUF_FOUND)
    set(Protobuf_INCLUDE_DIRS ${Protobuf_INCLUDE_DIR})
    message(STATUS "Found Protobuf. ${Protobuf_INCLUDE_DIRS}")
endif()

# hide locals from GUI
mark_as_advanced(Protobuf_INCLUDE_DIR)
