// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TimeUtilsTest
#include <boost/test/unit_test.hpp>

#include "common/time/timeUtilsController.h"

using namespace Lima;

// conversion functions
BOOST_AUTO_TEST_CASE( TimeUtilsTest0 )
{
  Lima::TimeUtilsController *timer = new Lima::TimeUtilsController("test");
  delete timer;
  
  BOOST_REQUIRE( true );
}

