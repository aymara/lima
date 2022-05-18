# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_subdirectory(examples)
add_subdirectory(commands)

# Define files
set(pelf-resedit_SRCS
    main.cpp
    ResourceTool.cpp
    ResourceReaderSimpleModel.cpp
    ResourceReaderSimpleWidget.cpp
    ResourceReaderTableModel.cpp
    ResourceReaderTableWidget.cpp
    ResourceEditorTableModel.cpp
    ResourceEditorTableWidget.cpp
    ResourceReaderToolBoxModel.cpp
    ResourceReaderToolBoxWidget.cpp
    ResourceFileEditDlg.cpp
    DictionnaryEntry.cpp
    DictionnaryEntryEditDlg.cpp
    NGramEntry.cpp
    SyntacticRuleEntry.cpp
    IdiomaticExpressionEntry.cpp
    IdiomaticExpressionEntryEditDlg.cpp
)
set(pelf-resedit_UIS
    pelf-rt.ui
    pelf-rt-editresourcetextfiledialog.ui
    pelf-rt-editdictionnaryentrydialog.ui
    pelf-rt-editidiomaticexpressionentrydialog.ui
)

# Wrap Qt UIs
qt5_wrap_ui(pelf-resedit_UIS_H ${pelf-resedit_UIS})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Compile, link and install main exec
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/PelfResourceTool)
add_executable(pelf-resedit ${pelf-resedit_SRCS} ${pelf-resedit_UIS_H})
target_link_libraries(pelf-resedit 
    lima-common
    lima-linguisticprocessing
    ${optionalLibs}
    ${Boost_LIBRARIES}
    ${Qt5_LIBRARIES}
)
install(TARGETS pelf-resedit DESTINATION bin)
