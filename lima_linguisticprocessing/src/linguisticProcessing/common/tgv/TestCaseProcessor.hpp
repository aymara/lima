// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * @author Benoit Mathieu <benoit.mathieu@cea.fr>                                 *
 * @date begin 2004
 **/
#ifndef LIMA_COMMON_TGV_TESTCASEPROCESSOR_HPP
#define LIMA_COMMON_TGV_TESTCASEPROCESSOR_HPP

#include "TestCase.h"
#include "TestCaseError.hpp"

#include "common/LimaCommon.h"

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

class TestCaseProcessorPrivate;
class LIMA_TGV_EXPORT TestCaseProcessor
{
public:

  TestCaseProcessor( const std::string workingDirectory );
  virtual ~TestCaseProcessor() = default;

  void configure(const std::string workingDirectory);

  virtual TestCaseError processTestCase(const TestCase& testCase) = 0;


protected:
  const QString& workingDirectory() const;

  TestCaseError evalTestCase(
    const TestCase& testCase, const std::string& pipeName,
    const std::string& textFile, const std::string& traceFilePrefix ) const;


private:
  TestCaseProcessorPrivate* m_d;
};

} // TGV
} // Common
} // Lima

std::ostream& operator<<(std::ostream& oss, const QStringList& qsl);

#endif
