#   Copyright 2019 CEA LIST
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

if (QtCore_FOUND)
  if (QtCore_VERSION VERSION_GREATER 5.7.0)
    if(QtCore_VERSION VERSION_GREATER 5.14.0)
      qt_add_resources(lima_SRCS resources.qrc)
    else()
      qt5_add_resources(lima_SRCS resources.qrc)
    endif()
    set(QMLDIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
  else()
    qt5_add_resources(lima_SRCS resources-old.qrc)
    set(QMLDIR ${CMAKE_CURRENT_SOURCE_DIR}/qml-old)
  endif()
endif()

if (WIN32)
  add_executable(lima WIN32
    ${lima_SRCS}
  )

  target_link_libraries(lima
    lima-common-mediaticdata
    lima-common-xmlconfigurationfiles
    lima-lp-client
    lima-lp-analysishandlers
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WinMain
  )
  include(DeployQt.cmake)
  mywindeployqt(
    TARGET lima
    DIRECTORY bin
    QMLDIR ${QMLDIR}
  )
  get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
  get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
  install(
    FILES
      ${_qt_bin_dir}/QtQuickTemplates2.dll
      ${_qt_bin_dir}/QtQuickControls2.dll
    DESTINATION
      bin
  )

else()
  add_executable(lima
    ${lima_SRCS}
  )

  target_link_libraries(lima
    lima-common-mediaticdata
    lima-common-xmlconfigurationfiles
    lima-lp-client
    lima-lp-analysishandlers
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Widgets
  )
endif()

install(TARGETS lima DESTINATION bin
  COMPONENT runtime)

########### install files ###############
install(PROGRAMS lima.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(PROGRAMS qml/resources/lima.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor)
