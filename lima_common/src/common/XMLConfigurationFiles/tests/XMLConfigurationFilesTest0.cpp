// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE XMLConfigurationFilesTest
#include <boost/test/unit_test.hpp>

#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/QsLog/QsLog.h"
#include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/QsLog/QsDebugOutput.h"

using namespace Lima;
using namespace Lima::Common::XMLConfigurationFiles;
    
// conversion functions
BOOST_AUTO_TEST_CASE( XMLConfigurationFilesTest0 )
{
  XMLConfigurationFileParser parser("test-config-file.xml");
  BOOST_REQUIRE( parser.getModuleGroupParamValue("module1","group1","p1") == "v1");
}

