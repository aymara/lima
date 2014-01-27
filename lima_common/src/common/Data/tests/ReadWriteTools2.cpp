#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LimaStringTest
#include <boost/test/unit_test.hpp>

#include "common/Data/LimaString.h"
#include "common/Data/readwritetools.h"
#include "common/Data/strwstrtools.h"

#include <QtCore/QString>

using namespace Lima;

// Test UTF-8 strings read/write functions

// ASCII string

BOOST_AUTO_TEST_CASE( ReadWriteTools2_1 )
{
  QString str = QString::fromUtf8("abcd");
  std::ostringstream oss;
  Lima::Common::Misc::writeUTF8StringField(oss, str);
  
  std::string buffer = oss.str();
  QString read_str;
  std::istringstream iss(buffer);
  Lima::Common::Misc::readUTF8StringField(iss, read_str);

  BOOST_REQUIRE( str == read_str );
}


// ASCII string

BOOST_AUTO_TEST_CASE( ReadWriteTools2_2 )
{
  QString str = QString::fromUtf8("éàù");
  std::ostringstream oss;
  Lima::Common::Misc::writeUTF8StringField(oss, str);

  std::string buffer = oss.str();
  std::istringstream iss(buffer);
  QString read_str;
  Lima::Common::Misc::readUTF8StringField(iss, read_str);

  BOOST_REQUIRE( str == read_str );
}
