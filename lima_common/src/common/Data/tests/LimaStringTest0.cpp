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

// conversion functions
BOOST_AUTO_TEST_CASE( LimaStringTest0 )
{
  BOOST_REQUIRE( Common::Misc::limastring2utf8stdstring(Common::Misc::utf8stdstring2limastring("abcé")) == "abcé" );
}
