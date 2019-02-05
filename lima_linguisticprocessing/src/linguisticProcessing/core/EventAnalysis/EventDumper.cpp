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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "EventDumper.h"
// #include "linguisticProcessing/core/LinguisticProcessors/HandlerStreamBuf.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"
#include "Events.h"

#include "common/time/traceUtils.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

#include <fstream>


using namespace std;
using namespace boost;
using namespace boost::tuples;

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::EventAnalysis;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {
  SimpleFactory<MediaProcessUnit,EventDumper> EventDumperFactory(EVENTDUMPER_CLASSID);

EventDumper::EventDumper()
    : MediaProcessUnit()
{
}


EventDumper::~EventDumper()
{
}

void EventDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  m_language=manager->getInitializationParameters().media;
  
  try
  {
    m_file=unitConfiguration.getParamsValueAtKey("traceFile");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    DUMPERLOGINIT;
    LWARN << "No 'traceFile' parameter in unit configuration '"
        << unitConfiguration.getName() << "' ; using tmpFile";
  }
  try
  {
    m_handler=unitConfiguration.getParamsValueAtKey("handler");
  }
  catch (NoSuchParam& )
  {
    DUMPERLOGINIT;
    LERROR << "EventDumper::init:  Missing parameter handler in EventDumper configuration";
    throw InvalidConfiguration();
  }
  
}

LimaStatusCode EventDumper::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  DUMPERLOGINIT;
  
  LDEBUG << "EventDumper::process()... ";

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    LERROR << "EventDumper::process: no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  if (anagraph==0)
  {
    LERROR << "EventDumper::process: no graph 'AnaGraph' available !";
    return MISSING_DATA;
  }
  AnalysisGraph* posgraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph==0)
  {
    LERROR << "EventDumper::process: no graph 'PosGraph' available !";
    return MISSING_DATA;
  }
  
  LDEBUG << "EventDumper::process(): handler will be: " << m_handler;
//  MediaId langid = static_cast<const  Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(metadata->getMetaData("Lang"))).getMedia();
//  AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(LinguisticResources::single().getResource(langid, m_handler));
  AnalysisHandlerContainer* h = static_cast<AnalysisHandlerContainer*>(analysis.getData("AnalysisHandlerContainer"));
  AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(h->getHandler(m_handler));
  LDEBUG << "EventDumper::process(): handler= " << handler;
  if (handler==0)
  {
    LERROR << "EventDumper::process(): handler " << m_handler << " has not been given to the core client";
    return MISSING_DATA;
  }

  HandlerStreamBuf hsb(handler);
  std::ostream out(&hsb);
  LDEBUG << "EventDumper::process(): handler->startAnalysis()...";
  handler->startAnalysis();

  LDEBUG << "EventDumper::process() get EventData...";
  Events* events=static_cast<Events*>(analysis.getData("EventData"));
  if (events==0)
  {
    LWARN << "EventDumper::process(): no Events !";
    events=new Events();
  }
  
  std::string stringEvents = events->toString("P_URI");
  LDEBUG << "EventDumper::process(): stringEvents =" << stringEvents;

  LDEBUG << "EventDumper::process() events->size()=" << events->size();
  LDEBUG << "EventDumper::process(): events->write()...";
  events->write(out);
  out.flush();
  LDEBUG << "EventDumper::process(): handler->endAnalysis()...";
  handler->endAnalysis();
  TimeUtils::logElapsedTime("EventDumper");
  return SUCCESS_ID;
}



}  // closing EventAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
