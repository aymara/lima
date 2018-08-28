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

SET(LimaStringTest0_SRCS
   LimaStringTest0.cpp
)

add_executable(LimaStringTest0 ${LimaStringTest0_SRCS})

target_link_libraries(LimaStringTest0 lima-common ${Boost_LIBRARIES} ${Qt5_LIBRARIES})

add_test(NAME LimaStringTest0 COMMAND LimaStringTest0)

########### next test ###############

SET(LimaStringTest1_SRCS
   LimaStringTest1.cpp
)

add_executable(LimaStringTest1 ${LimaStringTest1_SRCS})

target_link_libraries(LimaStringTest1 lima-common ${Boost_LIBRARIES} ${Qt5_LIBRARIES})

add_test(NAME LimaStringTest1 COMMAND LimaStringTest1)

########### next test ###############

SET(LimaStringTest2_SRCS
   LimaStringTest2.cpp
)

add_executable(LimaStringTest2 ${LimaStringTest2_SRCS})

target_link_libraries(LimaStringTest2 lima-common ${Boost_LIBRARIES} ${Qt5_LIBRARIES})

add_test(NAME LimaStringTest2 COMMAND LimaStringTest2)


########### next test ###############

SET(LimaStringTest3_SRCS
   LimaStringTest3.cpp
)

add_executable(LimaStringTest3 ${LimaStringTest3_SRCS})

target_link_libraries(LimaStringTest3 lima-common ${Boost_LIBRARIES} ${Qt5_LIBRARIES})

add_test(NAME LimaStringTest3 COMMAND LimaStringTest3)


########### next test ###############

SET(ReadWriteTools1_SRCS
   ReadWriteTools1.cpp
)

add_executable(ReadWriteTools1 ${ReadWriteTools1_SRCS})

target_link_libraries(ReadWriteTools1 lima-common ${Boost_LIBRARIES} ${Qt5_LIBRARIES})

add_test(NAME ReadWriteTools1 COMMAND ReadWriteTools1)

########### next test ###############

SET(ReadWriteTools2_SRCS
   ReadWriteTools2.cpp
)

add_executable(ReadWriteTools2 ${ReadWriteTools2_SRCS})

target_link_libraries(ReadWriteTools2 lima-common ${Boost_LIBRARIES} ${Qt5_LIBRARIES})

add_test(NAME ReadWriteTools2 COMMAND ReadWriteTools2)

########### next test ###############
# set (FileUtilsTest0_MOC_HEADERS FileUtilsTest.h)
# qt5_wrap_cpp (FileUtilsTest0_MOC_OUTFILES ${FileUtilsTest0_MOC_HEADERS})


SET(FileUtilsTest0_SRCS 
  FileUtilsTest.cpp
)

add_executable(FileUtilsTest0 ${FileUtilsTest0_SRCS} ${FileUtilsTest0_MOC_OUTFILES})

target_link_libraries(FileUtilsTest0 lima-common ${Boost_LIBRARIES} ${Qt5_LIBRARIES})

add_test(NAME FileUtilsTest0 COMMAND FileUtilsTest0 -v2 -nocrashhandler)
set_tests_properties(FileUtilsTest0 PROPERTIES ENVIRONMENT LIMA_CONF=${CMAKE_SOURCE_DIR}/conf)


