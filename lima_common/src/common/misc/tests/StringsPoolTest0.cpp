// clazy:skip

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE StringsPoolTest
#include <boost/test/unit_test.hpp>

#include "common/misc/stringspool.h"

using namespace Lima;

// conversion functions
BOOST_AUTO_TEST_CASE( StringsPoolTest0 )
{
  LimaString str = "abc";
  StringsPool pool;
  StringsPoolIndex index1 = pool[str];
  StringsPoolIndex index2 = pool[str];
  BOOST_REQUIRE( index1 == index2 );
}

