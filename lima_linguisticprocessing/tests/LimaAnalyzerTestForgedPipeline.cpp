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
    LimaAnalyzer lima("ud-eng", "empty", "");

    qDebug() << "LimaAnalyzerTestForgedPipeline::test_forged_pipeline add tokenizer";

    // Testing the forging of pipelines
    //   auto langid = Lima::Common::MediaticData::MediaticData::single().getMediaId("ud-eng");
    //   auto pipe = MediaProcessors::changeable().getPipelineForId(langid, pipeline);
    //   auto managers = Lima::MediaProcessors::single().managers();
    //   {
    std::string tokenizerConfig = R"(
        {
            "name": "cpptftokenizer",
            "class": "CppUppsalaTensorFlowTokenizer",
            "model_prefix": "tokenizer-eng"
        }
    )";
    QVERIFY(lima.addPipelineUnit("empty", "ud-eng", tokenizerConfig));

    qDebug() << "LimaAnalyzerTestForgedPipeline::test_forged_pipeline add tagger and parser";
    std::string taggerConfig = R"(
        {
            "name": "tfmorphosyntax",
            "class": "TensorFlowMorphoSyntax",
            "model_prefix": "morphosyntax-eng",
            "embeddings": "fasttext-eng.bin"
        }
    )";
    QVERIFY(lima.addPipelineUnit("empty", "ud-eng", taggerConfig));

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
    QVERIFY(lima.addPipelineUnit("empty", "ud-eng", dumperConfig));

    qDebug() << "LimaAnalyzerTestForgedPipeline::test_forged_pipeline run analyzeText";
    auto result = lima.analyzeText("This is a text on 02/05/2022.", "ud-eng", "empty");
    qDebug() << "LimaAnalyzerTestForgedPipeline::test_forged_pipeline result:" << Qt::endl << result;
    QVERIFY(!lima.error);
}

QTEST_GUILESS_MAIN(LimaAnalyzerTestForgedPipeline)

