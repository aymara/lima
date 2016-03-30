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

SET(BagOfWordsTest0_SRCS
   BagOfWordsTest0.cpp
)

add_executable(BagOfWordsTest0 ${BagOfWordsTest0_SRCS})

target_link_libraries(BagOfWordsTest0
  lima-common-data
  lima-common-qslog
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-linguisticdata
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)

add_test(BagOfWordsTest0 ${CMAKE_BUILD_TYPE}/BagOfWordsTest0)

########### next test ###############

SET(BagOfWordsTest1_SRCS
   BagOfWordsTest1.cpp
)

add_executable(BagOfWordsTest1 ${BagOfWordsTest1_SRCS})

target_link_libraries(BagOfWordsTest1
  lima-common-data
  lima-common-qslog
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-linguisticdata
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)

add_test(BagOfWordsTest1 ${CMAKE_BUILD_TYPE}/BagOfWordsTest1)

########### next test ###############

SET(BagOfWordsTest2_SRCS BagOfWordsTest2.cpp)

set (BagOfWordsTest2_MOC_HEADERS BagOfWordsTest2.h)
qt5_wrap_cpp (BagOfWordsTest2_MOC_OUTFILES ${BagOfWordsTest2_MOC_HEADERS})

add_executable(BagOfWordsTest2 ${BagOfWordsTest2_SRCS} ${BagOfWordsTest2_MOC_OUTFILES})
target_link_libraries(BagOfWordsTest2
  lima-common-data
  lima-common-mediaticdata
  lima-common-qslog
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-linguisticdata
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)

add_test(BagOfWordsTest2 ${CMAKE_BUILD_TYPE}/BagOfWordsTest2)

########### next test ###############

SET(BagOfWordsTest3_SRCS BagOfWordsTest3.cpp)

set (BagOfWordsTest3_MOC_HEADERS BagOfWordsTest3.h)
qt5_wrap_cpp (BagOfWordsTest3_MOC_OUTFILES ${BagOfWordsTest3_MOC_HEADERS})

add_executable(BagOfWordsTest3 ${BagOfWordsTest3_SRCS} ${BagOfWordsTest3_MOC_OUTFILES})
target_link_libraries(BagOfWordsTest3
  lima-common-data
  lima-common-mediaticdata
  lima-common-qslog
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-linguisticdata
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)

add_test(BagOfWordsTest3 ${CMAKE_BUILD_TYPE}/BagOfWordsTest3)

