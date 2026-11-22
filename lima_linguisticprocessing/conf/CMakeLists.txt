# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### install files ###############

set (PosTagger "viterbiPostagger-freq")
if (SVMTOOL++_FOUND)
  set (PosTagger "SvmToolPosTagger")
endif ()
if(TRE_FOUND)
  configure_file(ApproxNames-modex.xml ApproxNames-modex.xml @ONLY)
else(TRE_FOUND)
  configure_file(ApproxNames-modex.empty.xml ApproxNames-modex.xml @ONLY)
endif(TRE_FOUND)

configure_file(lima-lp-ud.xml lima-lp-ud.xml @ONLY)
configure_file(lima-lp-ud-eng.xml lima-lp-ud-eng.xml @ONLY)
configure_file(lima-lp-ud-fra.xml lima-lp-ud-fra.xml @ONLY)
# The legacy configurations are always generated (the execEnv of a legacy build
# copies them from here) but only installed with WITH_LEGACY_PIPELINES.
configure_file(lima-lp-eng.xml lima-lp-eng.xml @ONLY)
configure_file(lima-lp-fre.xml lima-lp-fre.xml @ONLY)
configure_file(lima-lp-por.xml lima-lp-por.xml @ONLY)

# limaserver initializes every language of its list at startup
if (WITH_LEGACY_PIPELINES)
  set(LIMA_SERVER_LEGACY_LANGUAGES "<item value=\"eng\" />
        <item value=\"fre\" />
        <item value=\"por\" />")
else()
  set(LIMA_SERVER_LEGACY_LANGUAGES "<!-- Legacy eng, fre and por media: built with WITH_LEGACY_PIPELINES=ON -->")
endif()
configure_file(lima-server.xml lima-server.xml @ONLY)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-ud.xml
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-ud-eng.xml
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-ud-fra.xml
  lima-lp-tva.xml
  ${CMAKE_CURRENT_BINARY_DIR}/lima-server.xml
  ${CMAKE_CURRENT_BINARY_DIR}/ApproxNames-modex.xml
  lima-analysis-art.xml
  lima-lp-tvx.xml
  DESTINATION share/config/lima
  COMPONENT runtime
)

if (WITH_LEGACY_PIPELINES)
  # DEPRECATED legacy eng/fre/por media (rule-based/statistical analysis)
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-eng.xml
    ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-fre.xml
    ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-por.xml
    lima-lp-tva-eng.xml
    lima-lp-tva-fre.xml
    lima-lp-tva-por.xml
    lima-lp-tvx-fre.xml
    lima-dbusserver.xml
    DESTINATION share/config/lima
    COMPONENT runtime
  )
endif()

install(
  FILES
    log4cpp/limalp.log.properties
  DESTINATION share/config/lima/log4cpp
  COMPONENT runtime
)
