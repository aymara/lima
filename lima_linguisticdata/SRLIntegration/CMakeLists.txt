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

add_subdirectory(lima_conll_dependency_tag_mapping)

set (DEPENDENCIES )

foreach(LANG ${LIMA_LANGUAGES})

  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/VerbNet-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SRLIntegration/VerbNet-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/VerbNet-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SRLIntegration/VerbNet-modex.xml
    COMMENT "create config env for srl rules (VerbNet-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/FrameNet-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SRLIntegration/FrameNet-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/FrameNet-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SRLIntegration/FrameNet-modex.xml
    COMMENT "create config env for srl rules (FrameNet-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/lima-common-${LANG}.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-common-${LANG}.xml
      ${CMAKE_BINARY_DIR}/execEnv/config/lima-common-${LANG}.xml
    DEPENDS
      ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-common-${LANG}.xml
    COMMENT "create config env for specific entities rules (lima-common-${LANG}.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/lima-lp-${LANG}.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-lp-${LANG}.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/lima-lp-${LANG}.xml
    DEPENDS
      ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-lp-${LANG}.xml
    COMMENT "create config env for specific entities rules (lima-lp-${LANG}.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/lima-analysis.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-analysis.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/lima-analysis.xml
    DEPENDS
      ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-analysis.xml
    COMMENT "create config env for specific entities rules (lima-analysis.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/lima-common.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-common.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/lima-common.xml
    DEPENDS
      ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-common.xml
    COMMENT "create config env for specific entities rules (lima-common.xml)"
    VERBATIM
  )

  # defini l'ensemble des dépendances (ce qui doit exister dans la partie configuration de l'environnement
  # d'execution de la cible rules-VerbNet-${LANG}-main
  add_custom_target(
    rules-${LANG}-VerbNet-configEnv
    ALL
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/VerbNet-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-common-${LANG}.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-lp-${LANG}.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-analysis.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-common.xml
  )
  add_custom_target(
    rules-${LANG}-FrameNet-configEnv
    ALL
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/FrameNet-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-common-${LANG}.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-lp-${LANG}.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-analysis.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-common.xml
  )

  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/${LANG}/code-${LANG}.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/${LANG}
    COMMAND ${CMAKE_COMMAND} -E copy 
      ${CMAKE_SOURCE_DIR}/analysisDictionary/${LANG}/code/code-${LANG}.xml
      ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/${LANG}/code-${LANG}.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/analysisDictionary/${LANG}/code/code-${LANG}.xml
    COMMENT "create exec env for ${LANG} srl rules (code)"
    VERBATIM
  )

  # defini l'ensemble des dépendances (ce qui doit exister dans l'environnement d'execution)
  # de la cible rules-DateTime-${LANG}main
  add_custom_target(
    srl-rules-${LANG}-execEnv
    ALL
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/${LANG}/code-${LANG}.xml
  )

# /home/clemence/Projets/Lima/Dist/srlloader/debug/share/config/lima/VerbNet-modex.xmlma-analysis.xml », nécessaire pour « execEnv/config/lima-analysis.xml


  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/VerbNet-${LANG}.rules)

    add_custom_command(
      OUTPUT VerbNet-${LANG}.bin
      COMMAND compile-rules --resourcesDir=${CMAKE_BINARY_DIR}/execEnv/resources --configDir=${CMAKE_BINARY_DIR}/execEnv/config --debug --language=${LANG} -oVerbNet-${LANG}.bin ${_current} --modex=VerbNet-modex.xml ${CMAKE_CURRENT_SOURCE_DIR}/VerbNet-${LANG}.rules
      DEPENDS ${_current} ${DEPENDENCIES}
#       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM
    )

    add_custom_target(
      rules-VerbNet-${LANG}-main
      ALL
      DEPENDS VerbNet-${LANG}.bin
    )

    # add the link between the current target and its execution environment dependencies
    add_dependencies(rules-VerbNet-${LANG}-main rules-${LANG}-VerbNet-configEnv-main rules-${LANG}-execEnv)

    add_dependencies(rules-VerbNet-${LANG}-main rules-${LANG}-VerbNet-configEnv srl-rules-${LANG}-execEnv)
    
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/VerbNet-${LANG}.bin COMPONENT ${LANG} DESTINATION share/apps/lima/resources/SRLIntegration)


  endif ()
  
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FrameNet-${LANG}.rules)

    add_custom_command(
      OUTPUT FrameNet-${LANG}.bin
      COMMAND compile-rules --resourcesDir=${CMAKE_BINARY_DIR}/execEnv/resources --configDir=${CMAKE_BINARY_DIR}/execEnv/config --language=${LANG} -oFrameNet-${LANG}.bin ${_current} --modex=FrameNet-modex.xml ${CMAKE_CURRENT_SOURCE_DIR}/FrameNet-${LANG}.rules
      DEPENDS ${_current} ${DEPENDENCIES}
#       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM
    )

    add_custom_target(
      rules-FrameNet-${LANG}-main
      ALL
      DEPENDS FrameNet-${LANG}.bin
    )

    # add the link between the current target and its execution environment dependencies
    add_dependencies(rules-FrameNet-${LANG}-main rules-${LANG}-FrameNet-configEnv-main rules-${LANG}-execEnv)

    add_dependencies(rules-FrameNet-${LANG}-main rules-${LANG}-FrameNet-configEnv srl-rules-${LANG}-execEnv)
    
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FrameNet-${LANG}.bin COMPONENT ${LANG} DESTINATION share/apps/lima/resources/SRLIntegration)


  endif ()

endforeach(LANG ${LIMA_LANGUAGES})


install(FILES FrameNet-modex.xml VerbNet-modex.xml COMPONENT common DESTINATION share/config/lima)
