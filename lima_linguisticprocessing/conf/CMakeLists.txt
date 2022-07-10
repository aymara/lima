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

set (tensorflowSpecificEntities "")
if (TensorFlow_FOUND AND Eigen_FOUND)
  set (tensorflowSpecificEntities "         <item value=\"tensorflowSpecificEntities\"/>")
endif()

configure_file(lima-lp-eng.xml lima-lp-eng.xml @ONLY)
configure_file(lima-lp-fre.xml lima-lp-fre.xml @ONLY)
configure_file(lima-lp-por.xml lima-lp-por.xml @ONLY)
configure_file(lima-lp-ud.xml lima-lp-ud.xml @ONLY)
configure_file(lima-lp-ud-eng.xml lima-lp-ud-eng.xml @ONLY)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-eng.xml
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-fre.xml
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-por.xml
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-ud.xml
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-ud-eng.xml
  lima-lp-tva.xml
  lima-lp-tva-eng.xml
  lima-lp-tva-fre.xml
  lima-lp-tva-por.xml
  lima-server.xml
  lima-dbusserver.xml
  ${CMAKE_CURRENT_BINARY_DIR}/ApproxNames-modex.xml
  lima-analysis-art.xml
  lima-lp-tvx.xml
  lima-lp-tvx-fre.xml
  DESTINATION share/config/lima
  COMPONENT runtime
)

install(
  FILES
    log4cpp/limalp.log.properties
  DESTINATION share/config/lima/log4cpp
  COMPONENT runtime
)
