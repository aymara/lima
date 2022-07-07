// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * @author Benoit Mathieu
 */

#ifndef LIMA_TGV_TESTCASESHANDLER_H
#define LIMA_TGV_TESTCASESHANDLER_H

#include "TestCase.h"
#include "TestCaseProcessor.hpp"

namespace Lima
{
namespace Common
{
namespace TGV
{

class TestCasesReaderPrivate;
class LIMA_TGV_EXPORT TestCasesReader
{

public:
  TestCasesReader(TestCaseProcessor& processor);
  ~TestCasesReader();

  struct TestReport {
    uint64_t success;
    uint64_t failed;
    uint64_t conditional;
    uint64_t nbtests;
    TestReport() : success(0),failed(0),conditional(0),nbtests(0) {};
  };

  bool parse(QIODevice *device);

  QString errorString() const;

  const std::map<std::string,TestReport>& reportByType() const;

  bool hasFatalError() const;

  void clear();

private:
  TestCasesReaderPrivate* m_d;
};

// This utility function return a user-defined exit code (64) if a "bloquant"
// test failed.
LIMA_TGV_EXPORT int exitCode(TestCasesReader const & tch);

} // TGV
} // Common
} // Lima


#endif
