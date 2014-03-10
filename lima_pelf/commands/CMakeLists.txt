file(GLOB global_command_files *.sh)
install(PROGRAMS ${global_command_files} DESTINATION bin/commands/)
