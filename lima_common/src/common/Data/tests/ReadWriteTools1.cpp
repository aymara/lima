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
// clazy:skip

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LimaStringTest
#include <boost/test/unit_test.hpp>

#include "common/Data/LimaString.h"
#include "common/Data/readwritetools.h"

using namespace Lima;

// Test coded int read/write function with various int values

BOOST_AUTO_TEST_CASE( ReadWriteTools1_1 )
{
  uint64_t written_number = 0;
  std::ostringstream oss;
  Lima::Common::Misc::writeCodedInt(oss, written_number);
  std::string buffer = oss.str();

  std::istringstream iss(buffer);
  uint16_t read_number = Lima::Common::Misc::readCodedInt(iss);
  BOOST_REQUIRE( written_number == read_number );
}

BOOST_AUTO_TEST_CASE( ReadWriteTools1_2 )
{
  uint64_t written_number = 5;
  std::ostringstream oss;
  Lima::Common::Misc::writeCodedInt(oss, written_number);
  std::streampos ppos = oss.tellp();
  std::istringstream iss(oss.str());
  uint64_t read_number = Lima::Common::Misc::readCodedInt(iss);
  std::streampos gpos = iss.tellg();
  BOOST_REQUIRE( gpos == ppos );

  BOOST_REQUIRE( written_number == read_number );
}

/// Read and write the higher 32 bits integer
BOOST_AUTO_TEST_CASE( ReadWriteTools1_3 )
{
  uint64_t written_number = std::numeric_limits<uint32_t>::max();
  std::string buffer;
  std::ostringstream oss(buffer);
  Lima::Common::Misc::writeCodedInt(oss, written_number);

  std::istringstream iss(oss.str());
  auto read_number = Lima::Common::Misc::readCodedInt(iss);
  BOOST_REQUIRE( written_number == read_number );
}

BOOST_AUTO_TEST_CASE( ReadWriteTools1_4 )
{
  auto written_number = std::numeric_limits<uint64_t>::max();
  std::string buffer;
  std::ostringstream oss(buffer);
  Lima::Common::Misc::writeCodedInt(oss, written_number);

  std::istringstream iss(oss.str());
  auto read_number = Lima::Common::Misc::readCodedInt(iss);
  BOOST_REQUIRE( written_number == read_number );
}
