// Copyright 2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "LimaAnalyzerTest0.h"
#include "client/lima.h"

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

void LimaAnalyzerTest0::test_two_same_instances()
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

void LimaAnalyzerTest0::test_two_different_instances()
{
    LimaAnalyzer analyzer0("ud-eng", "deepud", "");
    auto result1 = analyzer0.analyzeText("Hop ! Hop !");
    qDebug() << "LimaAnalyzerTest0::test_two_different_instances analyzer 1:" << result1;

    LimaAnalyzer analyzer1("eng", "main", "");
    auto result2 = analyzer1.analyzeText("One, 2, tree.");
    qDebug() << "LimaAnalyzerTest0::test_two_different_instances analyzer 2:" << result2;

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
    LimaAnalyzer thelima("ud-eng", "", "");
    // QVERIFY_THROWS_EXCEPTION has been introduced in Qt6.3 and Ubuntu 22.04
    // has only Qt6.2

    // QVERIFY_THROWS_EXCEPTION(std::exception, thelima.analyzeText(
    //   "This is a text on 02/05/2022.", "ud-eng", nullptr));

    do {
        QT_TRY {
            thelima.analyzeText("This is a text on 02/05/2022.",
                                                 "ud-eng", nullptr);
            QTest::qFail("Expected exception of type std::exception to be thrown"
                         " but no exception caught", __FILE__, __LINE__);
            return;
        } QT_CATCH (const std::exception &) {
            /* success */
        } QT_CATCH (...) {
            // QTest::qCaught("std::exception", __FILE__, __LINE__);
            return;
        }
    } while (false);

}


void LimaAnalyzerTest0::test_analyzeText_pipeline_none_lang_eng() {
    LimaAnalyzer thelima("eng", "", "");
    QVERIFY(thelima.error);
    // QVERIFY_THROWS_EXCEPTION has been introduced in Qt6.3 and Ubuntu 22.04
    // has only Qt6.2

    // QVERIFY_THROWS_EXCEPTION(std::exception,
    //                          thelima.analyzeText("This is a text on 02/05/2022.",
    //                                              "eng", nullptr));
    do {
        QT_TRY {
            thelima.analyzeText("This is a text on 02/05/2022.",
                                                 "eng", nullptr);
            QTest::qFail("Expected exception of type std::exception to be thrown"
                         " but no exception caught", __FILE__, __LINE__);
            return;
        } QT_CATCH (const std::exception &) {
            /* success */
        } QT_CATCH (...) {
            // QTest::qCaught("std::exception", __FILE__, __LINE__);
            return;
        }
    } while (false);

}

void LimaAnalyzerTest0::test_analyzeText_init_with_lang_and_pipe() {
    LimaAnalyzer thelima("ud-eng", "deepud", "");
    QVERIFY(
      typeid(thelima.analyzeText("This is a text on 02/05/2022.")) == typeid(std::string));
}

void LimaAnalyzerTest0::test_analyzeText_pipeline_not_avail()
{
    LimaAnalyzer lima("ud-eng", "deepud", "");
    lima.analyzeText("This is a text on 02/05/2022.", "other");
    QVERIFY(lima.error);
}

// Regression for the neural-lemmatizer crash on multi-byte UTF-8 tokens.
// RnnTokensAnalyzer used to set token_pos::m_len from the LIMA character count
// while token_pos::m_pch points to UTF-8 bytes. For a single multi-byte character
// used as a token (e.g. the French preposition "à", U+00E0 = 2 bytes) the form was
// truncated to one byte, decoded to an empty string, and crashed the seq2seq
// lemmatizer (input_len == 0). The analysis must now complete and keep "à".
void LimaAnalyzerTest0::test_analyzeText_multibyte_single_char_token() {
    LimaAnalyzer analyzer("ud-eng", "deepud", "");
    QVERIFY(!analyzer.error);
    auto result = analyzer.analyzeText("à Berlin.");
    if (result.empty())
        QSKIP("deepud model not available in this environment");
    // "à" (UTF-8 0xC3 0xA0) and "Berlin" must survive tokenization intact.
    QVERIFY(result.find("à") != std::string::npos);
    QVERIFY(result.find("Berlin") != std::string::npos);
}

// Multi-byte characters inside longer tokens must not be truncated in the output.
void LimaAnalyzerTest0::test_analyzeText_multibyte_forms_preserved() {
    LimaAnalyzer analyzer("ud-eng", "deepud", "");
    QVERIFY(!analyzer.error);
    auto result = analyzer.analyzeText("A café, a naïve résumé.");
    if (result.empty())
        QSKIP("deepud model not available in this environment");
    QVERIFY(result.find("café") != std::string::npos);
    QVERIFY(result.find("naïve") != std::string::npos);
    QVERIFY(result.find("résumé") != std::string::npos);
}

// Wider UTF-8 sequences (3-byte em dash, 4-byte emoji) must not crash the analysis
// nor corrupt neighbouring ASCII tokens.
void LimaAnalyzerTest0::test_analyzeText_multibyte_wide_chars() {
    LimaAnalyzer analyzer("ud-eng", "deepud", "");
    QVERIFY(!analyzer.error);
    auto result = analyzer.analyzeText("Hello — world 😀 end.");
    if (result.empty())
        QSKIP("deepud model not available in this environment");
    QVERIFY(result.find("Hello") != std::string::npos);
    QVERIFY(result.find("world") != std::string::npos);
    QVERIFY(result.find("end") != std::string::npos);
}

// void LimaAnalyzerTest0::test_forged_pipeline()
// {
//     LimaAnalyzer lima("ud-eng", "empty", "");
//
//     // Testing the forging of pipelines
//     //   auto langid = Lima::Common::MediaticData::MediaticData::single().getMediaId("ud-eng");
//     //   auto pipe = MediaProcessors::changeable().getPipelineForId(langid, pipeline);
//     //   auto managers = Lima::MediaProcessors::single().managers();
//     //   {
//     std::string jsonGroupString =
//         "{ \"name\":\"cpptftokenizer\", "
//         "  \"class\":\"CppUppsalaTensorFlowTokenizer\", "
//         "  \"model_prefix\": \"tokenizer-eng\" }";
//     lima.addPipelineUnit("empty", "ud-eng", jsonGroupString);
//     jsonGroupString =
//         "{ \"name\":\"tfmorphosyntax\", "
//         "  \"class\":\"TensorFlowMorphoSyntax\", "
//         "  \"model_prefix\": \"morphosyntax-eng\", "
//         "  \"embeddings\": \"fasttext-eng.bin\" "
//         "}";
//     lima.addPipelineUnit("empty", "ud-eng", jsonGroupString);
//     jsonGroupString =
//         "{ \"name\":\"conllDumper\", "
//         "  \"class\":\"ConllDumper\", "
//         "  \"handler\": \"simpleStreamHandler\" "
//         "  \"fakeDependencyGraph\": \"false\", "
//         "}";
//     lima.addPipelineUnit("empty", "ud-eng", jsonGroupString);
//     lima.analyzeText("This is a text on 02/05/2022.", "ud-eng", "empty");
//     QVERIFY(lima.error);
// }

QTEST_GUILESS_MAIN(LimaAnalyzerTest0)

