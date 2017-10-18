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
if (CMAKE_BUILD_TYPE MATCHES Debug)
option(BUILD_SVMTOOL_MODELS "Set to true to build and install the SVMTool PoS \
                             tagging models. Default is FALSE to avoid \
                             training times."
                             OFF)
else()
option(BUILD_SVMTOOL_MODELS "Set to true to build and install the SVMTool PoS tagging models. Default is FALSE to avoid  training times." ON)
endif()

foreach(LANG ${LIMA_LANGUAGES})
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LANG})
    add_subdirectory(${LANG})
  endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LANG})
endforeach(LANG ${LIMA_LANGUAGES})
