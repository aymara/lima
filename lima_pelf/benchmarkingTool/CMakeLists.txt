# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_subdirectory(examples)
add_subdirectory(commands)

# Define files
set(pelf-bench_SRCS  
    main.cpp
    BenchmarkingTool.cpp
    Pipeline.cpp
    PipelineUnit.cpp
    PipelineUnitDelegate.cpp
    Command.cpp
    BenchmarkingResult.cpp
    EvaluationResult.cpp
    EvaluationResultSet.cpp
    EvaluationResultDimension.cpp
    PipelineWidget.cpp
    PipelineEditFileDlg.cpp
    PipelineConfigureDlg.cpp
    ResultsModel.cpp
    ResultsWidget.cpp
    CommentEditDlg.cpp
    syanotprocess.cpp
    BenchmarkingXmlReader.cpp
    BenchmarkingXmlWriter.cpp
)
set(pelf-bench_UIS
    pelf-bt.ui
    pelf-bt-editfiledialog.ui
    pelf-bt-configurepipelinedialog.ui
    pelf-bt-commenteditdialog.ui
)
SET(pelf-bt_RCCS pelf-bt-resources.qrc)


# Wrap Qt UIs
qt5_wrap_ui(pelf-bench_UIS_H ${pelf-bench_UIS})
#qt5_automoc(${pelf-bench_SRCS})
qt5_add_resources(pelf-bt_RCC_SRCS ${pelf-bt_RCCS})
# Compile, link and install main exec
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/PelfBenchmarkingTool)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(pelf-bench ${pelf-bench_SRCS} ${pelf-bench_UIS_H} ${pelf-bt_RCC_SRCS})
target_link_libraries(pelf-bench ${Qt5_LIBRARIES} ${QWT_LIBRARY})
install(TARGETS pelf-bench DESTINATION bin)

