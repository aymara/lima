# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_TENSORFLOWSPECIFICENTITIES_EXPORTING)

########### next target ###############

SET(lima-lp-tensorflowspecificentities_LIB_SRCS
  nerUtils.cpp
  TensorflowSpecificEntities.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-tensorflowspecificentities)

target_link_libraries(lima-lp-tensorflowspecificentities
    lima-common-factory
    lima-common-misc
    lima-common-data
    lima-common-fsaaccess
    lima-common-mediaticdata
    lima-common-time
    lima-common-xmlconfigurationfiles
    lima-common-processunitframework
    lima-common-mediaprocessors
    lima-lp-linguisticprocessors
    lima-lp-linguisticresources
    lima-lp-annotationgraph
    lima-lp-analysisdict
    lima-lp-linguisticanalysisstructure
    lima-lp-applyrecognizer
    lima-lp-automaton
    lima-lp-specificentities
    ${TensorFlow_LIBRARIES}
    ${optionalLibs}
)

set_target_properties(lima-lp-tensorflowspecificentities
  PROPERTIES
  VERSION ${LIMA_LP_LIB_VERSION}
  SOVERSION ${LIMA_LP_LIB_SOVERSION}
  )

install(TARGETS lima-lp-tensorflowspecificentities
  DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### next target ###############
add_executable(limaTfNer main.cpp)


# Use the Widgets module from Qt 5.
target_link_libraries(limaTfNer
  Qt5::Core
  ${PROJECT_LIBRARIES}
  ${Boost_LIBRARIES}
  lima-lp-tensorflowspecificentities
)

install(FILES
    data/IOB1/eng/words.txt
    data/lima/eng/IOB1/tags.txt #TAGS change from IOB1 orginal format for LIMA use
    data/IOB1/eng/chars.txt
    results/eng_IOB1/output_graph.pb
  COMPONENT eng
  DESTINATION "share/apps/lima/resources/TensorFlowSpecificEntities/eng/IOB1/")

# uncomment if you want to use IOB2 format
install(FILES
     data/IOB2/eng/words.txt
     data/lima/eng/IOB2/tags.txt #TAGS change from IOB2 orginal format for LIMA use
     data/IOB2/eng/chars.txt
     results/eng_IOB2/output_graph.pb
  COMPONENT eng
  DESTINATION "share/apps/lima/resources/TensorFlowSpecificEntities/eng/IOB2/")


  set(FR_OUTPUTGRAPH_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbab
    ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbac
    ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbad
    ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbae
    ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbaf
  )

if(WIN32)
  execute_process(
      COMMAND
      ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbaa ${CMAKE_CURRENT_BINARY_DIR}/output_graph.pb
  )

  string(REPLACE "/" "\\" TARGET_FN ${CMAKE_CURRENT_BINARY_DIR}/output_graph.pb)

  foreach(FR_OUTPUTGRAPH_FILE ${FR_OUTPUTGRAPH_FILES})
    message(STATUS "Appending ${FR_OUTPUTGRAPH_FILE} to ${CMAKE_CURRENT_BINARY_DIR}/output_graph.pb ...")
    string(REPLACE "/" "\\" FN ${FR_OUTPUTGRAPH_FILE})
    execute_process(
      COMMAND
      cmd "/C" "copy /b ${TARGET_FN}+${FN} ${TARGET_FN}"
    )
    endforeach()

  execute_process(
    COMMAND
    cmd
    "copy /b ${FR_OUTPUTGRAPH_FILES_CONCAT} ${CMAKE_CURRENT_BINARY_DIR}/output_graph.pb"
  )

else() # expect sh-like environment

  execute_process(
        COMMAND
        /bin/bash
        "-c" "/bin/cp ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbaa ${CMAKE_CURRENT_BINARY_DIR}/output_graph.pb"
        )

  foreach(FR_OUTPUTGRAPH_FILE ${FR_OUTPUTGRAPH_FILES})
    message(STATUS "Appending ${FR_OUTPUTGRAPH_FILE} to ${CMAKE_CURRENT_BINARY_DIR}/output_graph.pb ...")
    execute_process(
        COMMAND
        /bin/bash
        "-c" "/bin/cat ${FR_OUTPUTGRAPH_FILE} >> ${CMAKE_CURRENT_BINARY_DIR}/output_graph.pb"
    )
  endforeach()

endif()

install(FILES
     data/IOB1/fr/words.txt
     data/lima/fr/IOB1/tags.txt
     data/IOB1/fr/chars.txt
     ${CMAKE_CURRENT_BINARY_DIR}/output_graph.pb
  COMPONENT fr
  DESTINATION "share/apps/lima/resources/TensorFlowSpecificEntities/fr/")

install(
  TARGETS limaTfNer
  DESTINATION bin
  COMPONENT runtime)


add_subdirectory(tests)

########### install files ###############

install(FILES
    TensorflowSpecificEntities.h
    TensorflowSpecificEntitiesExport.h
  DESTINATION include/linguisticProcessing/core/TensorflowSpecificEntities
  COMPONENT devel
)

install(FILES
  logTensorflowSpecificEntitiescpp.properties
  DESTINATION share/config/lima/log4cpp
  COMPONENT runtime
)
