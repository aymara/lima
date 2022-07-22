// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventTemplateFilling.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Thu Sep 01 2011
 * copyright   Copyright (C) 2011 by CEA - LIST
 *
 ***********************************************************************/

#include "EventTemplateFilling.h"
#include "EventTemplateData.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/time/traceUtils.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"

using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

//----------------------------------------------------------------------
// factory for process unit
SimpleFactory<MediaProcessUnit,EventTemplateFilling> EventTemplateFilling(EVENTTEMPLATEFILLING_CLASSID);

EventTemplateFilling::EventTemplateFilling():
ApplyRecognizer()
{
}

EventTemplateFilling::~EventTemplateFilling()
{
}

void EventTemplateFilling::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  MediaId language=manager->getInitializationParameters().media;
  
  ApplyRecognizer::init(unitConfiguration,manager);

  try {
    std::string templateResource=unitConfiguration.getParamsValueAtKey("eventTemplate");
    AbstractResource* res=LinguisticResources::single().getResource(language,templateResource);
    if (res) {
      m_templateDefinition=static_cast<EventTemplateDefinitionResource*>(res);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LOGINIT("LP::EventAnalysis");
    LERROR << "EventTemplateFilling: Missing 'eventTemplate' parameter in EventTemplateFilling definition";
    //throw InvalidConfiguration;
  }
  catch (std::exception& e) {
    LOGINIT("LP::EventAnalysis");
    LERROR << "EventTemplateFilling: Missing ressource for 'eventTemplate' parameter for language" << language << ":" << e.what();
    //throw InvalidConfiguration;
  }
}

LimaStatusCode EventTemplateFilling::process(AnalysisContent& analysis) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplateFilling process";
  TimeUtils::updateCurrentTime();

  // create EventTemplateData
  EventTemplateData* eventData=static_cast<EventTemplateData*>(analysis.getData("EventTemplateData"));
  if (eventData==0) {
    LDEBUG << "EventTemplateFilling: create new data 'EventTemplateData'";
    eventData = new EventTemplateData();
    analysis.setData("EventTemplateData", eventData);
  }

  LimaStatusCode returnCode=SUCCESS_ID;
  returnCode=ApplyRecognizer::process(analysis);

  TimeUtils::logElapsedTime("EventTemplateFilling");
  return returnCode;
}

} // end namespace
} // end namespace
} // end namespace
