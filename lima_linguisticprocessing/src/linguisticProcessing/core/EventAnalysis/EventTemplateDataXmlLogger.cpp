// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventTemplateDataXmlLogger.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Mar 24 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 *
 ***********************************************************************/

#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "EventTemplateDataXmlLogger.h"

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace Lima::Common;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace boost;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

SimpleFactory<MediaProcessUnit,EventTemplateDataXmlLogger>
EventTemplateDataXmlLoggerFactory(EVENTTEMPLATEDATAXMLLOGGER_CLASSID);

//***********************************************************************
// constructors and destructors
EventTemplateDataXmlLogger::EventTemplateDataXmlLogger():
AbstractTextualAnalysisDumper(),
m_eventData("EventTemplateData")
{
}

EventTemplateDataXmlLogger::~EventTemplateDataXmlLogger()
{
}

//***********************************************************************
void EventTemplateDataXmlLogger::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                          Manager* manager)

{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplateDataXmlLogger::init";

  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  try {
    m_eventData=unitConfiguration.getParamsValueAtKey("eventTemplateData");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LDEBUG << "EventTemplateDataXmlLogger: no parameter 'eventTemplateData', use default ('"<<m_eventData << "')";
    // not an error, keep default
  }
}

LimaStatusCode EventTemplateDataXmlLogger::process(AnalysisContent& analysis) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplateDataXmlLogger::process";
  TimeUtils::updateCurrentTime();

  // initialize output
  auto dstream=AbstractTextualAnalysisDumper::initialize(analysis);
  ostream& out=dstream->out();

  const auto annotationData = std::dynamic_pointer_cast< AnnotationData >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "no annotation graph available !";
    return MISSING_DATA;
  }

  if (! m_eventData.empty()) {
    auto data = analysis.getData(m_eventData);
    if (data!=0) {
      // see if the data is of type Events
      auto eventData = std::dynamic_pointer_cast<const EventTemplateData>(data);
      if (eventData==0) {
        LOGINIT("LP::EventAnalysis");
        LERROR << "data '" << m_eventData << "' is neither of type EventData nor Events";
        return MISSING_DATA;
      }
      else {
        outputEventData(out, eventData.get(), annotationData.get());
      }
    }
    else {
      LOGINIT("LP::EventAnalysis");
      LERROR << "no data of name " << m_eventData;
    }
  }
  TimeUtils::logElapsedTime("EventTemplateDataXmlLogger");
  return SUCCESS_ID;
}

void EventTemplateDataXmlLogger::outputEventData(std::ostream& out,
                                                 const EventTemplateData* eventData,
                                                 const AnnotationData* annotationData) const
{
  int i=0;
  out << "<events>" << endl;
  for (std::vector<EventTemplate>::const_iterator it= eventData->begin(); it!= eventData->end();it++)
  {
    const TemplateElements& templateElements=(*it).getTemplateElements();
    if (! templateElements.empty()) {
      i++;
      out << "  <event id=\"" << i << "\""
          << " w=\"" << (*it).getWeight() << "\""
          << " main=\"" << (*it).isMainEvent() << "\""
          << " type=\"" << (*it).getType() << "\">"
          << endl;
      int j=0;
      out << "    <entities>" << endl;
      for(map<string,EventTemplateElement>::const_iterator it1= templateElements.begin(); it1!= templateElements.end();it1++)
      {
        string typeName=Common::Misc::limastring2utf8stdstring(MediaticData::MediaticData::single().getEntityName((*it1).second.getType()));
        out << "      <entity role=\"" << (*it1).first << "\""
            << " type=\"" << typeName << "\">"
            << endl;
        const LinguisticAnalysisStructure::AnalysisGraph* graph=(*it1).second.getGraph();
        LinguisticGraphVertex v=(*it1).second.getVertex();
        outputEntity(out, graph, v,annotationData);
        out << "      </entity>" << endl;
        j++;
      }
      out << "    </entities>" << endl;
    out << "  </event>" << endl;
    }
  }
  out << "</events>" << endl;
}


void EventTemplateDataXmlLogger::outputEntity(std::ostream& out,
                                              const LinguisticAnalysisStructure::AnalysisGraph* graph,
                                              LinguisticGraphVertex v,
                                              const AnnotationData* annotationData)  const
{
  LinguisticAnalysisStructure::Token* token=get(vertex_token, *(graph->getGraph()), v);
  if (token==0) {
        LOGINIT("LP::EventAnalysis");    LWARN << "EventTemplateDataXmlLogger: no token for vertex " << v;
    return;
  }
  out << "        <entityOccurrence"
      << " pos=\"" << token->position() << "\""
      << " len=\"" << token->length() << "\"";

  string str(""),norm("");
  str=xmlString(Common::Misc::limastring2utf8stdstring(token->stringForm()));

  // check if vertex corresponds to a specific entity found
  std::set< AnnotationGraphVertex > matches = annotationData->matches(graph->getGraphId(),v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
  it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      const SpecificEntityAnnotation* se =
      annotationData->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
      pointerValue<SpecificEntityAnnotation>();

      const Automaton::EntityFeatures& features=se->getFeatures();
      for (Automaton::EntityFeatures::const_iterator
        featureItr=features.begin(),features_end=features.end();
      featureItr!=features_end; featureItr++)
      {
        if (featureItr->getName()=="value") {
          norm=xmlString(featureItr->getValueString());
        }
      }
    }
  }

  if (! str.empty()) {
    out << " string=\"" << str << "\"";
    out << " norm=\"" << norm << "\"";
    out << ">" << str ;
  }
  out << "</entityOccurrence>" << endl;
}

std::string EventTemplateDataXmlLogger::xmlString(const std::string& inputStr) const
{
  // protect XML entities
  std::string str(inputStr);
  boost::replace_all(str,"&", "&amp;");
  boost::replace_all(str,"<", "&lt;");
  boost::replace_all(str,">", "&gt;");
  boost::replace_all(str,"\"", "&quot;");
  boost::replace_all(str,"\n", "\n");
  return str;
}


} // end namespace
} // end namespace
} // end namespace
