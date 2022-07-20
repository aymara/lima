# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

file(GLOB files *.xml)

install(FILES ${files} DESTINATION share/apps/pelf/examples/easy-ref/text
  COMPONENT runtime)
