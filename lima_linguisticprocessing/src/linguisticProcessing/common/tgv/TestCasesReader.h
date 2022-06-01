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

#ifndef LIMA_TGV_TESTCASESHANDLER_H
#define LIMA_TGV_TESTCASESHANDLER_H

#include "TestCase.h"
#include "TestCaseProcessor.hpp"

#include <QtXml/QXmlDefaultHandler>

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
