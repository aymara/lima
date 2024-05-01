// Copyright 2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "LimaAnalyzerTest0.h"
#include "lima.h"

#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/QsLog/QsLog.h"
#include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/QsLog/QsDebugOutput.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"
#include "common/tools/LimaMainTaskRunner.h"

#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/LTRTextHandler.h"
#include "linguisticProcessing/core/EventAnalysis/EventHandler.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"


#include <deque>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <QtCore/QCoreApplication>
#include <QtCore/QString>

#include <QtCore>


using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima;



void LimaAnalyzerTest0::initTestCase()
{
}

void LimaAnalyzerTest0::test_two_instances()
{
  // qDebug() << "LimaAnalyzerTest0::test_load";
  {
    LimaAnalyzer analyzer("eng", "main", "");
    auto result = analyzer.analyzeText("Hello 1");
    qDebug() << "LimaAnalyzerTest0::test_load analyzer 1:" << result;
  }
  QVERIFY( true );
  {
    LimaAnalyzer analyzer("eng", "main", "");
    auto result = analyzer.analyzeText("Hello 2");
    qDebug() << "LimaAnalyzerTest0::test_load analyzer 2:" << result;
  }
  QVERIFY( true );

}

void LimaAnalyzerTest0::test_unknownLanguage() {
    QVERIFY(LimaAnalyzer("this_is_not_a_language_name", "", "").error);
}

void LimaAnalyzerTest0::test_lang_no_prefix() {
    QVERIFY(LimaAnalyzer("cym", "", "").error);
}

void LimaAnalyzerTest0::test_several_instances() {
    LimaAnalyzer lima1("eng", "main", "");
    std::string text = "Give it back! He pleaded.";

    auto doc1 = lima1.analyzeText(text);
    QVERIFY(typeid(doc1) == typeid(std::string));

    LimaAnalyzer lima2("fre", "main", "");
    auto doc2 = lima2.analyzeText("Et maintenant, du français.");
    QVERIFY(typeid(doc2) == typeid(std::string));
}


void LimaAnalyzerTest0::test_analyzeText_lang_not_str() {
    // When the analyzer is in error, analyzeText does nothing
    QVERIFY(LimaAnalyzer("", "", "").analyzeText(
      "This is a text on 02/05/2022.", "", "main") == "");
}
void LimaAnalyzerTest0::test_analyzeText_lang_not_init() {
    // When the analyzer is in error, analyzeText does nothing
    QVERIFY(LimaAnalyzer("", "", "").analyzeText(
      "This is a text on 02/05/2022.", "wol", "main") == "");
}
void LimaAnalyzerTest0::test_analyzeText_pipeline_none_lang_ud() {
    // Your test logic here
    LimaAnalyzer thelima("ud-eng", "", "");
    QVERIFY_THROWS_EXCEPTION(std::exception, thelima.analyzeText(
      "This is a text on 02/05/2022.", "ud-eng", nullptr));
}


void LimaAnalyzerTest0::test_analyzeText_pipeline_none_lang_eng() {
    LimaAnalyzer thelima("eng", "", "");
    QVERIFY(thelima.error);
    QVERIFY_THROWS_EXCEPTION(std::exception,
                             thelima.analyzeText("This is a text on 02/05/2022.",
                                                 "eng", nullptr));
}

void LimaAnalyzerTest0::test_analyzeText_init_with_lang_and_pipe() {
    LimaAnalyzer thelima("ud-eng", "deepud", "");
    QVERIFY(
      typeid(thelima.analyzeText("This is a text on 02/05/2022.")) == typeid(std::string));
}

void LimaAnalyzerTest0::test_analyzeText_pipeline_not_avail() {
    LimaAnalyzer lima("ud-eng", "deepud", "");
    lima.analyzeText("This is a text on 02/05/2022.", "other");
    QVERIFY(lima.error);
}

QTEST_GUILESS_MAIN(LimaAnalyzerTest0)

