# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### install files ###############

# The deprecated legacy eng/fre/por media are only declared available (and
# thus loaded by clients that initialize every available media, like the GUI)
# when they are built.
if (WITH_LEGACY_PIPELINES)
  set(LIMA_LEGACY_MEDIA_ITEMS "<item value=\"eng\" />
        <item value=\"fre\" />
        <item value=\"por\" />")
else()
  set(LIMA_LEGACY_MEDIA_ITEMS "<!-- Legacy eng, fre and por media: built with WITH_LEGACY_PIPELINES=ON -->")
endif()
configure_file(lima-analysis.xml lima-analysis.xml @ONLY)
configure_file(lima-common.xml lima-common.xml @ONLY)

install(FILES
    log4cpp.properties
    ${CMAKE_CURRENT_BINARY_DIR}/lima-analysis.xml
    ${CMAKE_CURRENT_BINARY_DIR}/lima-common.xml
    lima-common-ud.xml # Generic config for Universal Dependencies
    lima-common-ud-eng.xml # English, deeplima backend
    lima-common-ud-fra.xml # French, deeplima backend
  DESTINATION share/config/lima
  COMPONENT runtime)

if (WITH_LEGACY_PIPELINES)
  # DEPRECATED legacy eng/fre/por media (rule-based/statistical analysis)
  install(FILES
      lima-common-eng.xml
      lima-common-fre.xml
      lima-common-por.xml
    DESTINATION share/config/lima
    COMPONENT runtime)
endif()

  install(FILES
    log4cpp/limacommon.log.properties
  DESTINATION share/config/lima/log4cpp
  COMPONENT runtime)
