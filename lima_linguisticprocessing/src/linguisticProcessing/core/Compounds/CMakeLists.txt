# Copyright 2002-2022 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


########### next target ###############

SET(lima-lp-compounds_LIB_SRCS
   CompoundsBuilderFromSyntacticData.cpp
   CompoundTokenAnnotation.cpp
   DepGraphCompoundsBuildVisitor.cpp
   CompoundsCompatibilityBuilder.cpp
   AnnotDotGraphWriter.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-compounds)

target_compile_definitions(lima-lp-compounds PRIVATE "LIMA_COMPOUNDS_EXPORTING")

target_link_libraries(lima-lp-compounds
  lima-common
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-annotationgraph
  lima-lp-linguisticanalysisstructure
  lima-lp-syntacticanalysis
  lima-lp-bagofwords
  lima-lp-automaton
  ${Qt5_LIBRARIES}
  lima-lp-propertycode
   lima-common-tools
   lima-common-mediaprocessors
   lima-common-mediaticdata
   lima-common-time
   lima-lp-linguisticprocessing-core
   lima-lp-textsegmentation
   lima-lp-linguisticdata
   lima-common-xmlconfigurationfiles
   lima-common-data
   lima-common-misc
   lima-common-processunitframework
   lima-common-factory
   lima-lp-morphologicanalysis
)
set_target_properties(lima-lp-compounds PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-compounds DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
# install(FILES
#     AnnotDotGraphWriter.h
#     CompoundsCompatibilityBuilder.h
#     CompoundsBuilderFromSyntacticData.h
#     CompoundTokenAnnotation.h
#     DepGraphCompoundsBuildVisitor.h
#   DESTINATION include/linguisticProcessing/core/Compounds
#   COMPONENT devel
# )


