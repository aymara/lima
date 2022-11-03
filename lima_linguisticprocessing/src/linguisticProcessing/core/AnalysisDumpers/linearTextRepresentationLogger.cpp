// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** =========================================================================
    @file       linearTextRepresentationLogger.cpp

    @version    $Id$
    @date       created       jun 28, 2005
    @date       last revised  jan 4, 2011

    @author     Olivier Ferret
    @brief      dump on a file a linear representation of the graph resulting
                from the linguistic analysis of a document

    Copyright (C) 2005-2011 by CEA LIST

    ========================================================================= */

#include "linearTextRepresentationLogger.h"

// #include "linguisticProcessing/core/LinguisticProcessors/HandlerStreamBuf.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"
#include "common/LimaCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <iostream>
#include <fstream>

using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace std;


namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

SimpleFactory<MediaProcessUnit, LinearTextRepresentationLogger> ltrLoggerFactory(LTRLOGGER_CLASSID);

/**
  * @class  LinearTextRepresentationLogger
  * @brief  dump on a file a linear representation of the graph resulting
  *         from the linguistic analysis of a document
  */

// -----------------------------------------------------------------------------
// -- process unit management
// -----------------------------------------------------------------------------

void LinearTextRepresentationLogger::init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) {

    DUMPERLOGINIT;
    m_language = manager->getInitializationParameters().media;
    // a stop list is used ?
    bool useStopList = false;
    try {
        string stopFlag = unitConfiguration.getParamsValueAtKey("useStopList");
        useStopList = (stopFlag == "true");
    }
    catch (NoSuchParam& ) {
        LWARN << "No param 'useStopList' in ltrDumper configuration group for language "
              << m_language;
        LWARN << "use default value: false";
    }
    // output suffix
    try {
        m_outputSuffix = unitConfiguration.getParamsValueAtKey("outputSuffix");
    }
    catch (NoSuchParam& ) {
        m_outputSuffix = string(".ltr");
    }
    // get stop list
    if (useStopList) {
        try {
            string stoplist = unitConfiguration.getParamsValueAtKey("stopList");
            m_stopList = static_cast<StopList*>(LinguisticResources::single().getResource(m_language, stoplist));
        }
        catch (NoSuchParam& ) {
            LERROR << "No param 'stopList' in LinearTextRepresentationLogger configuration group for language " << m_language;
            throw InvalidConfiguration();
        }
    }
    else {    
        m_stopList = 0;
    }
}

LimaStatusCode LinearTextRepresentationLogger::process(
    AnalysisContent& analysis) const {

    DUMPERLOGINIT;
    // get metadata
    auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
    if (metadata == 0) {
        LERROR << "no LinguisticMetaData ! abort";
        return MISSING_DATA;
    }
    // get the analysis graph
    auto anaGraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph"));
    if (anaGraph == 0) {
        LERROR << "no AnalysisGraph ! abort";
        return MISSING_DATA;
    }
    // get sentence boundaries
    auto sb = std::dynamic_pointer_cast<SegmentationData>(analysis.getData("SentenceBoundaries"));
    if (sb == 0) {
      LDEBUG << "LinearTextRepresentationDumper::process: no SentenceBounds available: ignored";
      // sentence bounds ignored: null pointer passed to LTRTextBuilder will be handled there
    }
    // build LTRText
    LTR_Text textRep;
    LTRTextBuilder builder(m_language, m_stopList);
    builder.buildLTRTextFrom(
        *(anaGraph->getGraph()),
        sb.get(),
        anaGraph->firstVertex(),
        anaGraph->lastVertex(),
        &textRep,
        metadata->getStartOffset());

    // write LTR_Text
    string textFileName = metadata->getMetaData("FileName");
    string outputFile = textFileName + m_outputSuffix;
    ofstream out(outputFile.c_str(), std::ofstream::binary);
    if (!out.good()) {
        throw runtime_error("can't open file " + outputFile);
    }
    textRep.binaryWriteOn(out);
    out.flush();
    out.close();
    return SUCCESS_ID;
}


} // AnalysisDumpers
} // LinguisticProcessing
} // Lima
