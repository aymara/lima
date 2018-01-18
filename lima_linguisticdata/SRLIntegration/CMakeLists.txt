#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>

if(NOT WIN32)
  set(LIMA_PATH_SEPARATOR ":")
else()
  set(LIMA_PATH_SEPARATOR "\;") # WANING: must be protected against cmake semicolon substitution
endif()

set(LIMA_CONF "${CMAKE_BINARY_DIR}/execEnv/config${LIMA_PATH_SEPARATOR}$ENV{LIMA_CONF}")
set(LIMA_RESOURCES "${CMAKE_BINARY_DIR}/execEnv/resources${LIMA_PATH_SEPARATOR}$ENV{LIMA_RESOURCES}")

add_subdirectory(lima_conll_dependency_tag_mapping)

foreach(LANG ${LIMA_LANGUAGES})



  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/VerbNet-${LANG}.rules)

    add_custom_command(
      OUTPUT VerbNet-${LANG}.bin
      COMMAND compile-rules --resourcesDir=${LIMA_RESOURCES} --configDir=${LIMA_CONF} --language=${LANG} -oVerbNet-${LANG}.bin --modex=VerbNet-modex.xml ${CMAKE_CURRENT_SOURCE_DIR}/VerbNet-${LANG}.rules
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/VerbNet-${LANG}.rules
#       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM
    )

    add_custom_target(
      rules-VerbNet-${LANG}-main
      ALL
      DEPENDS VerbNet-${LANG}.bin
    )

    add_dependencies(
      rules-VerbNet-${LANG}-main 
      rules-${_lang}-execEnv rules-configEnv)
    
    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/VerbNet-${LANG}.bin 
      COMPONENT ${LANG} 
      DESTINATION share/apps/lima/resources/SRLIntegration)


  endif ()
  
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FrameNet-${LANG}.rules)

    add_custom_command(
      OUTPUT FrameNet-${LANG}.bin
      COMMAND compile-rules --resourcesDir=${LIMA_RESOURCES} --configDir=${LIMA_CONF} --language=${LANG} -oFrameNet-${LANG}.bin --modex=FrameNet-modex.xml ${CMAKE_CURRENT_SOURCE_DIR}/FrameNet-${LANG}.rules
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/FrameNet-${LANG}.rules
#       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM
    )

    add_custom_target(
      rules-FrameNet-${LANG}-main
      ALL
      DEPENDS FrameNet-${LANG}.bin
    )

    # add the link between the current target and its execution environment dependencies
    add_dependencies(
      rules-FrameNet-${LANG}-main 
      rules-${_lang}-execEnv rules-configEnv)

    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/FrameNet-${LANG}.bin 
      COMPONENT ${LANG} 
      DESTINATION share/apps/lima/resources/SRLIntegration)


  endif ()

endforeach(LANG ${LIMA_LANGUAGES})
