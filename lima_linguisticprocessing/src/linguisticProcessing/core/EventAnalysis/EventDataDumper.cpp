// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventDataDumper.cpp
 * @author     Olivier Mesnard (olivier.mesnard@cea.fr)
 * @date       Mon Sep 19 2011
 * copyright   Copyright (C) 2011 by CEA - LIST
 *
 ***********************************************************************/
#include "EventDataDumper.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "Events.h"
#include "Entity.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"

using namespace std;
using namespace Lima::Common;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace boost;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

//***********************************************************************
SimpleFactory<MediaProcessUnit,EventDataDumper>
EventDataDumperFactory(EVENTDATADUMPER_CLASSID);

EventDataDumper::EventDataDumper() :
AbstractTextualAnalysisDumper(),
// key to access EventData in AnalysisContent
m_eventData("EventData"),
m_segmentationData("SegmentationData")
{
}

EventDataDumper::~EventDataDumper() {
}

//***********************************************************************
void EventDataDumper::
init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
     Manager* manager)

{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventDataDumper::init";

  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  try {
    m_eventData=unitConfiguration.getParamsValueAtKey("eventData");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LDEBUG << "EventDataDumper: no parameter 'eventData', use default ('"<<m_eventData << "')";
    // not an error, keep default
  }
  try {
    m_segmentationData=unitConfiguration.getParamsValueAtKey("segmentationData");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LDEBUG << "EventDataDumper: no parameter 'segmentationData', use default ('"<<m_segmentationData << "')";
    // not an error, keep default
  }
}

LimaStatusCode EventDataDumper::process(AnalysisContent& analysis) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventDataDumper::process";
  TimeUtils::updateCurrentTime();

  // initialize output
  auto dstream = AbstractTextualAnalysisDumper::initialize(analysis);
  ostream& out=dstream->out();
  if ((! m_eventData.empty())
    &&(! m_segmentationData.empty())) {
    AnalysisData* data =analysis.getData(m_eventData);
    if (data!=0) {
      // see if the data is of type EventData
      const EventData* eventData=dynamic_cast<const EventData*>(data);

      if (eventData!=0) {
        LOGINIT("LP::EventAnalysis");
        LDEBUG << "EventDataDumper::process: data '" << m_eventData << "' is of type EventData";
        // get segmentation
        const AnalysisData* data2 =analysis.getData(m_segmentationData);
        const Lima::LinguisticProcessing::SegmentationData* segmData=0;
        if(data2==0) {
    LERROR << "EventDataDumper::process: error: no SegmentationData!!";
    return MISSING_DATA;
  }
        segmData = dynamic_cast<const Lima::LinguisticProcessing::SegmentationData*>(data2);
  if (segmData==0) {
    LERROR << "EventDataDumper::process: data "<< data2 <<" is not an object of class SegmentationData";
    return MISSING_DATA;
  }
  eventData->convertToEvents(segmData);
  Events *events=eventData->convertToEvents(segmData);
  LDEBUG << "EventDataDumper::process: events->write(out)...";
  events->write(out);
      }
      else {
        // see if the data is of type Events
        const Events* events=dynamic_cast<const Events*>(data);
        if (events!=0) {
          LDEBUG << "data '" << m_eventData << "' is of type Events";
          events->write(out);
        }
        else {
          LERROR << "data '" << m_eventData << "' is neither of type EventData nor Events";
          return MISSING_DATA;
        }
      }
    }
    else {
      LOGINIT("LP::EventAnalysis");
      LERROR << "no data of name " << m_eventData;
      return MISSING_DATA;
    }
  }
  else {
    LOGINIT("LP::EventAnalysis");
    LERROR << "no key to access eventData or segmentationData! ";
    return MISSING_DATA;
  }


  TimeUtils::logElapsedTime("EventDataDumper");
  return SUCCESS_ID;
}

} // end namespace
} // end namespace
} // end namespace
