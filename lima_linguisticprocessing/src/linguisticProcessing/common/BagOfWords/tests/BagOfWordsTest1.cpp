// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

