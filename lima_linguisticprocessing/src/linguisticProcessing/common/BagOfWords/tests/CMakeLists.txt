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
################### tests ########################

########### next test ###############

SET(BagOfWordsTest0_SRCS
   BagOfWordsTest0.cpp
)

add_executable(BagOfWordsTest0 ${BagOfWordsTest0_SRCS})

target_link_libraries(BagOfWordsTest0
  lima-lp-bagofwords
  Boost::unit_test_framework
  Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME BagOfWordsTest0 COMMAND BagOfWordsTest0)

########### next test ###############

SET(BagOfWordsTest1_SRCS
   BagOfWordsTest1.cpp
)

add_executable(BagOfWordsTest1 ${BagOfWordsTest1_SRCS})

target_link_libraries(BagOfWordsTest1
  lima-lp-bagofwords
  Boost::unit_test_framework
  Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME BagOfWordsTest1 COMMAND BagOfWordsTest1)

########### next test ###############

SET(BagOfWordsTest2_SRCS BagOfWordsTest2.cpp)

add_executable(BagOfWordsTest2 ${BagOfWordsTest2_SRCS})

target_link_libraries(BagOfWordsTest2
  lima-lp-bagofwords
  Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME BagOfWordsTest2 COMMAND BagOfWordsTest2)

########### next test ###############

SET(BagOfWordsTest3_SRCS BagOfWordsTest3.cpp)

add_executable(BagOfWordsTest3 ${BagOfWordsTest3_SRCS})

target_link_libraries(BagOfWordsTest3
  lima-lp-bagofwords
  Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME BagOfWordsTest3 COMMAND BagOfWordsTest3)

