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
#define BOOST_TEST_MODULE BagOfWordsTest
#include <boost/test/unit_test.hpp>

#include "linguisticProcessing/common/BagOfWords/bowToken.h"

using namespace Lima;
using namespace Lima::Common::BagOfWords;

BOOST_AUTO_TEST_CASE( BagOfWordsTest0_1 )
{
  BoWToken bt1(QString::fromUtf8("lemma"), 1, 10, 5);
  BOOST_REQUIRE( bt1.getLemma() == QString::fromUtf8("lemma") );
}

BOOST_AUTO_TEST_CASE( BagOfWordsTest0_2 )
{
  BoWToken bt1(QString::fromUtf8("démocratique"), 1, 10, 12);
  BOOST_REQUIRE( bt1.getLemma() == QString::fromUtf8("démocratique") );
}
