// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/
#include "linguisticProcessing/common/tgv/TestCase.h"

#include "common/Data/strwstrtools.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>


using namespace std;

namespace Lima
{
namespace Common
{
namespace TGV
{
std::ostream & operator << (std::ostream &os, const Lima::Common::TGV::TestCase& tc) {
  tc.print(os);
  return os;
}

void TestCase::print(std::ostream &os) const {
  os << "<testcase id='" << id  << "' type='" << type  << "'>\n";
  os << "<expl>"  << explanation  << "</expl>\n";
  os << "<call-parameters>\n";
  for( SimpleValCallParams::const_iterator paramIt = simpleValCallParams.begin() ;
     paramIt != simpleValCallParams.end() ; paramIt++ ) {
    os << "<param key='" << (*paramIt).first  << "' value='" << (*paramIt).first << "'>\n";
  }
  for( MultiValCallParams::const_iterator paramIt = multiValCallParams.begin() ;
     paramIt != multiValCallParams.end() ; paramIt++ ) {
    const std::list<std::string>& list = (*paramIt).second;
    os << "<list key='" << (*paramIt).first  << "'>\n";
    for( std::list<std::string>::const_iterator valueIt = list.begin() ;
      valueIt != list.end() ; valueIt++ ) {
      os << "<item value='" << (*valueIt) << "'/>\n";
    }
    os << "</list>\n";
  }
  for( MapValCallParams::const_iterator paramIt = mapValCallParams.begin() ;
     paramIt != mapValCallParams.end() ; paramIt++ ) {
    const std::map<std::string,std::string>& map = (*paramIt).second;
    os << "<map key='" << (*paramIt).first  << "'>\n";
    for( std::map<std::string,std::string>::const_iterator valueIt = map.begin() ;
      valueIt != map.end() ; valueIt++ ) {
      os << "<item attr='" << (*valueIt).first << "' val='" << (*valueIt).second << "'/>\n";
    }
    os << "</list>\n";
  }
  os << "</call-parameters>\n";
  for(std::list<TestCase::TestUnit>::const_iterator testUnitIt = tests.begin() ;
    testUnitIt != tests.end() ; testUnitIt++ ) {
    testUnitIt->print(os);
  }
  os << "</testcase>\n";
}

std::string TestCase::getParam( const std::string& key ) const
{

  SimpleValCallParams::const_iterator paramIt = simpleValCallParams.find(key);
  if( paramIt == simpleValCallParams.end() ) {
    TGVLOGINIT;
    // not ERROR: parameter may be optional
    LDEBUG << "TestCaseProcessor::getParam: no '"
           << key << "' attribute in call parameter of testCase "
           << id;
    return "";
  }
  string paramVal = (*paramIt).second;
  return paramVal;

}

void TestCase::getList( const std::string& key, std::list<std::string >& list ) const
{

  MultiValCallParams::const_iterator paramIt = multiValCallParams.find(key);
  if( paramIt == multiValCallParams.end() ) {
    TGVLOGINIT;
    LWARN << "TestCaseProcessor::getList: no '"
                << key << "' attribute in call parameter of testCase "
                << id;
    return;
  }
  list = (*paramIt).second;
}

void TestCase::getMap( const std::string& key, std::map<std::string,std::string >& map ) const
{

  MapValCallParams::const_iterator paramIt = mapValCallParams.find(key);
  if( paramIt == mapValCallParams.end() ) {
    TGVLOGINIT;
    LWARN << "TestCaseProcessor::getMap: no '"
                << key << "' attribute in call parameter of testCase "
                << id;
    return;
  }
  map = (*paramIt).second;
}

std::ostream & operator << (std::ostream &os, const Lima::Common::TGV::TestCase::TestUnit& tcu) {
  tcu.print(os);
  return os;
}

void TestCase::TestUnit::print(std::ostream &os) const {
  os << "<test id='" << id  << "' trace='" << trace << "'\n"
     << "comment='" << comment  << "'\n"
     << "left='" << left << "'\n"
     << "operator='" << op << "'\n"
     << "right='" << right << "'\n"
     << "conditional='" << (conditional?"true":"false") << "'/>\n";
}

std::ostream& operator<<(std::ostream& out,const std::set<std::string>& s)
{
  out << "{";
  copy(s.begin(),s.end(),ostream_iterator<std::string>(out,","));
  out << "}";
  return out;
}

} // TGV
} // Common
} // Lima
