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
#     ${Protobuf_LIBRARIES}
    ${optionalLibs}
    ${Boost_LIBRARIES}
    ${Qt5_LIBRARIES}
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

function(cat IN_FILE OUT_FILE)
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()
function(append IN_FILE OUT_FILE)
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

cat(
  ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbaa 
  ${CMAKE_CURRENT_BINARY_DIR}/output_graph.pb)
set(FR_OUTPUTGRAPH_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbab
  ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbac
  ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbad
  ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbae
  ${CMAKE_CURRENT_SOURCE_DIR}/results/fr/output_graph.pbaf
)

foreach(FR_OUTPUTGRAPH_FILE ${FR_OUTPUTGRAPH_FILES})
  append(${FR_OUTPUTGRAPH_FILE} ${CMAKE_CURRENT_BINARY_DIR}/output_graph.pb)
endforeach()

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
  

# Comment out TF-based NER tests while a solution to store models has not been
# found
# add_subdirectory(tests)

########### install files ###############

install(FILES 
    TensorflowSpecificEntities.h
    TensorflowSpecificEntitiesExport.h
  DESTINATION include/linguisticProcessing/core/TensorflowSpecificEntities
  COMPONENT devel
)

install(FILES
  logTensorflowSpecificEntitiescpp.properties
  DESTINATION share/config/lima
  COMPONENT runtime
)
