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
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

########### next target ###############

SET(normalizeTerm_SRCS
   normalizeTerm.cpp
)

add_executable(normalizeTerm ${normalizeTerm_SRCS})

target_link_libraries(normalizeTerm lima-linguisticprocessing)

install(TARGETS normalizeTerm DESTINATION bin)


########### next target ###############

SET(desaccent_SRCS
   desaccent.cpp
)

add_executable(desaccent ${desaccent_SRCS})

target_link_libraries(desaccent lima-linguisticprocessing)

install(TARGETS desaccent DESTINATION bin)


########### install files ###############
