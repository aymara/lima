// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
