// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BagOfWordsTest
#include <boost/test/unit_test.hpp>

#include "linguisticProcessing/common/BagOfWords/bowToken.h"

using namespace Lima;
using namespace Lima::Common::BagOfWords;

BOOST_AUTO_TEST_CASE( BagOfWordsTest0_1 )
{
  BoWToken bt1(QString::fromUtf8("lemma"), LinguisticCode::fromUInt(1), 10, 5);
  BOOST_REQUIRE( bt1.getLemma() == QString::fromUtf8("lemma") );
}

BOOST_AUTO_TEST_CASE( BagOfWordsTest0_2 )
{
  BoWToken bt1(QString::fromUtf8("démocratique"), LinguisticCode::fromUInt(1), 10, 12);
  BOOST_REQUIRE( bt1.getLemma() == QString::fromUtf8("démocratique") );
}
