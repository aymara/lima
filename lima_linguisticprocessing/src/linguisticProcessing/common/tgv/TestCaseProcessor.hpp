// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_COMMON_TGV_TESTCASEPROCESSOR_HPP
#define LIMA_COMMON_TGV_TESTCASEPROCESSOR_HPP

#include "TestCase.h"
#include "TestCaseError.hpp"

#include "common/LimaCommon.h"

#include <QtXmlPatterns/QXmlQuery>

#include <ostream>
#include <string>
#include <map>
#include <list>
#include <stdexcept>


namespace Lima
{
namespace Common
{
namespace TGV
{

class LIMA_TGV_EXPORT TestCaseProcessor
{
public:

  TestCaseProcessor( const std::string workingDirectory );

  void configure(
    const std::string workingDirectory);

  virtual TestCaseError processTestCase(const TestCase& testCase) = 0;
 
  virtual ~TestCaseProcessor();

protected:
  TestCaseError evalTestCase(
    const TestCase& testCase, const std::string& pipeName,
    const std::string& textFile, const std::string& traceFilePrefix ) const;

  std::string m_workingDirectory;
  
private:
  void terminate();
    
  static QStringList evaluateExpression(
    const std::string& expr,
    QXmlQuery& document);
// 
  static bool existsExpression(
    const std::string& expr,
    QXmlQuery& document);
    
};

} // TGV
} // Common
} // Lima

std::ostream& operator<<(std::ostream& oss, const QStringList& qsl);

#endif
