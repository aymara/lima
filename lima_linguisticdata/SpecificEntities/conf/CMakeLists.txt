# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

file (GLOB MODEXES *.xml)

install(FILES ${MODEXES} COMPONENT common DESTINATION share/config/lima)
