# Copyright 2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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

if (Qt5Core_FOUND)
  if (Qt5Core_VERSION VERSION_GREATER 5.7.0)
    qt5_add_resources(lima_SRCS resources.qrc)
    set(QMLDIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
  else()
    qt5_add_resources(lima_SRCS resources-old.qrc)
    set(QMLDIR ${CMAKE_CURRENT_SOURCE_DIR}/qml-old)
  endif()
endif()

#set(QTLIBS Qt5::Core Qt5::Test Qt5::Qml Qt5::Quick Qt5::Gui Qt5::Widgets)

# if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
#   if(Qt5Core_VERSION VERSION_LESS 5.3)
#     message(WARNING "Cannot use windeployqt in Qt 5.2 and earlier")
#   else()
#     set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
#     include(WinDeployQt)
#     WinDeployQt(
#       COMPILER_RUNTIME
#       TARGET lima
#       QMLDIR ${QMLDIR}
#       INCLUDE_MODULES ${QTLIBS}
#     )
#     install(
#       DIRECTORY ${PROJECT_BINARY_DIR}/windeployqt/
#       DESTINATION ${CMAKE_INSTALL_BINDIR})
#   endif()
# endif()

if (WIN32)
  add_executable(lima WIN32
    ${lima_SRCS}
  )

  target_link_libraries(lima
    lima-common-mediaticdata
    lima-common-xmlconfigurationfiles
    lima-lp-client
    lima-lp-analysishandlers
    Qt5::Gui
    Qt5::Quick
    Qt5::Widgets
    Qt5::WinMain
  )
  include(DeployQt.cmake)
  mywindeployqt(
    TARGET lima
    DIRECTORY bin
    QMLDIR ${QMLDIR}
  )
  get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
  get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
  install(
    FILES
      ${_qt_bin_dir}/Qt5QuickTemplates2.dll
      ${_qt_bin_dir}/Qt5QuickControls2.dll
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
    Qt5::Gui
    Qt5::Quick
    Qt5::Widgets
  )
endif()

install(TARGETS lima DESTINATION bin
  COMPONENT runtime)

########### install files ###############
install(PROGRAMS lima.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(PROGRAMS qml/resources/lima.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor)
