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

target_link_libraries(LimaStringTest0 lima-common ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(LimaStringTest0 LimaStringTest0)

########### next test ###############

SET(LimaStringTest1_SRCS
   LimaStringTest1.cpp
)

add_executable(LimaStringTest1 ${LimaStringTest1_SRCS})

target_link_libraries(LimaStringTest1 lima-common ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(LimaStringTest1 LimaStringTest1)

########### next test ###############

SET(LimaStringTest2_SRCS
   LimaStringTest2.cpp
)

add_executable(LimaStringTest2 ${LimaStringTest2_SRCS})

target_link_libraries(LimaStringTest2 lima-common ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(LimaStringTest2 LimaStringTest2)


########### next test ###############

SET(LimaStringTest3_SRCS
   LimaStringTest3.cpp
)

add_executable(LimaStringTest3 ${LimaStringTest3_SRCS})

target_link_libraries(LimaStringTest3 lima-common ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(LimaStringTest3 LimaStringTest3)


########### next test ###############

SET(ReadWriteTools1_SRCS
   ReadWriteTools1.cpp
)

add_executable(ReadWriteTools1 ${ReadWriteTools1_SRCS})

target_link_libraries(ReadWriteTools1 lima-common ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(ReadWriteTools1 ReadWriteTools1)

########### next test ###############

SET(ReadWriteTools2_SRCS
   ReadWriteTools2.cpp
)

add_executable(ReadWriteTools2 ${ReadWriteTools2_SRCS})

target_link_libraries(ReadWriteTools2 lima-common ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(ReadWriteTools2 ReadWriteTools2)
