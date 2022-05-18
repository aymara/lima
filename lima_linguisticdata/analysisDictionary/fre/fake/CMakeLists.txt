# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

COMPILEXMLDIC(fre ${CMAKE_CURRENT_SOURCE_DIR}/corporate.xml "fake")
COMPILEXMLDIC(fre ${CMAKE_CURRENT_SOURCE_DIR}/user.xml "fake")

# file(GLOB FILES_TO_INSTALL *.dat)
# install(FILES ${FILES_TO_INSTALL} COMPONENT fre DESTINATION share/apps/lima/resources/LinguisticProcessings/fre/fake)
