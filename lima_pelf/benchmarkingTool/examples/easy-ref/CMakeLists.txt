# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_subdirectory(ref)
add_subdirectory(text)

install(FILES easy-ref-small.lbp easy-ref.lbp DESTINATION share/apps/pelf/examples/easy-ref
  COMPONENT runtime)
