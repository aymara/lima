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
  BoWToken bt1(QString::fromUtf8("lemma"), 1, 10, 5);
  BoWBinaryWriter writer;
  std::stringstream stream;
  std::map<BoWToken*,uint64_t> refMap;
  writer.writeBoWToken(stream, &bt1, refMap);

  BoWBinaryReader reader;
  std::vector<AbstractBoWElement*> rrefMap;
  BoWToken* bt2 = static_cast<BoWToken*>(reader.readBoWToken(stream, rrefMap));

  BOOST_REQUIRE( bt1.getLemma() == bt2->getLemma() );
  delete bt2;
}

BOOST_AUTO_TEST_CASE( BagOfWordsTest1_2 )
{
  BoWToken bt1(QString::fromUtf8("démocratique"), 1, 10, 12);
  BoWBinaryWriter writer;
  std::stringstream stream;
  std::map<BoWToken*,uint64_t> refMap;
  writer.writeBoWToken(stream, &bt1, refMap);

  BoWBinaryReader reader;
  std::vector<AbstractBoWElement*> rrefMap;
  BoWToken* bt2 = static_cast<BoWToken*>(reader.readBoWToken(stream, rrefMap));

  BOOST_REQUIRE( bt1.getLemma() == bt2->getLemma() );
  delete bt2;
}

BOOST_AUTO_TEST_CASE( BagOfWordsTest1_3 )
{
  BoWToken bt1(QString::fromUtf8("lemma"), 1, 10, 5);
  bt1.setInflectedForm(QString::fromUtf8("lemma"));
  BoWToken bt2(QString::fromUtf8("démocratique"), 1, 10, 12);
  bt2.setInflectedForm(QString::fromUtf8("démocratique"));
  BoWToken bt3(QString::fromUtf8("word"), 1, 10, 4);
  bt3.setInflectedForm(QString::fromUtf8("word"));
  BoWBinaryWriter writer;
  std::stringstream stream;
  std::map<BoWToken*,uint64_t> refMap;
  writer.writeBoWToken(stream, &bt1, refMap);
  writer.writeBoWToken(stream, &bt2, refMap);
  writer.writeBoWToken(stream, &bt3, refMap);

  BoWBinaryReader reader;
  std::vector<AbstractBoWElement*> rrefMap;
  BoWToken* rbt1 = static_cast<BoWToken*>(reader.readBoWToken(stream, rrefMap));
  BoWToken* rbt2 = static_cast<BoWToken*>(reader.readBoWToken(stream, rrefMap));
  BoWToken* rbt3 = static_cast<BoWToken*>(reader.readBoWToken(stream, rrefMap));

  BOOST_REQUIRE( bt2.getLemma() == rbt2->getLemma() );
  delete rbt1;
  delete rbt2;
  delete rbt3;
}

