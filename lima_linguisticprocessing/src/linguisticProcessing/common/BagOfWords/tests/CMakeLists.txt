# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next test ###############

SET(BagOfWordsTest0_SRCS
   BagOfWordsTest0.cpp
)

add_executable(BagOfWordsTest0 ${BagOfWordsTest0_SRCS})

target_link_libraries(BagOfWordsTest0
  lima-lp-bagofwords
  Boost::unit_test_framework
  Qt5::Test
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
  Qt5::Test
)

add_test(NAME BagOfWordsTest1 COMMAND BagOfWordsTest1)

########### next test ###############

SET(BagOfWordsTest2_SRCS BagOfWordsTest2.cpp)

add_executable(BagOfWordsTest2 ${BagOfWordsTest2_SRCS})

target_link_libraries(BagOfWordsTest2
  lima-lp-bagofwords
  Qt5::Test
)

add_test(NAME BagOfWordsTest2 COMMAND BagOfWordsTest2)

########### next test ###############

SET(BagOfWordsTest3_SRCS BagOfWordsTest3.cpp)

add_executable(BagOfWordsTest3 ${BagOfWordsTest3_SRCS})

target_link_libraries(BagOfWordsTest3
  lima-lp-bagofwords
  Qt5::Test
)

add_test(NAME BagOfWordsTest3 COMMAND BagOfWordsTest3)

