# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next test ###############

SET(LimaAnalyzerTest0_SRCS LimaAnalyzerTest0.cpp)

set (LimaAnalyzerTest0_MOC_HEADERS LimaAnalyzerTest0.h)

add_executable(LimaAnalyzerTest0 ${LimaAnalyzerTest0_SRCS} ${LimaAnalyzerTest0_MOC_OUTFILES})
target_link_libraries(LimaAnalyzerTest0 lima-linguisticprocessing Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME LimaAnalyzerTest0 COMMAND LimaAnalyzerTest0)

