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

########### install files ###############

set (PosTagger "viterbiPostagger-freq")
if (SVMTOOL++_FOUND)
  set (PosTagger "SvmToolPosTagger")
endif ()
IF(TRE_FOUND)
configure_file(ApproxNames-modex.xml ApproxNames-modex.xml @ONLY)
ELSE(TRE_FOUND)
configure_file(ApproxNames-modex.empty.xml ApproxNames-modex.xml @ONLY)
ENDIF(TRE_FOUND)

configure_file(lima-lp-eng.xml lima-lp-eng.xml @ONLY)
configure_file(lima-lp-fre.xml lima-lp-fre.xml @ONLY)
configure_file(lima-lp-por.xml lima-lp-por.xml @ONLY)


install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-fre.xml 
  lima-lp-tva-fre.xml 
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-eng.xml 
  lima-lp-tva.xml 
  lima-lp-tva-eng.xml
  ${CMAKE_CURRENT_BINARY_DIR}/lima-lp-por.xml 
  lima-lp-tva-por.xml
  lima-server.xml
  ${CMAKE_CURRENT_BINARY_DIR}/ApproxNames-modex.xml
  lima-dbusserver.xml 
  DESTINATION share/config/lima
  COMPONENT runtime)
