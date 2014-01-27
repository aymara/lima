/*
    Copyright 2002-2013 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
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
