/*
    Copyright 2002-2022 CEA LIST

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
/**
 * @author Benoit Mathieu <benoit.mathieu@cea.fr>                                 *
 * @date begin 2004
 **/
#include "linguisticProcessing/common/tgv/TestCaseProcessor.hpp"
#include "linguisticProcessing/common/tgv/TestCase.h"
#include "linguisticProcessing/common/tgv/TestCaseError.hpp"
#include "linguisticProcessing/common/tgv/pugixml.hpp"

#include "common/Data/strwstrtools.h"

#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>

#include <QtCore/QStringList>
#include <QtCore/QFile>

std::ostream& operator<<(std::ostream& oss, const QStringList& qsl)
{
  oss << "{";
  Q_FOREACH(QString s, qsl)
  {
    oss << s.toUtf8().data() << ",";
  }
  oss << "}";
  return oss;
}

namespace Lima
{
namespace Common
{
namespace TGV
{

class TestCaseProcessorPrivate
{
  friend class TestCaseProcessor;
public:

  TestCaseProcessorPrivate( const std::string workingDirectory );
  ~TestCaseProcessorPrivate() = default;

  QString m_workingDirectory;

  static QStringList evaluateExpression(
    const std::string& expr,
    pugi::xml_node& node);
//
  static bool existsExpression(
    const std::string& expr,
    pugi::xml_node& node);

};

TestCaseProcessorPrivate::TestCaseProcessorPrivate(const std::string workingDirectory) :
    m_workingDirectory(QString::fromStdString(workingDirectory))
{
}

TestCaseProcessor::TestCaseProcessor(const std::string workingDirectory) :
    m_d(new TestCaseProcessorPrivate(workingDirectory))
{
}

TestCaseError TestCaseProcessor::evalTestCase(
  const TestCase& testCase, const std::string& pipeName,
  const std::string& textFile, const std::string& traceFilePrefix ) const
{
  LIMA_UNUSED(textFile);
  TGVLOGINIT;
  // Valid TestUnits
  for (const auto& test: testCase.tests)
  {
    std::string traceFile(traceFilePrefix+test.trace);

    /* Load XML document */
    pugi::xml_document doc;

    LTRACE << "TestCaseProcessor::evalTestCase load trace file" << traceFile;
    pugi::xml_parse_result result = doc.load_file(traceFile.c_str());

    if (!result)
    {
      LERROR << "Error: Unable to parse output file " << traceFile << ":" << result.description();
      return TestCaseError(testCase, TestCaseError::TestCaseFailed,
                           "Error: Unable to parse output file  !", pipeName, test);
    }

    // OK, let's evaluate the expression...
    // Check unary operators before evaluate se right member
    LTRACE << "TestCaseProcessor::evalTestCase test:" << test.left << test.op << test.right;
    if (test.op=="notexists")
    {
      if (m_d->existsExpression(test.left, doc))
      {
        return TestCaseError(testCase, TestCaseError::TestCaseFailed,"an element exists !", pipeName, test);
      }
    }
    else if (test.op=="exists")
    {
      if (!m_d->existsExpression(test.left, doc))
      {
        std::ostringstream oss;
        oss << "element doesn't exist ! : " << test.left;
        return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, test);
      }
    }
    else
    {

      // operator is a binary one. evaluate the second expression
      QStringList left = m_d->evaluateExpression(test.left, doc);
      left.removeDuplicates();
      QStringList right = m_d->evaluateExpression(test.right, doc);
      right.removeDuplicates();

      QSet<QString> sleft;

      Q_FOREACH (QString element, left)
      {
        sleft.insert(element);
      }
      QSet<QString> sright;
      Q_FOREACH (QString element, right)
      {
        sright.insert(element);
      }

      if (test.op=="=")
      {
        if (sleft!=sright)
        {
          std::ostringstream oss;
          oss << "equality check failed : " << left << " != " << right << std::endl;
          oss << "                        left : " << test.left << std::endl;
          oss << "                        right: " << test.right;

          return TestCaseError(testCase, TestCaseError::TestCaseFailed, oss.str(), pipeName, test);
        }
        else if (left.size()==0)
        {
          std::ostringstream oss;
          oss << "left and right member are empty !";
          return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, test);
        }
      }
      else if (test.op=="!=")
      {
        if (sleft==sright)
        {
          std::ostringstream oss;
          oss << "inequality check failed : " << left << " == " << right << std::endl;
          oss << "                          left : " << test.left << std::endl;
          oss << "                          right: " << test.right;
          return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, test);
        }
      }
      else if (test.op=="contains")
      {
        Q_FOREACH(QString s, sright)
        {
          if (!sleft.contains(s))
          {
            std::ostringstream oss;
            oss << "includes check failed : " << left << " not contains " << right << std::endl;
            oss << "                          left : " << test.left << std::endl;
            oss << "                          right: " << test.right;
            return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, test);
          }
        }
      }
      else if (test.op=="notcontains")
      {
        Q_FOREACH(QString s, sleft)
        {
          if (!sright.contains(s))
          {
            std::ostringstream oss;
            oss << "not includes check failed : " << left << " contains " << right;
            return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, test);
          }
        }
      }
      else if (test.op=="distinct")
      {
        Q_FOREACH(QString l, sleft)
        {
          Q_FOREACH(QString r, sright)
          {
            if (l==r)
            {
              std::ostringstream oss;
              oss << l.toUtf8().data() << " is a common value of the two sets that should be disctinct !" << std::endl;
              oss << "                          left : " << test.left << std::endl;
              oss << "                          right: " << test.right;
              return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, test);
            }
          }
        }
      }
      else if (test.op=="intersect")
      {
        Q_FOREACH(QString l, sleft)
        {
          Q_FOREACH(QString r, sright)
          {
            if (l==r)
            {
              return TestCaseError(testCase, TestCaseError::NoError, "", pipeName, test);
            }
          }
        }
        std::ostringstream oss;
        oss << "intersect failed : no common value between " << left << " and " << right << " !" << std::endl;
        oss << "                          left : " << test.left << std::endl;
        oss << "                          right: " << test.right;
        return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, test);
      }
      else if ((test.op!="exists") && (test.op!="notexists"))
      {
        return TestCaseError(testCase, TestCaseError::InvalidOperator, (test).id, pipeName, test);
      }
    }
  }
  return TestCaseError(testCase, "", pipeName);
}

QStringList TestCaseProcessorPrivate::evaluateExpression(
    const std::string& expr,
    pugi::xml_node& node)
{
  TGVLOGINIT;
  QStringList results;

  if (expr.substr(0,6)=="XPATH#")
  {
    /* Evaluate xpath expression */
    QString xpath = QString::fromUtf8(expr.substr(6).c_str());//+"/string()";
    LDEBUG << "TestCaseProcessor::evaluateExpression setQuery("<<xpath<<");";
    auto xnodes = node.select_nodes(xpath.toUtf8().data());
    LDEBUG << "TestCaseProcessor::evaluateExpression nb nodes selected:" << xnodes.size();
    for (const auto& node: xnodes)
    {
      if (node.attribute())
      {
        LDEBUG << "TestCaseProcessor::evaluateExpression result is an attribute with value:" << node.attribute().value();
        results.push_back(QString::fromUtf8(node.attribute().value()));
      }
      else if (node.node())
      {
        LDEBUG << "TestCaseProcessor::evaluateExpression result is a node with text:" << node.node().text().as_string();
        results.push_back(QString::fromUtf8(node.node().text().as_string()));
      }
    }
    LDEBUG << "TestCaseProcessor::evaluateExpression evaluates to " << results;
  }
  else if (expr.substr(0,4)=="SET#")
  {
    size_t start=4;
    while (start<expr.size())
    {
      size_t end=expr.find('#',start);
      if (end==std::string::npos)
      {
        end=expr.size();
      }
      std::string val=expr.substr(start,end-start);
      results.push_back(QString::fromUtf8(val.c_str()));
      start=end+1;
    }
  }
  else
  {
    results.push_back(QString::fromUtf8(expr.c_str()));
  }
  return results;
}

bool TestCaseProcessorPrivate::existsExpression(
  const std::string& expr,
  pugi::xml_node& node)
{
//   TGVLOGINIT;
  return !evaluateExpression(expr,node).empty();
}

const QString& TestCaseProcessor::workingDirectory() const
{
  return m_d->m_workingDirectory;
}

} // TGV
} // Common
} // Lima
