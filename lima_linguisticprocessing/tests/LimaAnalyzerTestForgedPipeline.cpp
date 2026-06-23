// Copyright 2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "LimaAnalyzerTestForgedPipeline.h"
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



void LimaAnalyzerTestForgedPipeline::initTestCase()
{
}

void LimaAnalyzerTestForgedPipeline::test_forged_pipeline()
{
    qDebug() << "LimaAnalyzerTestForgedPipeline::test_forged_pipeline instantiate lima";
    // The language id carries the treebank stem so that the "udlang" option
    // resolves to "UD_English-EWT": the deeplima models are named after the
    // treebank (RnnTokenizer/ud/tokenizer-UD_English-EWT.pt), as installed by
    // `deeplima_models -i UD_English-EWT`. A bare "ud-eng" would resolve udlang
    // to "eng" and the $udlang-templated model files would not be found.
    LimaAnalyzer lima("ud-UD_English-EWT", "empty", "");

    qDebug() << "LimaAnalyzerTestForgedPipeline::test_forged_pipeline add tokenizer";

    // Testing the forging of pipelines
    //   auto langid = Lima::Common::MediaticData::MediaticData::single().getMediaId("ud-UD_English-EWT");
    //   auto pipe = MediaProcessors::changeable().getPipelineForId(langid, pipeline);
    //   auto managers = Lima::MediaProcessors::single().managers();
    //   {
    // The libtorch (deeplima) units, mirroring the deepud pipeline configuration.
    std::string tokenizerConfig = R"(
        {
            "name": "RnnTokenizer",
            "class": "RnnTokenizer",
            "model_prefix": "tokenizer-$udlang"
        }
    )";
    QVERIFY(lima.addPipelineUnit("empty", "ud-UD_English-EWT", tokenizerConfig));

    qDebug() << "LimaAnalyzerTestForgedPipeline::test_forged_pipeline add tagger and lemmatizer";
    // RnnTokensAnalyzer writes UPOS/features into the MorphoSyntacticData and the lemma.
    std::string taggerConfig = R"(
        {
            "name": "RnnTokensAnalyzer",
            "class": "RnnTokensAnalyzer",
            "tagger_model_prefix": "tagger-$udlang",
            "lemmatizer_model_prefix": "lemmatizer-$udlang"
        }
    )";
    QVERIFY(lima.addPipelineUnit("empty", "ud-UD_English-EWT", taggerConfig));

    qDebug() << "LimaAnalyzerTestForgedPipeline::test_forged_pipeline add dumper";
    std::string dumperConfig = R"(
        {
            "name": "conllDumper",
            "class": "ConllDumper",
            "format": "CoNLL-U",
            "handler": "simpleStreamHandler",
            "fakeDependencyGraph": "false",
            "withColsHeader": "true"
        }
    )";
    QVERIFY(lima.addPipelineUnit("empty", "ud-UD_English-EWT", dumperConfig));

    qDebug() << "LimaAnalyzerTestForgedPipeline::test_forged_pipeline run analyzeText";
    auto result = lima.analyzeText("This is a text on 02/05/2022.", "ud-UD_English-EWT", "empty");
    qDebug() << "LimaAnalyzerTestForgedPipeline::test_forged_pipeline result:" << Qt::endl << result;
    QVERIFY(!lima.error);
}

QTEST_GUILESS_MAIN(LimaAnalyzerTestForgedPipeline)

