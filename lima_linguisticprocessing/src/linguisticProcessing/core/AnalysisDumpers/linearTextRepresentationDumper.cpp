// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** =========================================================================
    @file       linearTextRepresentationDumper.cpp

    @version    $Id$
    @date       created       jan 6, 2005
    @date       last revised  jan 4, 2011

    @author     Olivier Ferret
    @brief      dump the list of all the BoWToken of a text according to
                their position

    Copyright (C) 2005-2011 by CEA LIST

    ========================================================================= */

#include "linearTextRepresentationDumper.h"

// #include "linguisticProcessing/core/LinguisticProcessors/HandlerStreamBuf.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"
#include "common/LimaCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

#include <iostream>
#include <fstream>

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

SimpleFactory<MediaProcessUnit, LinearTextRepresentationDumper> ltrDumperFactory(LTRDUMPER_CLASSID);

/**
  * @class  LinearTextRepresentationDumper
  * @brief  dump the list of all the BoWToken of a text according to
            their position
  */

// -----------------------------------------------------------------------------
// -- process unit management
// -----------------------------------------------------------------------------

void LinearTextRepresentationDumper::init(
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

    if (useStopList) {
        try {
            string stoplist = unitConfiguration.getParamsValueAtKey("stopList");
            m_stopList = std::dynamic_pointer_cast<StopList>(LinguisticResources::single().getResource(m_language, stoplist));
        }
        catch (NoSuchParam& ) {
            LERROR << "LinearTextRepresentationDumper::init:  No param 'stopList' in LinearTextRepresentationDumper configuration group for language " << m_language;
            throw InvalidConfiguration();
        }
    }
    else {
        m_stopList = 0;
    }
    try {
      m_handler = unitConfiguration.getParamsValueAtKey("handler");
    }
    catch (NoSuchParam& ) {
      DUMPERLOGINIT;
      LERROR << "LinearTextRepresentationDumper::init: Missing parameter handler in LinearTextRepresentationDumper configuration";
      throw InvalidConfiguration();
    }    
}

LimaStatusCode LinearTextRepresentationDumper::process(
    AnalysisContent& analysis) const {

    DUMPERLOGINIT;
    // get metadata    
    auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
    if (metadata == 0) {
        LERROR << "LinearTextRepresentationDumper::process: no LinguisticMetaData ! abort";
        return MISSING_DATA;
    }
    // get the analysis graph    
    auto anaGraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph"));
    if (anaGraph == 0) {
        LERROR << "LinearTextRepresentationDumper::process: no AnalysisGraph ! abort";
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
    LDEBUG << "handler will be: " << m_handler;
//     MediaId langid = static_cast<const  Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(metadata->getMetaData("Lang"))).getMedia();
    auto h = std::dynamic_pointer_cast<AnalysisHandlerContainer>(analysis.getData("AnalysisHandlerContainer"));
    AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(h->getHandler(m_handler));
    if (handler == 0) {
      LERROR << "LinearTextRepresentationDumper::process: handler " << m_handler << " has not been given to the core client";
      return MISSING_DATA;
    }    
    handler->startAnalysis();
    HandlerStreamBuf hsb(handler);
    ostream out(&hsb);
    LDEBUG << textRep;
    textRep.binaryWriteOn(out);
    out.flush();
    handler->endAnalysis();
    return SUCCESS_ID;
}


} // AnalysisDumpers
} // LinguisticProcessing
} // Lima
