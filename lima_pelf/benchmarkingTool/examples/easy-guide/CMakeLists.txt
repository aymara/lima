# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_subdirectory(ref)
add_subdirectory(text)

set (LBP_PATH ${CMAKE_INSTALL_PREFIX}/share/apps/pelf/examples/easy-guide)
set (PELF_BT_COMMANDS_ROOT ${CMAKE_INSTALL_PREFIX}/share/apps/pelf/commands)

configure_file(easy-guide-small.lbp easy-guide-small.lbp)
configure_file(easy-guide.lbp easy-guide.lbp)

install(FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/easy-guide-small.lbp 
    ${CMAKE_CURRENT_BINARY_DIR}/easy-guide.lbp 
  DESTINATION share/apps/pelf/examples/easy-guide)
