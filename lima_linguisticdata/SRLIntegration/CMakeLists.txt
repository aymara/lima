# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

if(NOT WIN32)
  set(LIMA_PATH_SEPARATOR ":")
else()
  set(LIMA_PATH_SEPARATOR "\;") # WANING: must be protected against cmake semicolon substitution
endif()

set(LIMA_CONF "${CMAKE_BINARY_DIR}/execEnv/config${LIMA_PATH_SEPARATOR}${CMAKE_BINARY_DIR}/execEnv/lib")
set(LIMA_RESOURCES "${CMAKE_BINARY_DIR}/execEnv/resources")

add_subdirectory(lima_conll_dependency_tag_mapping)

foreach(LANG ${LIMA_LANGUAGES})

  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/VerbNet-${LANG}.rules)

    add_custom_command(
      OUTPUT VerbNet-${LANG}.bin
      COMMAND compile-rules --resourcesDir=${LIMA_RESOURCES} --configDir=${LIMA_CONF} --language=${LANG} -oVerbNet-${LANG}.bin --modex=VerbNet-modex.xml ${CMAKE_CURRENT_SOURCE_DIR}/VerbNet-${LANG}.rules
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/VerbNet-${LANG}.rules rules-${LANG}-execEnv rules-configEnv compile-rules
#       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM
    )

    add_custom_target(
      rules-VerbNet-${LANG}-main
      ALL
      DEPENDS VerbNet-${LANG}.bin
    )

    add_dependencies(rules-VerbNet-${LANG}-main
      rules-${LANG}-execEnv
      rules-configEnv
      compile-rules
    )

    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/VerbNet-${LANG}.bin
      COMPONENT ${LANG}
      DESTINATION share/apps/lima/resources/SRLIntegration
    )

  endif ()

  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FrameNet-${LANG}.rules)

    add_custom_command(
      OUTPUT FrameNet-${LANG}.bin
      COMMAND compile-rules --resourcesDir=${LIMA_RESOURCES} --configDir=${LIMA_CONF} --language=${LANG} -oFrameNet-${LANG}.bin --modex=FrameNet-modex.xml ${CMAKE_CURRENT_SOURCE_DIR}/FrameNet-${LANG}.rules
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/FrameNet-${LANG}.rules rules-${LANG}-execEnv rules-configEnv compile-rules
#       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM
    )

    add_custom_target(
      rules-FrameNet-${LANG}-main
      ALL
      DEPENDS FrameNet-${LANG}.bin
    )

    # add the link between the current target and its execution environment dependencies
    add_dependencies(rules-FrameNet-${LANG}-main
      rules-${LANG}-execEnv
      rules-configEnv
      compile-rules
    )

    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/FrameNet-${LANG}.bin
      COMPONENT ${LANG}
      DESTINATION share/apps/lima/resources/SRLIntegration
    )

  endif ()

endforeach(LANG ${LIMA_LANGUAGES})

install(FILES FrameNet-modex.xml VerbNet-modex.xml COMPONENT common DESTINATION share/config/lima)
