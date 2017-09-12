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

#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "common/Data/strwstrtools.h"
#include "common/QsLog/QsLog.h"
#include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/QsLog/QsDebugOutput.h"

using namespace Lima;
using namespace Lima::LinguisticProcessing::FlatTokenizer;

// unmarking of consonant character is itself
BOOST_AUTO_TEST_CASE( CharChartTestAra_0 )
{
  QsLogging::initQsLog();
  CharChart cc;
  BOOST_REQUIRE( cc.loadFromFile("testcharchart-ara.chars.tok") );
  BOOST_REQUIRE_MESSAGE( cc.unmark(LimaString::fromUtf8("ن")[0]) == LimaString::fromUtf8(  "ن"  )[0], (std::string("unmark(ن) Got ") + LimaString(cc.unmark(LimaString::fromUtf8("ن")[0])).toUtf8().data() + " instead of ن").c_str() );
}

// // unmarking of accented lowercase character
// BOOST_AUTO_TEST_CASE( CharChartTestAra_1 )
// {
//   CharChart cc;
//   BOOST_REQUIRE( cc.loadFromFile("testcharchart.chars.tok") );
//   BOOST_REQUIRE( cc.unmark(LimaString::fromUtf8("é")[0]) == LimaString::fromUtf8("e")[0] );
// }
// 
// // unmarking of uppercase character
// BOOST_AUTO_TEST_CASE( CharChartTestAra_2 )
// {
//   CharChart cc;
//   BOOST_REQUIRE( cc.loadFromFile("testcharchart.chars.tok") );
//   BOOST_REQUIRE( cc.unmark(LimaString::fromUtf8("A")[0]) == LimaString::fromUtf8("a")[0] );
// }
// 
// // unmarking of accented uppercase character is the unaccented lowercase one
// BOOST_AUTO_TEST_CASE( CharChartTestAra_3 )
// {
//   CharChart cc;
//   BOOST_REQUIRE( cc.loadFromFile("testcharchart.chars.tok") );
//   BOOST_REQUIRE( cc.unmark(LimaString::fromUtf8("É")[0]) == LimaString::fromUtf8("e")[0] );
// }
// 
// // unmarking of non-letter char is null
// BOOST_AUTO_TEST_CASE( CharChartTestAra_4 )
// {
//   CharChart cc;
//   BOOST_REQUIRE( cc.loadFromFile("testcharchart.chars.tok") );
//   BOOST_REQUIRE( cc.unmark(LimaString::fromUtf8(".")[0]) == LimaChar() );
// }
// 
// // unmarking of acronym
// BOOST_AUTO_TEST_CASE( CharChartTestAra_5 )
// {
//   CharChart cc;
//   BOOST_REQUIRE( cc.loadFromFile("testcharchart.chars.tok") );
//   BOOST_REQUIRE_MESSAGE( cc.unmark(LimaString::fromUtf8("A.A.")) == LimaString::fromUtf8("aa"), (std::string("unmark(A.A.) Got ") + cc.unmark(LimaString::fromUtf8("A.A.")).toUtf8().data() + " instead of aa").c_str() );
// }
