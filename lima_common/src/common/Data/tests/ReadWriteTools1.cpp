// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
