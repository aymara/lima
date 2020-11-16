/*
    Copyright 2002-2020 CEA LIST

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
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"

#include <sstream>

using namespace Lima;
using namespace Lima::Common::BagOfWords;

// Binary reading and writing

BOOST_AUTO_TEST_CASE( BagOfWordsTest1_1 )
{
  boost::shared_ptr< BoWToken > bt1(new BoWToken( QString::fromUtf8("lemma"), Lima::LinguisticCode::fromUInt(1), 10, 5));
  BoWBinaryWriter writer;
  std::stringstream stream;
  writer.writeBoWToken(stream, bt1);

  BoWBinaryReader reader;
  boost::shared_ptr< BoWToken > bt2 = boost::dynamic_pointer_cast<BoWToken>(reader.readBoWToken(stream));

  BOOST_REQUIRE( bt1->getLemma() == bt2->getLemma() );
}

BOOST_AUTO_TEST_CASE( BagOfWordsTest1_2 )
{
  boost::shared_ptr< BoWToken > bt1(new BoWToken(QString::fromUtf8("démocratique"), Lima::LinguisticCode::fromUInt(1), 10, 12));
  BoWBinaryWriter writer;
  std::stringstream stream;
  writer.writeBoWToken(stream, bt1);

  BoWBinaryReader reader;
  boost::shared_ptr< BoWToken > bt2 = boost::dynamic_pointer_cast<BoWToken>(reader.readBoWToken(stream));

  BOOST_REQUIRE( bt1->getLemma() == bt2->getLemma() );
}

BOOST_AUTO_TEST_CASE( BagOfWordsTest1_3 )
{
  boost::shared_ptr< BoWToken > bt1(new BoWToken(QString::fromUtf8("lemma"), Lima::LinguisticCode::fromUInt(1), 10, 5));
  bt1->setInflectedForm(QString::fromUtf8("lemma"));
  boost::shared_ptr< BoWToken > bt2(new BoWToken(QString::fromUtf8("démocratique"), Lima::LinguisticCode::fromUInt(1), 10, 12));
  bt2->setInflectedForm(QString::fromUtf8("démocratique"));
  boost::shared_ptr< BoWToken > bt3(new BoWToken(QString::fromUtf8("word"), Lima::LinguisticCode::fromUInt(1), 10, 4));
  bt3->setInflectedForm(QString::fromUtf8("word"));
  BoWBinaryWriter writer;
  std::stringstream stream;
  writer.writeBoWToken(stream, bt1);
  writer.writeBoWToken(stream, bt2);
  writer.writeBoWToken(stream, bt3);

  BoWBinaryReader reader;
  boost::shared_ptr< BoWToken > rbt1 = boost::dynamic_pointer_cast<BoWToken>(reader.readBoWToken(stream));
  boost::shared_ptr< BoWToken > rbt2 = boost::dynamic_pointer_cast<BoWToken>(reader.readBoWToken(stream));
  boost::shared_ptr< BoWToken > rbt3 = boost::dynamic_pointer_cast<BoWToken>(reader.readBoWToken(stream));

  BOOST_REQUIRE( bt2->getLemma() == rbt2->getLemma() );
}

