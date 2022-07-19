# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

file(GLOB global_command_files *.sh)
install(PROGRAMS ${global_command_files} DESTINATION bin/commands/
  COMPONENT runtime)
