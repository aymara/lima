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

char const * const FATAL_ERROR_TYPE = "blocking";
int const EXIT_FATAL_ERROR = 64;

class TestCasesHandlerPrivate
{
  friend class TestCasesHandler;
  TestCasesHandlerPrivate() :
    m_testcasesId(""),
    m_traceName("")
  {
  }
  ~TestCasesHandlerPrivate() {}

  QString m_testcasesId;
  QString m_traceName;
};

TestCasesHandler::TestCasesHandler( TestCaseProcessor& processor) :
  m_reportByType(),
  m_processor(processor),
  m_hasFatalError(false),
  m_inText(false),
  m_inExpl(false),
  m_inParam(false),
  m_inList(false),
  m_inMap(false),
  m_d(new TestCasesHandlerPrivate())
{
}

TestCasesHandler::~TestCasesHandler()
{
  delete m_d;
}

// -----------------------------------------------------------------------
//  Implementations of the SAX DocumentHandler interface
// -----------------------------------------------------------------------

bool TestCasesHandler::startDocument()
{
  TGVLOGINIT;
  LDEBUG << "TestCasesHandler::startDocument";
  currentTestCase=TestCase();
  m_d->m_testcasesId = "";
  m_d->m_traceName = "";
  m_inExpl=false;
  m_inText=false;
  m_inParam=false;
  m_inList=false;
  return true;
}

bool TestCasesHandler::startElement (
  const QStringRef & namespaceURI,
  const QStringRef & localname,
  const QStringRef & qname,
  const QXmlStreamAttributes & attrs,
  int lineNumber,
  int columnNumber )
{
  LIMA_UNUSED(namespaceURI);
  TGVLOGINIT;
  LDEBUG << "TestCasesHandler::startElement" << namespaceURI << localname
          << qname << lineNumber;

  QStringRef name = getName(localname,qname);

  if (name == "testcases")
  {
    m_d->m_testcasesId = attributeValue("id", attrs).toUtf8().constData();
    m_d->m_traceName = attributeValue("trace", attrs).toUtf8().constData();
  }
  if (name == "testcase")
  {
    LDEBUG << "TestCasesHandler::startElement: new testcase ";
    currentTestCase=TestCase();
    currentTestCase.id = attributeValue("id", attrs).toUtf8().constData();
    if (currentTestCase.id.isEmpty())
    {
      currentTestCase.id = QString("testcase_%1")
          .arg(lineNumber).toUtf8().constData();
    }
    if (!m_d->m_testcasesId.isEmpty())
    {
      currentTestCase.id = m_d->m_testcasesId + "-" + currentTestCase.id;
    }
    auto typech = attributeValue("type", attrs);
    currentTestCase.type=(typech.isEmpty()?"undefined":typech.toUtf8().constData());
    LDEBUG << "TestCasesHandler::startElement: id=" << currentTestCase.id
           << ", type=" << currentTestCase.type;
  }
  else if (name=="text")
  {
    // For compatibility with old testTva format
    LDEBUG << "TestCasesHandler::startElement: start text ";
    m_inText=true;
  }
  else if (name=="expl")
  {
    LDEBUG << "TestCasesHandler::startElement: start expl ";
    m_inExpl=true;
  }
  else if (name=="call-parameters")
  {
    m_inParam=true;
    LDEBUG << "TestCasesHandler::startElement: start call-parameters ";
  }
  else if (name=="list")
  {
    LDEBUG << "TestCasesHandler::startElement: start list ";
    m_listKey = attributeValue("key", attrs).toUtf8().constData();
    MultiValCallParams::iterator pos = currentTestCase.multiValCallParams.find(m_listKey);
    if( pos == currentTestCase.multiValCallParams.end() )
    {
      // typedef std::map<std::string,std::list<std::string> > MultiValCallParams;
      std::pair<std::string,std::list<std::string> > newElem(m_listKey,std::list<std::string>() );
//      std::pair<MultiValCallParams::iterator,bool> ret =
//        currentTestCase.multiValCallParams.insert(
//          std::pair<std::string,std::list<std::string> >(m_listKey,std::list<std::string>) );
      /*std::pair<MultiValCallParams::iterator,bool> ret =*/ currentTestCase.multiValCallParams.insert(newElem);
    }
    m_inList=true;
  }
  else if (name=="map")
  {
    LDEBUG << "TestCasesHandler::startElement: start map ";
    m_mapKey = attributeValue("key",attrs).toUtf8().constData();
    MapValCallParams::iterator pos = currentTestCase.mapValCallParams.find(m_mapKey);
    if( pos == currentTestCase.mapValCallParams.end() )
    {
      // typedef std::map<std::string,std::list<std::string> > MultiValCallParams;
      std::pair<std::string,std::map<std::string,std::string> > newElem(m_mapKey,std::map<std::string,std::string>() );
//      std::pair<MultiValCallParams::iterator,bool> ret =
//        currentTestCase.multiValCallParams.insert(
//          std::pair<std::string,std::list<std::string> >(m_listKey,std::list<std::string>) );
      /*std::pair<MapValCallParams::iterator,bool> ret =*/ currentTestCase.mapValCallParams.insert(newElem);
    }
    m_inMap=true;
  }
  else if (name=="item")
  {
    LDEBUG << "TestCasesHandler::startElement: start item ";
    if( m_inList == true )
    {
      MultiValCallParams::iterator pos = currentTestCase.multiValCallParams.find(m_listKey);
      if( pos != currentTestCase.multiValCallParams.end() )
      {
        std::string val = attributeValue("value", attrs).toUtf8().constData();
        (*pos).second.push_back(val);
      }
      else
      {
        LERROR << "!! TestCasesHandler::startElement: no list for param " << m_listKey;
      }
    }
    else if (m_inMap == true)
    {
      MapValCallParams::iterator pos = currentTestCase.mapValCallParams.find(m_mapKey);
      if( pos != currentTestCase.mapValCallParams.end() )
      {
        std::string attr = attributeValue("key", attrs).toUtf8().constData();
        std::string val = attributeValue("value",attrs).toUtf8().constData();
        (*pos).second.insert(make_pair(attr,val));
      }
      else
      {
        LERROR << "!! TestCasesHandler::startElement: no list for param " << m_listKey;
      }
    }
//    currentTestCase.multiValCallParams.insert(std::pair<std::string, std::string>(key,val) );
  }
  else if (name=="param")
  {
    LDEBUG << "TestCasesHandler::startElement: start param ";
    std::string key = attributeValue("key",attrs).toUtf8().constData();
    std::string val = attributeValue("value", attrs).toUtf8().constData();
    currentTestCase.simpleValCallParams.insert(std::pair<std::string, std::string>(key,val) );
    // TODO: read multiValCallParams "pipelines"
    // std::string pipch=attributeValue("pipelines");
  }
  else if (name=="test")
  {
    // build a testUnit
    //cout << "add test" << std::endl;
    TestCase::TestUnit tu;
    tu.id=attributeValue("id", attrs).toString();
    if (tu.id.isEmpty())
    {
      tu.id = QString("test_%1_%2")
          .arg(lineNumber).arg(columnNumber).toUtf8().constData();
    }
    tu.trace=attributeValue("trace", attrs).toUtf8().constData();
    if (tu.trace.empty())
    {
      if (m_d->m_traceName.isEmpty())
      {
        TGVLOGINIT;
        LERROR << "Missing attribute trace for test at line" << lineNumber;
        return false;
      }
      tu.trace = m_d->m_traceName.toUtf8().constData();
    }
    tu.comment=attributeValue("comment", attrs).toUtf8().constData();
    tu.left=attributeValue("left", attrs).toUtf8().constData();
    if (tu.left.empty())
    {
      TGVLOGINIT;
      LERROR << "Missing attribute left for test at line" << lineNumber;
      return false;
    }
    tu.op=attributeValue("operator", attrs).toUtf8().constData();
    if (tu.op.empty())
    {
      TGVLOGINIT;
      LERROR << "Missing attribute op for test at line" << lineNumber;
      return false;
    }
    tu.right=attributeValue("right",attrs).toUtf8().constData();
    if (tu.right.empty())
    {
      TGVLOGINIT;
      LERROR << "Missing attribute right for test at line" << lineNumber;
      return false;
    }
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

bool TestCasesHandler::characters(const QStringRef& chars)
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

bool TestCasesHandler::endElement (const QStringRef & namespaceURI,
                                   const QStringRef & localname,
                                   const QStringRef & qname)
{
  LIMA_UNUSED(namespaceURI);
  TGVLOGINIT;
  QStringRef name = getName(localname,qname);
  if (name=="testcase")
  {
    m_reportByType[currentTestCase.type].nbtests++;
    LDEBUG << "TestCasesHandler::endElement: call processTestCase(" << currentTestCase.id
                                                            << "," << currentTestCase.type << ")";
    TestCaseError e = m_processor.processTestCase(currentTestCase);
    if (e())
    {
      std::cout << currentTestCase.id.toUtf8().constData()
                << " (" << currentTestCase.type << ") got error (type: '"
                << e() << "'): " << std::endl << e.what() << std::endl;

      if (currentTestCase.type == FATAL_ERROR_TYPE) {
        m_hasFatalError = true;
      }
      if (e() == TestCaseError::TestCaseFailed)
      {
        std::cout << currentTestCase.id.toUtf8().constData()
                  << " (" << currentTestCase.type << ") got error (type: '"
                  << e() << "'): " << std::endl << e.what() << std::endl;
         if (currentTestCase.type == FATAL_ERROR_TYPE) {
          m_hasFatalError = true;
        }
        if (e() == TestCaseError::TestCaseFailed)
        {
          m_reportByType[currentTestCase.type].conditional++;
        } else
        {
          m_reportByType[currentTestCase.type].failed++;
        }
      }
      else
      {
        std::cout << currentTestCase.id.toUtf8().constData()
                  << " (" << currentTestCase.type << ") passed successfully."
                  << std::endl;;
        m_reportByType[currentTestCase.type].success++;
      }
    }
    else
    {
      std::cout << currentTestCase.id.toUtf8().constData()
                << " (" << currentTestCase.type << ") passed successfully."
                << std::endl;;
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

QStringRef TestCasesHandler::getName(const QStringRef& localName,
                                       const QStringRef& qName) {

  return (localName.isEmpty())?qName:localName;
}

QStringRef TestCasesHandler::attributeValue(
    const QString& attr,
    const QXmlStreamAttributes& attrs) const
{
  return attrs.value(attr);
}

int exitCode(TestCasesHandler const & tch)
{
  return tch.hasFatalError() ? EXIT_FATAL_ERROR : EXIT_SUCCESS;
}

} // TGV
} // Common
} // Lima
