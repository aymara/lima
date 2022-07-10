# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### install files ###############
file (GLOB MODELES *.mod)

install(FILES
  ${MODELES}
COMPONENT eng DESTINATION share/apps/lima/resources/SpecificEntities)