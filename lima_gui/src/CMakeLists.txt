#   Copyright 2017 CEA LIST
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

add_subdirectory(treeview)

########### next target ###############

SET (lima_MOC_HEADERS
  LimaGuiApplication.h
  Threads.h
  
  config/LimaConfiguration.h
  config/ConfigurationTreeModel.h
  
  ConllListModel.h
  NamedEntitiesParser.h
)

qt5_wrap_cpp (lima_MOC_OUTFILES ${lima_MOC_HEADERS})

SET (lima_SRCS 
  main.cpp
  LimaGuiApplication.cpp
  Threads.cpp
  ConllParser.cpp
  
  config/LimaConfiguration.cpp
  config/ConfigurationTree.cpp
  config/ConfigurationTreeModel.cpp
  
  ConllListModel.cpp
  NamedEntitiesParser.cpp
  
  tools/extract/FileTextExtractor.cpp
)

if (Qt5Core_FOUND)
  if (Qt5Core_VERSION VERSION_GREATER 5.7.0)
    qt5_add_resources(lima_SRCS resources.qrc)
  else()
    qt5_add_resources(lima_SRCS resources-old.qrc)
  endif()
endif()

set(QTLIBS Qt5::Core Qt5::Test Qt5::Qml Qt5::Quick Qt5::Gui)

if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
  if(Qt5Core_VERSION VERSION_LESS 5.3)
    message(WARNING "Cannot use windeployqt in Qt 5.2 and earlier")
  else()
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
    include(WinDeployQt)
    WinDeployQt(
      COMPILER_RUNTIME 
      TARGET lima 
      QMLDIR qml-old
      INCLUDE_MODULES ${QTLIBS} 
    )
    install(
      DIRECTORY ${PROJECT_BINARY_DIR}/windeployqt/
      DESTINATION ${CMAKE_INSTALL_BINDIR})
  endif()
endif()

if (WIN32 AND CMAKE_BUILD_TYPE MATCHES Release)
  add_executable(lima WIN32
    ${lima_SRCS} 
    ${lima_MOC_OUTFILES}
  )
  
target_link_libraries(lima 
  lima-common-mediaticdata
  lima-common-xmlconfigurationfiles
  lima-common-tools
  lima-common-data 
  lima-common-factory
  lima-common-misc
  lima-common
  lima-lp-annotationgraph
  lima-lp-lineartextrepresentation
  lima-lp-misc
  lima-lp-tgv
  lima-lp-linguisticdata
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-regexmatcher
  lima-lp-wordsensetagger
  lima-lp-linguisticprocessing-core
  lima-lp-analysisdict
  lima-lp-eventanalyzer
  lima-lp-corefsolver
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-lp-morphologicanalysis
  lima-lp-dictionary
  lima-lp-easyxmldumper
  lima-lp-analysisdumpers
  lima-lp-textsegmentation
  lima-lp-linguisticanalysisstructure
  lima-lp-semanticanalysis
  lima-lp-linguisticresources
  lima-lp-entitytracker
  lima-lp-flattokenizer
  lima-lp-applyrecognizer
  lima-lp-linguisticprocessors
  lima-lp-postagger
  lima-lp-automaton
  lima-lp-analysishandlers
  lima-lp-client
  ${Qt5_LIBRARIES}
  Qt5::WinMain
)
else()
  add_executable(lima
    ${lima_SRCS} 
    ${lima_MOC_OUTFILES}
  )
target_link_libraries(lima 
  lima-common-mediaticdata
  lima-common-xmlconfigurationfiles
  lima-common-tools
  lima-common-data 
  lima-common-factory
  lima-common-misc
  lima-common
  lima-lp-annotationgraph
  lima-lp-lineartextrepresentation
  lima-lp-misc
  lima-lp-tgv
  lima-lp-linguisticdata
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-regexmatcher
  lima-lp-wordsensetagger
  lima-lp-linguisticprocessing-core
  lima-lp-analysisdict
  lima-lp-eventanalyzer
  lima-lp-corefsolver
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-lp-morphologicanalysis
  lima-lp-dictionary
  lima-lp-easyxmldumper
  lima-lp-analysisdumpers
  lima-lp-textsegmentation
  lima-lp-linguisticanalysisstructure
  lima-lp-semanticanalysis
  lima-lp-linguisticresources
  lima-lp-entitytracker
  lima-lp-flattokenizer
  lima-lp-applyrecognizer
  lima-lp-linguisticprocessors
  lima-lp-postagger
  lima-lp-automaton
  lima-lp-analysishandlers
  lima-lp-client
  ${Qt5_LIBRARIES}
)
endif()

#   lima-common
#   lima-linguisticprocessing

install(TARGETS lima DESTINATION bin
  COMPONENT runtime)

########### install files ###############
install(PROGRAMS lima.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(PROGRAMS qml/resources/lima.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor)
