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



// The legacy eng/fre/por media are deprecated and only built with
// WITH_LEGACY_PIPELINES=ON. Otherwise the instance tests use the deeplima-backed
// UD media.
#if LIMA_WITH_LEGACY_PIPELINES
#define ENG_LANG "eng"
#define ENG_PIPE "main"
#define FRE_LANG "fre"
#define FRE_PIPE "main"
#define FRE_META ""
#else
#define ENG_LANG "ud-eng"
#define ENG_PIPE "deepud"
#define FRE_LANG "ud-fra"
#define FRE_PIPE "deepud"
#define FRE_META "udlang:fra-UD_French-MERGE"
#endif

void LimaAnalyzerTest0::initTestCase()
{
}

void LimaAnalyzerTest0::test_two_same_instances()
{
  // qDebug() << "LimaAnalyzerTest0::test_load";
  {
    LimaAnalyzer analyzer(ENG_LANG, ENG_PIPE, "");
    auto result = analyzer.analyzeText("Hello 1");
    qDebug() << "LimaAnalyzerTest0::test_load analyzer 1:" << result;
  }
  QVERIFY( true );
  {
    LimaAnalyzer analyzer(ENG_LANG, ENG_PIPE, "");
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

    LimaAnalyzer analyzer1(ENG_LANG, ENG_PIPE, "");
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
    LimaAnalyzer lima1(ENG_LANG, ENG_PIPE, "");
    std::string text = "Give it back! He pleaded.";

    auto doc1 = lima1.analyzeText(text);
    QVERIFY(typeid(doc1) == typeid(std::string));

    LimaAnalyzer lima2(FRE_LANG, FRE_PIPE, "", "", "", FRE_META);
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
    // Without any pipeline the analyzer cannot be initialized: it is in error
    // and analyzeText refuses to analyze. (This test used to pass nullptr as the
    // pipeline name, which is undefined behavior: libstdc++ throws, the MSVC
    // runtime crashes.)
    LimaAnalyzer thelima("ud-eng", "", "");
    QVERIFY(thelima.error);
    auto result = thelima.analyzeText("This is a text on 02/05/2022.", "ud-eng", "");
    QVERIFY(thelima.error);
    QVERIFY(result.empty());
}


void LimaAnalyzerTest0::test_analyzeText_pipeline_none_lang_eng() {
#if !LIMA_WITH_LEGACY_PIPELINES
    QSKIP("legacy eng media not built (WITH_LEGACY_PIPELINES=OFF)");
#endif
    // Without any pipeline the analyzer cannot be initialized: it is in error
    // and analyzeText refuses to analyze. (This test used to pass nullptr as the
    // pipeline name, which is undefined behavior: libstdc++ throws, the MSVC
    // runtime crashes.)
    LimaAnalyzer thelima("eng", "", "");
    QVERIFY(thelima.error);
    auto result = thelima.analyzeText("This is a text on 02/05/2022.", "eng", "");
    QVERIFY(thelima.error);
    QVERIFY(result.empty());
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
    LimaAnalyzer analyzer("ud", "deepud", "", "", "", "udlang:fra-UD_French-MERGE");
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
    LimaAnalyzer analyzer("ud", "deepud", "", "", "", "udlang:fra-UD_French-MERGE");
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
    LimaAnalyzer analyzer("ud", "deepud", "", "", "", "udlang:fra-UD_French-MERGE");
    QVERIFY(!analyzer.error);
    // 3-byte em dash "—" and a 4-byte emoji "😀" between ASCII tokens.
    auto result = analyzer.analyzeText("Voici — un test 😀 final.");
    if (result.empty())
        QSKIP("deepud model not available in this environment");
    QVERIFY(result.find("Voici") != std::string::npos);
    QVERIFY(result.find("test") != std::string::npos);
    QVERIFY(result.find("final") != std::string::npos);
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

// Token positions are 1-based character offsets of the text. RnnTokenizer
// gave the end of the previous token, before the spaces in between (one
// character early after a space), counted in UTF-8 bytes (one more per
// non-ASCII character before): lima-python spans cut words.
void LimaAnalyzerTest0::test_analyzeText_token_positions() {
    LimaAnalyzer analyzer("ud", "deepud", "", "", "", "udlang:fra-UD_French-MERGE");
    QVERIFY(!analyzer.error);
    auto result = analyzer.analyzeText("Un café chaud.");
    if (result.empty())
        QSKIP("deepud model not available in this environment");
    QVERIFY(result.find("\tPos=1|Len=2") != std::string::npos);   // Un
    QVERIFY(result.find("\tPos=4|Len=4") != std::string::npos);   // café
    QVERIFY(result.find("\tPos=9|Len=5") != std::string::npos);   // chaud
    QVERIFY(result.find("\tPos=14|Len=1") != std::string::npos);  // .
    // "Un" is followed by a space (the early position of "café" made it
    // look glued to it)
    QVERIFY(result.find("\tPos=1|Len=2|SpaceAfter=No") == std::string::npos);
}

// The words of a multiword token ("du" -> "de" "le") are written after its
// N-M line, which was never written (its ids were counted one vertex off)
void LimaAnalyzerTest0::test_analyzeText_multiword_tokens() {
    LimaAnalyzer analyzer("ud-fra", "deepud", "", "", "", "udlang:fra-UD_French-MERGE");
    QVERIFY(!analyzer.error);
    auto result = analyzer.analyzeText("Jean Dupont vient du Japon et parle au directeur.");
    if (result.empty())
        QSKIP("deepud model not available in this environment");
    QVERIFY(result.find("\n4-5\tdu\t") != std::string::npos);
    QVERIFY(result.find("\n4\tde\t") != std::string::npos);
    QVERIFY(result.find("\n5\tle\t") != std::string::npos);
    QVERIFY(result.find("\n9-10\tau\t") != std::string::npos);
    // The words that do not spell the surface share its position
    QVERIFY(result.find("\tPos=19|Len=2\n5\tle") != std::string::npos);
}

// "Apple's" is a multiword token of the English MERGE model ("Apple" "'s"):
// with "Apple" recognized as an entity, ConllDumper read the token of the
// entity's PosGraph vertex from the AnalysisGraph, out of it (StringsPool
// out of bounds or segfault)
void LimaAnalyzerTest0::test_analyzeText_entity_before_multiword_token() {
    LimaAnalyzer analyzer("ud-eng", "deepud", "", "", "", "udlang:eng-UD_English-MERGE");
    if (analyzer.error)
        QSKIP("eng-UD_English-MERGE model not available in this environment");
    auto result = analyzer.analyzeText("Apple's CEO met Microsoft's.");
    if (result.empty())
        QSKIP("eng-UD_English-MERGE model not available in this environment");
    QVERIFY(!analyzer.error);
    QVERIFY(result.find("\n1-2\tApple's\t") != std::string::npos);
    // The words that spell the surface have their own positions
    QVERIFY(result.find("\t's\t's\t") != std::string::npos);
    QVERIFY(result.find("|Pos=1|Len=5") != std::string::npos);
    QVERIFY(result.find("\tPos=6|Len=2") != std::string::npos);
    // SpaceAfter=No is on the multiword token line, not on its last word
    QVERIFY(result.find("\n5-6\tMicrosoft's\t_\t_\t_\t_\t_\t_\t_\tSpaceAfter=No\n")
            != std::string::npos);
}

// The English MERGE model predicts GUM's obl:unmarked for "morning", a
// relation missing from LIMA's relationList: its arc was dropped (HEAD 0,
// DEPREL _). The parser now registers its model's relations
void LimaAnalyzerTest0::test_analyzeText_model_relation_missing_from_config() {
    LimaAnalyzer analyzer("ud-eng", "deepud", "", "", "", "udlang:eng-UD_English-MERGE");
    if (analyzer.error)
        QSKIP("eng-UD_English-MERGE model not available in this environment");
    auto result = analyzer.analyzeText("Mr. Best flew to New York on Saturday morning.");
    if (result.empty())
        QSKIP("eng-UD_English-MERGE model not available in this environment");
    QVERIFY(!analyzer.error);
    QVERIFY(result.find("\tmorning\tmorning\t") != std::string::npos);
    QVERIFY(result.find("\tmorning\tNOUN\t_\tNumber=Sing\t0\t_\t") == std::string::npos);
}

// The CoNLL-U output of a second analysis by the same analyzer had the heads
// and relations of the first one (ConllDumper kept them)
void LimaAnalyzerTest0::test_analyzeText_second_text_relations() {
    LimaAnalyzer analyzer("ud-fra", "deepud", "", "", "", "udlang:fra-UD_French-MERGE");
    QVERIFY(!analyzer.error);
    auto first = analyzer.analyzeText("Le petit chat noir dort sur le grand tapis rouge.");
    if (first.empty())
        QSKIP("deepud model not available in this environment");
    auto second = analyzer.analyzeText("Il parle.");
    LimaAnalyzer fresh("ud-fra", "deepud", "", "", "", "udlang:fra-UD_French-MERGE");
    auto expected = fresh.analyzeText("Il parle.");
    // Compared on ID, FORM, LEMMA, UPOS, HEAD, DEPREL: the tagger's features
    // may depend on the previous analysis (a separate issue)
    auto columns = [](const std::string& conllu) {
        QStringList result;
        for (const auto& line : QString::fromStdString(conllu).split('\n'))
        {
            auto cols = line.split('\t');
            if (cols.size() == 10)
                result << QStringList({cols[0], cols[1], cols[2], cols[3], cols[6], cols[7]}).join('\t');
        }
        return result;
    };
    QCOMPARE(columns(second), columns(expected));
    QVERIFY(second.find("\tparler\tVERB\t") != std::string::npos);
    QVERIFY(second.find("\t0\troot\t") != std::string::npos);
}

QTEST_GUILESS_MAIN(LimaAnalyzerTest0)
