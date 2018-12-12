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
################### tests ########################

########### next test ###############

SET(AnnotationGraphTest0_SRCS
   AnnotationGraphTest0.cpp
)

add_executable(AnnotationGraphTest0 ${AnnotationGraphTest0_SRCS})

target_link_libraries(AnnotationGraphTest0
  lima-common-data
  lima-common-tools
  lima-lp-annotationgraph
  ${Boost_LIBRARIES}
  ${Qt5_LIBRARIES}
)

add_test(NAME AnnotationGraphTest0 COMMAND AnnotationGraphTest0)

