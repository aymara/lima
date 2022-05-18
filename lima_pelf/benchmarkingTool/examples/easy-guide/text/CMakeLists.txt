# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

file(GLOB files Guide.*)

install(FILES ${files} DESTINATION share/apps/pelf/examples/easy-guide/text)
