// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventTemplateDataDumper.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Sep 19 2011
 * copyright   Copyright (C) 2011 by CEA - LIST
 *
 ***********************************************************************/

#include "EventTemplateDataDumper.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "Events.h"
#include "Entity.h"

using namespace std;
using namespace Lima::Common;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace boost;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

//***********************************************************************
SimpleFactory<MediaProcessUnit,EventTemplateDataDumper>
EventTemplateDataDumperFactory(EVENTTEMPLATEDATADUMPER_CLASSID);

EventTemplateDataDumper::EventTemplateDataDumper() :
AbstractTextualAnalysisDumper(),
m_eventData("EventTemplateData")
{
}

EventTemplateDataDumper::~EventTemplateDataDumper() {
}

//***********************************************************************
void EventTemplateDataDumper::
init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
     Manager* manager)

{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplateDataDumper::init";

  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  try {
    m_eventData=unitConfiguration.getParamsValueAtKey("eventTemplateData");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LDEBUG << "EventTemplateDataDumper: no parameter 'eventTemplateData', use default ('"<<m_eventData << "')";
    // not an error, keep default
  }
}

LimaStatusCode EventTemplateDataDumper::process(AnalysisContent& analysis) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplateDataDumper::process";
  TimeUtils::updateCurrentTime();

  // initialize output
  auto dstream=AbstractTextualAnalysisDumper::initialize(analysis);
  ostream& out=dstream->out();

  const AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "no annotation graph available !";
    return MISSING_DATA;
  }

  if (! m_eventData.empty()) {
    const AnalysisData* data =analysis.getData(m_eventData);
    if (data!=0) {
      // see if the data is of type Events
      const EventTemplateData* eventData=dynamic_cast<const EventTemplateData*>(data);
      if (eventData==0) {
        LOGINIT("LP::EventAnalysis");
        LERROR << "data '" << m_eventData << "' is neither of type EventData nor Events";
        return MISSING_DATA;
      }
      else {
        Events *events=eventData->convertToEvents(annotationData);
        events->write(out);
      }
    }
    else {
      LOGINIT("LP::EventAnalysis");
      LERROR << "no data of name " << m_eventData;
    }
  }

  TimeUtils::logElapsedTime("EventTemplateDataDumper");
  return SUCCESS_ID;
}

} // end namespace
} // end namespace
} // end namespace
