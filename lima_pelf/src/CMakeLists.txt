add_subdirectory(PelfBenchmarkingTool)
add_subdirectory(PelfResourceTool)

file(GLOB global_command_files commands/*.sh)
install(PROGRAMS ${global_command_files} DESTINATION bin/commands/)
