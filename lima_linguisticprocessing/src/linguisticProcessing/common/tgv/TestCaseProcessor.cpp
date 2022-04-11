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
#include "linguisticProcessing/common/tgv/TestCaseProcessor.hpp"
#include "linguisticProcessing/common/tgv/TestCase.h"
#include "linguisticProcessing/common/tgv/TestCaseError.hpp"

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

void TestCaseProcessor::configure( const std::string workingDirectory)
{
  LIMA_UNUSED(workingDirectory);
}

void TestCaseProcessor::terminate()
{
}

TestCaseProcessor::TestCaseProcessor(  const std::string workingDirectory) :
    m_workingDirectory(workingDirectory)
{
  configure( workingDirectory);
}

TestCaseProcessor::~TestCaseProcessor() {
  terminate();
}

TestCaseError TestCaseProcessor::evalTestCase(
  const TestCase& testCase, const std::string& pipeName,
  const std::string& textFile, const std::string& traceFilePrefix ) const
{
  LIMA_UNUSED(textFile);
  TGVLOGINIT;
  // Valid TestUnits
  for (auto tuItr = testCase.tests.cbegin(); tuItr != testCase.tests.cend(); tuItr++)
  {
    std::string traceFile(traceFilePrefix+tuItr->trace);
    QFile sourceDocument;
    sourceDocument.setFileName(traceFile.c_str());
    sourceDocument.open(QIODevice::ReadOnly);

    /* Load XML document */
    QXmlQuery theDocument;
    if (!theDocument.setFocus(&sourceDocument)) {
      LERROR << "Error: Unable to parse file " << traceFile;
      return TestCaseError(testCase, TestCaseError::TestCaseFailed,"No output file to evaluate !", pipeName, *tuItr);
    }

    // OK, let's evaluate the expression...
    // Check unary operators before evaluate se right member

    if (tuItr->op=="notexists")
    {
      if (existsExpression(tuItr->left,theDocument))
      {
        return TestCaseError(testCase, TestCaseError::TestCaseFailed,"an element exists !", pipeName, *tuItr);
      }
    }
    else if (tuItr->op=="exists")
    {
      if (!existsExpression(tuItr->left,theDocument))
      {
        std::ostringstream oss;
        oss << "element doesn't exist ! : " << tuItr->left;
        return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, *tuItr);
      }
    }
    else
    {

      // operator is a binary one. evaluate the second expression
      QStringList left=evaluateExpression(tuItr->left,theDocument);
      left.removeDuplicates();
      QStringList right=evaluateExpression(tuItr->right,theDocument);
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

      if (tuItr->op=="=")
      {
        if (sleft!=sright)
        {
          std::ostringstream oss;
          oss << "equality check failed : " << left << " != " << right << std::endl;
          oss << "                        left : " << tuItr->left << std::endl;
          oss << "                        right: " << tuItr->right;

          return TestCaseError(testCase, TestCaseError::TestCaseFailed, oss.str(), pipeName, *tuItr);
        }
        else if (left.size()==0)
        {
          std::ostringstream oss;
          oss << "left and right member are empty !";
          return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, *tuItr);
        }
      }
      else if (tuItr->op=="!=")
      {
        if (sleft==sright)
        {
          std::ostringstream oss;
          oss << "inequality check failed : " << left << " == " << right << std::endl;
          oss << "                          left : " << tuItr->left << std::endl;
          oss << "                          right: " << tuItr->right;
          return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, *tuItr);
        }
      }
      else if (tuItr->op=="contains")
      {
        Q_FOREACH(QString s, sright)
        {
          if (!sleft.contains(s))
          {
            std::ostringstream oss;
            oss << "includes check failed : " << left << " not contains " << right << std::endl;
            oss << "                          left : " << tuItr->left << std::endl;
            oss << "                          right: " << tuItr->right;
            return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, *tuItr);
          }
        }
      }
      else if (tuItr->op=="notcontains")
      {
        Q_FOREACH(QString s, sleft)
        {
          if (!sright.contains(s))
          {
            std::ostringstream oss;
            oss << "not includes check failed : " << left << " contains " << right;
            return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, *tuItr);
          }
        }
      }
      else if (tuItr->op=="distinct")
      {
        Q_FOREACH(QString l, sleft)
        {
          Q_FOREACH(QString r, sright)
          {
            if (l==r)
            {
              std::ostringstream oss;
              oss << l.toUtf8().data() << " is a common value of the two sets that should be disctinct !" << std::endl;
              oss << "                          left : " << tuItr->left << std::endl;
              oss << "                          right: " << tuItr->right;
              return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, *tuItr);
            }
          }
        }
      }
      else if (tuItr->op=="intersect")
      {
        Q_FOREACH(QString l, sleft)
        {
          Q_FOREACH(QString r, sright)
          {
            if (l==r)
            {
              return TestCaseError(testCase, TestCaseError::NoError, "", pipeName, *tuItr);
            }
          }
        }
        std::ostringstream oss;
        oss << "intersect failed : no common value between " << left << " and " << right << " !" << std::endl;
        oss << "                          left : " << tuItr->left << std::endl;
        oss << "                          right: " << tuItr->right;
        return TestCaseError(testCase, TestCaseError::TestCaseFailed,oss.str(), pipeName, *tuItr);
      }
      else if ((tuItr->op!="exists") && (tuItr->op!="notexists"))
      {
        return TestCaseError(testCase, TestCaseError::InvalidOperator, (*tuItr).id, pipeName, *tuItr);
      }
    }
  }
  return TestCaseError(testCase, "", pipeName);
}

QStringList TestCaseProcessor::evaluateExpression(
    const std::string& expr,
    QXmlQuery& document)
{
  TGVLOGINIT;
  QStringList results;

  if (expr.substr(0,6)=="XPATH#")
  {
    /* Evaluate xpath expression */
    QString xpath = QString::fromUtf8(expr.substr(6).c_str())+"/string()";
    LDEBUG << "TestCaseProcessor::evaluateExpression setQuery("<<xpath<<");";
    document.setQuery(xpath);
    if(!document.evaluateTo(&results)) {
      LERROR << "TestCaseProcessor::evaluateExpression unable to evaluate xpath expression \""<<xpath;
      return results;
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

bool TestCaseProcessor::existsExpression(
  const std::string& expr,
  QXmlQuery& document)
{
//   TGVLOGINIT;
  return !evaluateExpression(expr,document).empty();
}


} // TGV
} // Common
} // Lima
