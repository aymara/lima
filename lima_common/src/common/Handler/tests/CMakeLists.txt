#   Copyright 2021 CEA LIST
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

SET(ShiftFromTest0_SRCS
   ShiftFromTest0.cpp
)

add_executable(ShiftFromTest0 ${ShiftFromTest0_SRCS} ${ShiftFromTest0_MOC_OUTFILES})

target_link_libraries(ShiftFromTest0
  lima-common-datahandler
  Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME ShiftFromTest0 COMMAND ShiftFromTest0)

