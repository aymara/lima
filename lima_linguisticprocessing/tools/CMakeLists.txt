#   Copyright 2002-2019 CEA LIST
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

#add_subdirectory(applyAutomaton) # n'est pas compile dans S2!!!
add_subdirectory(automatonCompiler)
add_subdirectory(common) # executable de s2common
add_subdirectory(dictionary)
add_subdirectory(evalCoref)
add_subdirectory(normalize)
# add_subdirectory(lima-lp-tgv) # --> src/linguisticProcessing/common/lima-lp-tgv
add_subdirectory(tva)
add_subdirectory(tvx)

include_directories(${CMAKE_SOURCE_DIR}/src )

########### install files ###############
