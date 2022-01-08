# Finds the required directories to include Eigen. Since Eigen is
# only header files, there is no library to locate, and therefore
# no *_LIBRARIES variable is set.

include(FindPackageHandleStandardArgs)
include(Eigen_VERSION)
unset(EIGEN_FOUND)

message(STATUS "Eigen_DIR = ${Eigen_DIR} / Eigen_INSTALL_DIR = ${Eigen_INSTALL_DIR}")

find_path(Eigen_INCLUDE_DIR
        NAMES
        ${Eigen_DIR}
        ${Eigen_DIR}/unsupported
        ${Eigen_DIR}/Eigen
        Eigen
        HINTS
        ${Eigen_INSTALL_DIR}
        /usr/local
        /usr/local/include
        )

# set Eigen_FOUND
find_package_handle_standard_args(Eigen DEFAULT_MSG Eigen_INCLUDE_DIR)

# set external variables for usage in CMakeLists.txt
if(EIGEN_FOUND)
    set(Eigen_INCLUDE_DIRS ${Eigen_INCLUDE_DIR} ${Eigen_INCLUDE_DIR}/${Eigen_DIR})
endif()

# hide locals from GUI
mark_as_advanced(Eigen_INCLUDE_DIR)
