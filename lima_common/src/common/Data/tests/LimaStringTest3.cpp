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

using namespace Lima;

// substrings
BOOST_AUTO_TEST_CASE( LimaStringTest3_1 )
{
  LimaString s = Common::Misc::utf8stdstring2limastring("abcdef");
  LimaString sub = s.mid(1,2);
  BOOST_REQUIRE( Common::Misc::limastring2utf8stdstring(sub) == "bc" );
  BOOST_REQUIRE( sub == Common::Misc::utf8stdstring2limastring("bc") );
}

// operators
BOOST_AUTO_TEST_CASE( LimaStringTest3_2 )
{
  LimaString s1 = Common::Misc::utf8stdstring2limastring("abc");
  LimaString s2 = Common::Misc::utf8stdstring2limastring("def");
  LimaString s3 = s1 + s2[1];
  BOOST_REQUIRE( s3 == Common::Misc::utf8stdstring2limastring("abce") );
}
