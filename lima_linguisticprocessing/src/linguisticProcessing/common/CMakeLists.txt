# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/linguisticProcessing )

add_subdirectory(BagOfWords)
add_subdirectory(PropertyCode)
add_subdirectory(annotationGraph)
add_subdirectory(linguisticData)
add_subdirectory(misc)
add_subdirectory(tgv)
add_subdirectory(linearTextRepresentation)
add_subdirectory(helpers)

########### install files ###############
