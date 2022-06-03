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
#ifndef LIMA_COMMON_TGV_TESTCASEERROR_HPP
#define LIMA_COMMON_TGV_TESTCASEERROR_HPP

#include "TestCase.h"

#include <string>
#include <map>
#include <list>
#include <stdexcept>
#include "common/LimaCommon.h"

namespace Lima
{
namespace Common
{
namespace TGV
{

class LIMA_TGV_EXPORT TestCaseError
{
public:
  enum ErrorTypes
  {
    NoError,
    InvalidKey,
    InvalidOperator,
    TestCaseFailed
  };
  TestCaseError() : m_type(NoError), m_reason(""), m_conditional(false) {}

  TestCaseError(const TestCase& tc,
                const std::string& reason,
                const std::string& pipeline);

  TestCaseError(const TestCase& tc, ErrorTypes type,
                const std::string& reason,
                const std::string& pipeline,
                const TestCase::TestUnit& tu);

  TestCaseError(const TestCaseError& error) :
      m_type(error.m_type) ,m_reason(error.m_reason), m_conditional(error.m_conditional) {}

  TestCaseError& operator=(const TestCaseError& error)
  {
      m_type = error.m_type;
      m_reason = error.m_reason;
      m_conditional = error.m_conditional;
      return *this;
  }

  virtual ~TestCaseError() = default;

  const std::string& what() const {return m_reason;}
  ErrorTypes operator()() {return m_type;}
  bool isConditional() const {return m_conditional;}

protected:
  ErrorTypes m_type;
  std::string m_reason;
  bool m_conditional;
};



} // TGV
} // Common
} // Lima

#endif
