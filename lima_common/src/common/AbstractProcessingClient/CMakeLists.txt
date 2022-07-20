# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


########### install files ###############
install(FILES
    AbstractProcessingClient.h
  DESTINATION include/common/AbstractProcessingClient/
  COMPONENT devel
)
