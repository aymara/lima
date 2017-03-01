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
/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/

#include "linguisticProcessing/common/tgv/TestCasesHandler.h"
#include "linguisticProcessing/common/tgv/TestCaseError.hpp"
#include "common/Data/strwstrtools.h"

#include <iostream>

namespace Lima
{
namespace Common
{
namespace TGV
{

char const * const FATAL_ERROR_TYPE = "bloquant";
int const EXIT_FATAL_ERROR = 64;

TestCasesHandler::TestCasesHandler( TestCaseProcessor& processor)
  : m_reportByType(), m_processor(processor), m_hasFatalError(false) {
}

TestCasesHandler::~TestCasesHandler() {
}

// -----------------------------------------------------------------------
//  Implementations of the SAX DocumentHandler interface
// -----------------------------------------------------------------------

bool TestCasesHandler::startDocument()
{
  currentTestCase=TestCase();
  m_inExpl=false;
  m_inText=false;
  m_inParam=false;
  m_inList=false;
  return true;
}

bool TestCasesHandler::startElement (
  const QString & namespaceURI, const QString & localname, const QString & qname, const QXmlAttributes & attrs)
{
  LIMA_UNUSED(namespaceURI);
  TGVLOGINIT;
  std::string name=getName(localname,qname);
  
  if (name=="testcase")
  {
    LDEBUG << "TestCasesHandler::startElement: new testcase ";
    currentTestCase=TestCase();
    currentTestCase.id=attributeValue("id", attrs);
    std::string typech=attributeValue("type", attrs);
    currentTestCase.type=(typech.empty()?"undefined":typech);
    LDEBUG << "TestCasesHandler::startElement: id=" << currentTestCase.id
           << ", type=" << currentTestCase.type;
  } else if (name=="text") {
    // For compatibility with old testTva format
    LDEBUG << "TestCasesHandler::startElement: start text ";
    m_inText=true;
  } else if (name=="expl") {
    LDEBUG << "TestCasesHandler::startElement: start expl ";
    m_inExpl=true;
  } else if (name=="call-parameters") {
    m_inParam=true;
    LDEBUG << "TestCasesHandler::startElement: start call-parameters ";
  } else if (name=="list") {
    LDEBUG << "TestCasesHandler::startElement: start list ";
    m_listKey = attributeValue("key", attrs);
    MultiValCallParams::iterator pos = currentTestCase.multiValCallParams.find(m_listKey);
    if( pos == currentTestCase.multiValCallParams.end() ) {
      // typedef std::map<std::string,std::list<std::string> > MultiValCallParams;
      std::pair<std::string,std::list<std::string> > newElem(m_listKey,std::list<std::string>() );
//      std::pair<MultiValCallParams::iterator,bool> ret = 
//        currentTestCase.multiValCallParams.insert(
//          std::pair<std::string,std::list<std::string> >(m_listKey,std::list<std::string>) );
      /*std::pair<MultiValCallParams::iterator,bool> ret =*/ currentTestCase.multiValCallParams.insert(newElem);
    }
    m_inList=true;
  } else if (name=="map") {
    LDEBUG << "TestCasesHandler::startElement: start map ";
    m_mapKey = attributeValue("key",attrs);
    MapValCallParams::iterator pos = currentTestCase.mapValCallParams.find(m_mapKey);
    if( pos == currentTestCase.mapValCallParams.end() ) {
      // typedef std::map<std::string,std::list<std::string> > MultiValCallParams;
      std::pair<std::string,std::map<std::string,std::string> > newElem(m_mapKey,std::map<std::string,std::string>() );
//      std::pair<MultiValCallParams::iterator,bool> ret = 
//        currentTestCase.multiValCallParams.insert(
//          std::pair<std::string,std::list<std::string> >(m_listKey,std::list<std::string>) );
      /*std::pair<MapValCallParams::iterator,bool> ret =*/ currentTestCase.mapValCallParams.insert(newElem);
    }
    m_inMap=true;
  } else if (name=="item") {
    LDEBUG << "TestCasesHandler::startElement: start item ";
    if( m_inList == true ) {
      MultiValCallParams::iterator pos = currentTestCase.multiValCallParams.find(m_listKey);
      if( pos != currentTestCase.multiValCallParams.end() ) {
        std::string val = attributeValue("value", attrs);
        (*pos).second.push_back(val);
      }
      else {
        LERROR << "!! TestCasesHandler::startElement: no list for param " << m_listKey;
      }
    }
    else if (m_inMap == true) {
      MapValCallParams::iterator pos = currentTestCase.mapValCallParams.find(m_mapKey);
      if( pos != currentTestCase.mapValCallParams.end() ) {
        std::string attr = attributeValue("key", attrs);
        std::string val = attributeValue("value",attrs);
        (*pos).second.insert(make_pair(attr,val));
      }
      else {
        LERROR << "!! TestCasesHandler::startElement: no list for param " << m_listKey;
      }
    }
//    currentTestCase.multiValCallParams.insert(std::pair<std::string, std::string>(key,val) );
  } else if (name=="param") {
    LDEBUG << "TestCasesHandler::startElement: start param ";
    std::string key = attributeValue("key",attrs);
    std::string val = attributeValue("value", attrs);
    currentTestCase.simpleValCallParams.insert(std::pair<std::string, std::string>(key,val) );
    // TODO: read multiValCallParams "pipelines"
    // std::string pipch=attributeValue("pipelines");
  } else if (name=="test") {
    // build a testUnit
    //cout << "add test" << std::endl;
    TestCase::TestUnit tu;
    tu.id=attributeValue("id", attrs);
    tu.trace=attributeValue("trace", attrs);
    tu.comment=attributeValue("comment", attrs);
    tu.left=attributeValue("left", attrs);
    tu.op=attributeValue("operator", attrs);
    tu.right=attributeValue("right",attrs);
    tu.conditional=(attributeValue("conditional",attrs)=="yes");
    currentTestCase.tests.push_back(tu);
    LDEBUG << "TestCasesHandler::startElement: push testUnit"
           << ", id =" << tu.id
           << ", trace =" << tu.trace
           << ", comment =" << tu.comment
           << ", left =" << tu.left
           << ", op =" << tu.op
           << ", right =" << tu.right
           << ", conditional =" << tu.conditional;
           
  }
  return true;
}

bool TestCasesHandler::characters(const QString& chars)
{
  if (m_inExpl) {
    currentTestCase.explanation.append(chars.toUtf8().data());
  } else if (m_inText) {
    // For compatibility with old testTva format
    std::string newTextVal(chars.toUtf8().data());

    SimpleValCallParams::iterator pos = currentTestCase.simpleValCallParams.find("text");
    if( pos == currentTestCase.simpleValCallParams.end() ) {
//       std::pair<SimpleValCallParams::iterator,bool> ret = 
        currentTestCase.simpleValCallParams.insert(std::pair<std::string, std::string>("text",newTextVal) );
    }
    else {
      std::string& textVal = (*pos).second;
      textVal.append(chars.toUtf8().data());
    }
  }
  return true;
  
}

bool TestCasesHandler::endElement (const QString & namespaceURI, const QString & localname, const QString & qname)
{
  LIMA_UNUSED(namespaceURI);
  TGVLOGINIT;
  std::string name=getName(localname,qname);
  if (name=="testcase")
  {
    m_reportByType[currentTestCase.type].nbtests++;
      LDEBUG << "TestCasesHandler::endElement: call processTestCase(" << currentTestCase.id
                                                             << "," << currentTestCase.type << ")";
      TestCaseError e = m_processor.processTestCase(currentTestCase);
      if (e())
      {
        std::cout << currentTestCase.id << " (" << currentTestCase.type << ") got error (type: '"<<e()<<"'): " << std::endl << e.what() << std::endl;
         if (currentTestCase.type == FATAL_ERROR_TYPE) {
          m_hasFatalError = true;
        }
        if (e() == TestCaseError::TestCaseFailed)
        {
          if (e.isConditional())
          {
            m_reportByType[currentTestCase.type].conditional++;
          } else
          {
            m_reportByType[currentTestCase.type].failed++;
          }
        }
        else
        {
          std::cout << "runtime error: " << e.what() << std::endl;
          m_reportByType[currentTestCase.type].failed++;
          throw std::runtime_error(e.what());
        }
      }
      else
      {
        std::cout << currentTestCase.id << " (" << currentTestCase.type << ") passed successfully." << std::endl;;
        m_reportByType[currentTestCase.type].success++;
      }
  } else if (name=="text") {
    // For compatibility with old testTva format
    m_inText=false;
  } else if (name=="expl") {
    m_inExpl=false;
  } else if (name=="call-parameters") {
    m_inParam=false;
  } else if (name=="list") {
    m_inList=false;
  } else if (name=="map") {
    m_inMap=false;
  }
  return true;
}

std::string TestCasesHandler::getName(const QString& localName,
                                       const QString& qName) {
  
  return Common::Misc::limastring2utf8stdstring( (localName.isEmpty())?qName:localName );
}

std::string TestCasesHandler::attributeValue(const QString& attr, const QXmlAttributes& attrs) const
{
  return Common::Misc::limastring2utf8stdstring( attrs.value(attr) );
}

int exitCode(TestCasesHandler const & tch)
{
  return tch.hasFatalError() ? EXIT_FATAL_ERROR : EXIT_SUCCESS;
}

} // TGV
} // Common
} // Lima
