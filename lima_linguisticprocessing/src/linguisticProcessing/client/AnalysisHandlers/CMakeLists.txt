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

add_definitions(-DLIMA_ANALYSISHANDLERS_EXPORTING)

########### next target ###############

SET(lima-lp-analysishandlers_LIB_SRCS
   AbstractTextualAnalysisHandler.cpp
   BowDocumentHandler.cpp
   BowDocumentWriter.cpp
   BowTextHandler.cpp
   LTRTextHandler.cpp
   BowTextWriter.cpp
   CompactFullDocumentHandler.cpp
   FakeHandler.cpp
   SBowDocumentWriter.cpp
   SimpleStreamHandler.cpp
   StructuredBoWToBoWDocument.cpp
)

add_library(lima-lp-analysishandlers SHARED 
  ${lima-lp-analysishandlers_LIB_SRCS} 
)
ADD_DLL_TO_PATH(lima-lp-analysishandlers)


target_link_libraries(lima-lp-analysishandlers
  lima-lp-linguisticresources
  lima-lp-lineartextrepresentation
  lima-lp-bagofwords
  lima-common
  ${Qt5_LIBRARIES}
)

set_target_properties(lima-lp-analysishandlers PROPERTIES 
                      VERSION ${LIMA_LP_LIB_VERSION} 
                      SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-analysishandlers 
        DESTINATION ${LIB_INSTALL_DIR}
        COMPONENT runtime)


########### install files ###############
install(FILES
    AnalysisHandlersExport.h
    AbstractTextualAnalysisHandler.h
    BowDocumentHandler.h
    BowTextWriter.h
    SBowDocumentWriter.h
    BowDocumentWriter.h
    CompactFullDocumentHandler.h
    SimpleStreamHandler.h
    BowTextHandler.h
    LTRTextHandler.h
    FakeHandler.h
#     StructuredBoWToBoWDocument.h
  DESTINATION include/linguisticProcessing/client/AnalysisHandlers
  COMPONENT devel
)
