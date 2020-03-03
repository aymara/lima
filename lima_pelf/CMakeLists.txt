# Define project

# Find required packages
message("Searching for Qwt")
find_package(Qwt)
if(Qwt_FOUND)
    include_directories(QWT_INCLUDE_DIR)
else()
    message(FATAL "Could not find Qwt")
endif()

set(LIMA_PELF_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
set(LIMA_PELF_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})

# Go to subdirectories
add_subdirectory(easyprocessor)
if (DEFINED ENV{PELF_EVALUATOR} AND DEFINED ENV{PELF_ALIGNER} AND Qwt_FOUND)
  set(PELF_EVALUATOR $ENV{PELF_EVALUATOR})
  set(PELF_ALIGNER $ENV{PELF_ALIGNER})
  add_subdirectory(benchmarkingTool)
else ()
  message("Environment variable PELF_ALIGNER and PELF_EVALUATOR should be set to build the benchmarking tool")
endif()

if (Qwt_FOUND)
add_subdirectory(resourceTool)
endif()
add_subdirectory(evalPosTagging)
add_subdirectory(commands)

########### install files ###############
# install(FILES LimaPelfConfig.cmake DESTINATION share/apps/lima/cmake/Modules COMPONENT devel)

