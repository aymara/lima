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
    LERROR << "EventTemplateFilling: Missing ressource for 'eventTemplate' parameter";
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
