/*
    Copyright 2002-2021 CEA LIST

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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LinguisticCodeTest
#include <boost/test/unit_test.hpp>

#ifdef WIN32
#include <cstdlib>
#define USE_OLD_RAND
#else
#include <experimental/random>
#endif

#include "../../LimaCommon.h"

using namespace Lima;

template<class T>
T rnd()
{
#ifdef USE_OLD_RAND
  return std::rand();
#else
  return std::experimental::randint(T(0), std::numeric_limits<T>::max());
#endif
}

void rnd_seed()
{
#ifdef USE_OLD_RAND
  std::srand(0);
#else
  std::experimental::reseed(0);
#endif
}

template<class T>
bool test_serialization_with_int(size_t iterations)
{
  size_t i = 0;
  while (i < iterations)
  {
    T r = rnd<uint64_t>();
    std::stringstream s;
    s << r;
    LinguisticCode lc = LinguisticCode::fromDecString(s.str());
    std::string str = lc.toString();
    if (s.str() != str)
    {
      return false;
    }
    i++;
  }
  return true;
}

template<class T>
bool test_lshift_inner(T val)
{
  std::stringstream val_stream;
  val_stream << val;
  for (size_t a = 0; a < sizeof(T) * 8; a++)
  {
    if ((std::numeric_limits<T>::max() >> a) < val)
    {
      break;
    }
    T x = val;
    x <<= a;
    std::stringstream s;
    s << x;
    LinguisticCode lc = LinguisticCode::fromDecString(val_stream.str());
    lc <<= a;
    std::string str = lc.toString();
    if (s.str() != str)
    {
      return false;
    }
  }
  return true;
}

template<class T>
bool test_lshift_rnd(size_t iterations)
{
  rnd_seed();
  size_t i = 0;
  while (i < iterations)
  {
    T r = rnd<uint64_t>();
    if (!test_lshift_inner(r))
    {
      return false;
    }
    i++;
  }
  return true;
}

template<class T>
bool test_lshift_one()
{
  return test_lshift_inner(1);
}

template<class T>
bool test_rshift_inner(T val)
{
  std::stringstream val_stream;
  val_stream << val;
  for (size_t a = 0; a < sizeof(T) * 8; a++)
  {
    T x = val;
    x >>= a;
    std::stringstream s;
    s << x;
    LinguisticCode lc = LinguisticCode::fromDecString(val_stream.str());
    lc >>= a;
    std::string str = lc.toString();
    if (s.str() != str)
    {
      return false;
    }
    if (x == 0)
    {
      break;
    }
  }
  return true;
}

template<class T>
bool test_rshift_rnd(size_t iterations)
{
  rnd_seed();
  size_t i = 0;
  while (i < iterations)
  {
    T r = rnd<uint64_t>();
    if (!test_rshift_inner(r))
    {
      return false;
    }
    i++;
  }
  return true;
}

template<class T>
bool test_rshift_one()
{
  return test_rshift_inner(T(1) << (sizeof(T) * 8 - 1));
}

LinguisticCode rand_lc()
{
  LinguisticCode lc;
  for (size_t i = 0; i < LinguisticCode::size() / ( sizeof(uint64_t) * 8 ); i++)
  {
    lc <<= sizeof(uint64_t) * 8;
    LinguisticCode r = LinguisticCode::fromUInt(rnd<uint64_t>());
    lc |= r;
  }
  return lc;
}

bool test_string_serialization(size_t iterations)
{
  rnd_seed();
  size_t i = 0;
  while (i < iterations)
  {
    LinguisticCode lc = rand_lc();
    std::stringstream s;
    s << lc;
    LinguisticCode lc2;
    s >> lc2;
    if (lc != lc2)
    {
      return false;
    }
    i++;
  }
  return true;
}

bool test_binary_serialization(size_t iterations)
{
  rnd_seed();
  size_t i = 0;
  while (i < iterations)
  {
    LinguisticCode lc = rand_lc();
    std::stringstream s;
    LinguisticCode::encodeToBinary(s, lc);
    std::string str = s.str();
    LinguisticCode lc2 = LinguisticCode::decodeFromBinary(s);
    if (lc != lc2)
    {
      return false;
    }
    unsigned char *p = new unsigned char[str.size() + 1];
    memcpy(p, str.c_str(), str.size() + 1);
    unsigned char *p_before = p;
    LinguisticCode lc3 = LinguisticCode::decodeFromBinary(p);
    delete[] p_before;
    if (lc != lc3)
    {
      return false;
    }
    i++;
  }
  return true;
}

BOOST_AUTO_TEST_CASE( LinguisticCodeTest_serialization_with_int )
{
  BOOST_REQUIRE( test_serialization_with_int<uint64_t>(1000) );
}

BOOST_AUTO_TEST_CASE( LinguisticCodeTest_lshift_rnd )
{
  BOOST_REQUIRE( test_lshift_rnd<uint64_t>(1000) );
}

BOOST_AUTO_TEST_CASE( LinguisticCodeTest_rshift_rnd )
{
  BOOST_REQUIRE( test_rshift_rnd<uint64_t>(1000) );
}

BOOST_AUTO_TEST_CASE( LinguisticCodeTest_lshift_one )
{
  BOOST_REQUIRE( test_lshift_one<uint64_t>() );
}

BOOST_AUTO_TEST_CASE( LinguisticCodeTest_rshift_one )
{
  BOOST_REQUIRE( test_rshift_one<uint64_t>() );
}

BOOST_AUTO_TEST_CASE( LinguisticCodeTest_string_serialization )
{
  BOOST_REQUIRE( test_string_serialization(1000) );
}

BOOST_AUTO_TEST_CASE( LinguisticCodeTest_binary_serialization )
{
  BOOST_REQUIRE( test_binary_serialization(1000) );
}
