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
 * @file       EventXmlLogger.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Mar 24 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "EventXmlLogger.h"
#include "EventTemplateData.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace Lima::Common;
using namespace Lima::Common::AnnotationGraphs;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

SimpleFactory<MediaProcessUnit,EventXmlLogger> 
EventXmlLoggerFactory(EVENTXMLLOGGER_CLASSID);

//***********************************************************************
// constructors and destructors
EventXmlLogger::EventXmlLogger():
AbstractTextualAnalysisDumper(),
m_eventData("EventData")
{
}

EventXmlLogger::~EventXmlLogger() 
{
}

//***********************************************************************
void EventXmlLogger::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                          Manager* manager)
  
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventXmlLogger::init";

  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  try {
    m_eventData=unitConfiguration.getParamsValueAtKey("eventData");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // do nothing, keep default
}
  
LimaStatusCode EventXmlLogger::process(AnalysisContent& analysis) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventXmlLogger::process";
  TimeUtils::updateCurrentTime();

  // initialize output
  DumperStream* dstream=AbstractTextualAnalysisDumper::initialize(analysis);
  ostream& out=dstream->out();

  if (! m_eventData.empty()) {
    const AnalysisData* data =analysis.getData(m_eventData);
    if (data!=0) {
      // see if the data is of type Events
      const Events* events=dynamic_cast<const Events*>(data);
      if (events!=0) {
        LDEBUG << "data '" << m_eventData << "' is of type Events";
        outputEvents(out, events);
      }
      else {
        // see if the data is of type EventData
        const EventData* eventData=dynamic_cast<const EventData*>(data);
        if (eventData!=0) {
          LDEBUG << "data '" << m_eventData << "' is of type EventData";
          outputEventData(out, eventData);
        }
        else {
          // see if data is of type EventTemplateData
          const EventTemplateData* eventTemplateData=dynamic_cast<const EventTemplateData*>(data);
          if (eventTemplateData!=0) {
            LDEBUG << "data '" << m_eventData << "' is of type EventTemplateData";
            // output it converted to Events (need annotation data)
            const AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
            if (annotationData==0)
            {
              LERROR << "no annotation graph available !";
              return MISSING_DATA;
            }
            
            outputEvents(out, eventTemplateData->convertToEvents(annotationData));
          }
          else {
            LOGINIT("LP::EventAnalysis");
          LERROR << "data '" << m_eventData << "' is neither of type EventData nor Events";
          }
        }
      }
    }
    else {
      LOGINIT("LP::EventAnalysis");
      LERROR << "no data of name " << m_eventData;
    }
  }
  delete dstream;
  TimeUtils::logElapsedTime("EventXmlLogger");
  return SUCCESS_ID;
}

void EventXmlLogger::outputEvents(std::ostream& out,
                                  const Events* eventData) const
{
  int i=0;
  out << "<events>" << endl;
  for (std::vector<Event*>::const_iterator iT= eventData->begin(); iT!= eventData->end();iT++)
  {
    i++;
    out << "<event id=\"" << i << "\"" 
        << " w=\"" << (*iT)->get_weight() << "\""
        << " main=\"" << (*iT)->getMain() << "\">" << endl;
    out << "<segments>" << endl;
    int j=0;
    for(std::vector<EventParagraph*>::const_iterator iT1= (*iT)->begin(); iT1!= (*iT)->end();iT1++)
    {
      out << "  <paragraph id=\"" << j << "\" pos=\"" << (*iT1)->getPosition() << "\" len=\"" << (*iT1)->getLength() <<"\">" << endl;
      std::map<Common::MediaticData::EntityType,std::vector<Entity *> > otherEntities=(*iT1)->getOtherEntities();
      out << "    <entities>" << endl;
      for(std::map<Common::MediaticData::EntityType,std::vector<Entity *> >::const_iterator iT2= otherEntities.begin(); iT2!= otherEntities.end();iT2++)
      {
        //out << "      <entity type=\"" << (*iT2).first << "\">" << endl;
        std::string entityTypeName=Common::Misc::limastring2utf8stdstring(MediaticData::MediaticData::single().getEntityName((*iT2).first));
        out << "      <entity type=\"" << entityTypeName << "\">" << endl;
        for(std::vector<Entity *>::const_iterator iT3=(*iT2).second.begin();iT3!=(*iT2).second.end();iT3++)
        {
          outputEntity(out,**iT3);
        }
        out << "      </entity>" << endl;
      }
      std::pair<Common::MediaticData::EntityType,std::vector<Entity *> > eventEntities=(*iT1)->getEventEntities();
      if (! eventEntities.second.empty()) {
        out << "      <entity type=\"" << eventEntities.first << "\">" << endl;
        for(std::vector<Entity *>::const_iterator iT3=eventEntities.second.begin();iT3!=eventEntities.second.end();iT3++)
        {
          outputEntity(out,**iT3);
        }
        out << "      </entity>" << endl;
      }
      out << "    </entities>" << endl;
      out << "  </paragraph>" << endl;
      j++;
    }
    out << "</segments>" << endl;
    out << "</event>" << endl;
  }
  out << "</events>" << endl;
}

void EventXmlLogger::outputEventData(std::ostream& out,
                                     const EventData* eventData) const
{
  out << "<events>" << endl;
  out << "<event id=\"" << 1 << "\"" 
      << " w=\"" << 1 << "\""
      << " main=\"true\">" << endl;
  out << "  <entities>" << endl;
  const map<Common::MediaticData::EntityType,vector<Entity> >& entities=eventData->getEntities();
  for (map<Common::MediaticData::EntityType,vector<Entity> >::const_iterator 
    it=entities.begin(),it_end=entities.end();it!=it_end;it++) {
    out << "    <entity type=\"" << Misc::limastring2utf8stdstring(MediaticData::MediaticData::single().getEntityName((*it).first)) << "\">" << endl;
    for(std::vector<Entity>::const_iterator e=(*it).second.begin(),e_end=(*it).second.end();e!=e_end;e++)
    {
      outputEntity(out,*e);
    }
    out << "    </entity>" << endl;
  }
  out << "  </entities>" << endl;
      
  out << "</event>" << endl;
  out << "</events>" << endl;
}

void EventXmlLogger::outputEntity(std::ostream& out, const Entity& e)  const
{
  out << "        <entityOccurrence"
      << " pos=\"" << e.getPosition() << "\""
      << " len=\"" <<e.getLength() << "\"";
  string str(""),norm(""),role("");
  const Automaton::EntityFeatures& features=e.getFeatures();
  for (Automaton::EntityFeatures::const_iterator 
    featureItr=features.begin(),features_end=features.end();
  featureItr!=features_end; featureItr++)
  {
    if (featureItr->getName()=="value") {
      str=xmlString(featureItr->getValueString());
      //protect xml entities
    }
    else if (featureItr->getName()=="role") {
      role=xmlString(featureItr->getValueString());
    }
    else {
      norm=xmlString(featureItr->getValueString());
    }
  }
  if (! role.empty()) {
    out << " role=\"" << role << "\"";
  }
  if (! str.empty()) {
    out << " string=\"" << str << "\"";
    out << " norm=\"" << str << "\"";
    if (e.getMain()) {
      out << " main=\"true\"";
    }
    if (! norm.empty()) {
      out << ">" << norm ;
    }
    else {
      out << ">" << str ;
    }
  }
  else {
    if (e.getMain()) {
      out << " main=\"true\"";
    }
    out << ">";
  }
  out << "</entityOccurrence>" << endl;
}

std::string EventXmlLogger::xmlString(const std::string& inputStr) const
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
