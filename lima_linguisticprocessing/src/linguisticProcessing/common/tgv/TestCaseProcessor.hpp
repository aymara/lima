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
