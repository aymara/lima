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

// conversion functions
BOOST_AUTO_TEST_CASE( LimaStringTest0 )
{
  BOOST_REQUIRE( Common::Misc::limastring2utf8stdstring(Common::Misc::utf8stdstring2limastring("abcé")) == "abcé" );
}
