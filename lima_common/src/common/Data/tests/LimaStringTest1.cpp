// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LimaStringTest
#include <boost/test/unit_test.hpp>

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include <iostream>

using namespace Lima;

// basic construction
BOOST_AUTO_TEST_CASE( LimaStringTest1_1 )
{
  LimaString s;
  BOOST_REQUIRE( s.size() == 0);
}

// basic construction
BOOST_AUTO_TEST_CASE( LimaStringTest1_2 )
{
  LimaString s = Common::Misc::utf8stdstring2limastring("abc");
  BOOST_REQUIRE( s.size() == 3);
}

// basic construction
BOOST_AUTO_TEST_CASE( LimaStringTest1_3 )
{
  LimaString s = Common::Misc::utf8stdstring2limastring("aéc");
  BOOST_REQUIRE( s.size() == 3);
}

// equality testing
BOOST_AUTO_TEST_CASE( LimaStringTest1_4 )
{
  LimaString s1 = Common::Misc::utf8stdstring2limastring("abc");
  LimaString s2 = Common::Misc::utf8stdstring2limastring("abc");
  BOOST_REQUIRE( s1 == s2 );
}
