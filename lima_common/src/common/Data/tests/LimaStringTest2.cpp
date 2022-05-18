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

// copy construction
BOOST_AUTO_TEST_CASE( LimaStringTest2_1 )
{
  LimaString s1 = Common::Misc::utf8stdstring2limastring("abc");
  BOOST_REQUIRE( s1.size() == 3);

  LimaString s2(s1);
  BOOST_REQUIRE( s2.size() == 3);
  
  BOOST_REQUIRE( s1 == s2 );
  
  LimaString s3 = Common::Misc::utf8stdstring2limastring("abc");
  BOOST_REQUIRE( s1 == s3 );
}
