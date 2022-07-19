# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (PELF_RESOURCES ${CMAKE_INSTALL_PREFIX}/share/apps/pelf/resources)

configure_file(evaluator-easy.sh evaluator-easy.sh)
configure_file(evaluator-easy-remote.sh evaluator-easy-remote.sh)

install(PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/evaluator-easy.sh
    ${CMAKE_CURRENT_BINARY_DIR}/evaluator-easy-remote.sh
  DESTINATION share/apps/pelf/commands/evaluator
  COMPONENT runtime)
