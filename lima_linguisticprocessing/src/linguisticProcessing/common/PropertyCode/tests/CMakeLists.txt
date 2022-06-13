#   Copyright 2002-2020 CEA LIST
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

SET(PropertyCodeTest0_SRCS PropertyCodeTest0.cpp)

set (PropertyCodeTest0_MOC_HEADERS PropertyCodeTest0.h)

add_executable(PropertyCodeTest0 ${PropertyCodeTest0_SRCS} ${PropertyCodeTest0_MOC_OUTFILES})
target_link_libraries(PropertyCodeTest0 lima-linguisticprocessing Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME PropertyCodeTest0 COMMAND PropertyCodeTest0)

