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
 * @file       EventTemplateMerging.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Sep 26 2011
 * copyright   Copyright (C) 2011 by CEA - LIST
 *
 ***********************************************************************/

#include "EventTemplateMerging.h"
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
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

//----------------------------------------------------------------------
// factory for process unit
SimpleFactory<MediaProcessUnit,EventTemplateMerging> EventTemplateMerging(EVENTTEMPLATEMERGING_CLASSID);

EventTemplateMerging::EventTemplateMerging():
m_templateDefinition(0),
m_mandatoryElements()
{
}

EventTemplateMerging::~EventTemplateMerging()
{
}

void EventTemplateMerging::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  MediaId language=manager->getInitializationParameters().media;
  
  try {
    std::string templateResource=unitConfiguration.getParamsValueAtKey("eventTemplate");
    AbstractResource* res=LinguisticResources::single().getResource(language,templateResource);
    if (res) {
      m_templateDefinition=static_cast<EventTemplateDefinitionResource*>(res);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LOGINIT("LP::EventAnalysis");
    LERROR << "EventTemplateMerging: Missing 'eventTemplate' parameter in EventTemplateMerging definition" << LENDL;
    //throw InvalidConfiguration;
  }
  catch (std::exception& ) {
    LOGINIT("LP::EventAnalysis");
    LERROR << "EventTemplateMerging: Missing ressource for 'eventTemplate' parameter" << LENDL;
    //throw InvalidConfiguration;
  }
  
  try {
    m_mandatoryElements=unitConfiguration.getListsValueAtKey("mandatoryElements");
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& ) {
    //LOGINIT("LP::EventAnalysis");
    //LDEBUG << "EventTemplateMerging: no list parameter 'mandatoryElements' in EventTemplateMerging definition" << LENDL;
  }
}

LimaStatusCode EventTemplateMerging::process(AnalysisContent& analysis) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplateMerging process" << LENDL;
  TimeUtils::updateCurrentTime();

  LimaStatusCode returnCode=SUCCESS_ID;
  returnCode=mergeEventTemplates(analysis);

  TimeUtils::logElapsedTime("EventTemplateMerging");
  return returnCode;
}

LimaStatusCode EventTemplateMerging::mergeEventTemplates(AnalysisContent& analysis) const
{
  // ad hoc strategy for merging event templates
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplateMerging mergeEventTemplates" << LENDL;
  TimeUtils::updateCurrentTime();
  
  // get EventTemplateData
  EventTemplateData* eventData=static_cast<EventTemplateData*>(analysis.getData("EventTemplateData"));
  if (eventData==0) {
    LERROR << "EventTemplateMerging::mergeEventTemplates: missing data 'EventTemplateData'" << LENDL;
    return MISSING_DATA;
  }

  // merge templates according to their positions and positions of intermediate entities
  
  /*
  // gather entities by position
  vector<pair<uint64_t, Common::MediaticData::EntityType> > entities;
  getEntityPositions();
  // gather template elements by position
  vector<pair<uint64_t, pair<std::string, uint64_t> > > templateElements;

  // get template elements
  uint64_t numTemplate=0;
  for (EventTemplateData::iterator it=eventData->begin(),it_end=eventData->end();it!=it_end;it++)
  {
    map<string,EventTemplateElement>& elts=(*it).getTemplateElements();
    for (map<string,EventTemplateElement>::const_iterator fill=elts.begin(),fill_end=elts.end(); fill!=fill_end; fill++) 
    {
      templateElements.push_back(make_pair((*fill).second.getPosition(),make_pair(first,numTemplate)));
    }
    numTemplate++;
  }
  */
  
  std::set<uint64_t> toRemove;
  uint64_t numTemplate=0;
  for (EventTemplateData::iterator it1=eventData->begin(),it_end=eventData->end();it1!=it_end;it1++)
  {
    map<string,EventTemplateElement>& templateElements1=(*it1).getTemplateElements();
    // begin comparison with next template
    EventTemplateData::iterator it2=it1; 
    uint64_t numOtherTemplate=numTemplate; // only used for debug messages
    for (it2++; it2!=it_end; it2++)
    {
      numOtherTemplate++;
      map<string,EventTemplateElement>& templateElements2=(*it2).getTemplateElements();
      if (templateElements2.empty()) {
        continue;
      }
      bool compatibleTemplates=true;
      map<string,pair<string, int> > matchingElements;
      for (map<string,EventTemplateElement>::const_iterator fill=templateElements2.begin(),
        fill_end=templateElements2.end(); fill!=fill_end; fill++) 
      {
        const std::string& role=(*fill).first;
        const EventTemplateElement& elt=(*fill).second;
        
        map<string,EventTemplateElement>::const_iterator found=templateElements1.find(role);
        if (found!=templateElements1.end()) {
          // same role, check value
          if (elt.isSimilar((*found).second)) {
            matchingElements[role]=make_pair(role,0);
          }
          else {
            // different values, assume templates of different elements
            LDEBUG << "EventTemplateMerging::mergeEventTemplates : templates " 
            << (numTemplate+1) << " and " << (numOtherTemplate+1) 
            << " have incompatible values for role "<< role << LENDL;
            compatibleTemplates=false;
          }
        }
        else { // try to find mapping through constraint
          for (map<string,EventTemplateElement>::const_iterator e=templateElements1.begin(),
            e_end=templateElements1.end(); e!=e_end; e++) {
            int mapping=m_templateDefinition->existsMapping((*e).first,role);
            if (mapping!=0) {
              // same role, check value
              if (elt.isSimilar((*e).second)) {
                matchingElements[(*e).first]=make_pair(role,mapping);
              }
              else {
                // different values, assume templates of different elements
                LDEBUG << "EventTemplateMerging::mergeEventTemplates : templates " 
                       << (numTemplate+1) << " and " << (numOtherTemplate+1) 
                       << " have incompatible values for matching roles "<< role << " and " << (*e).first << LENDL;
                compatibleTemplates=false;
              }
              // do not search for another match in mapping, take first
              break;
            }
          }
        }
      }
 
      if (compatibleTemplates) {
        // perform the merge : add elements from first template in second
        LDEBUG << "EventTemplateMerging::mergeEventTemplates : template " 
               << (numTemplate+1) << " compatible with template "
               << (numOtherTemplate+1) << " : perform the merge" << LENDL;
        for (map<string,EventTemplateElement>::const_iterator elt1=templateElements1.begin(),elt1_end=templateElements1.end();
        elt1!=elt1_end; elt1++) 
        {
          map<string,pair<string,int> >::const_iterator match=matchingElements.find((*elt1).first);
          if (match==matchingElements.end()) {
            // no matching: add element from first template in second one
            LDEBUG << "EventTemplateMerging::mergeEventTemplates : add element "<< (*elt1).first << LENDL;
            templateElements2.insert(*elt1);
          }
          else {
            // matching exists : check value
            if ((*match).second.second==-1) {
              // mapping from 1 to 2 : remove role from 2 and insert role from 1
              LDEBUG << "EventTemplateMerging::mergeEventTemplates : replace element "<< (*match).second.first << " with " << (*elt1).first << LENDL;
              templateElements2.insert(*elt1);
              templateElements2.erase((*match).second.first);
            }
            else if ((*match).second.second==1) {
              LDEBUG << "EventTemplateMerging::mergeEventTemplates : mapping 1: ignore "<< (*elt1).first << " (keep " << (*match).second.first << ")" << LENDL;
              // mapping from 2 to 1 : do not insert element in second template, keep role
            }
            else {
              // else (0) : same role : do not insert element in second template
              LDEBUG << "EventTemplateMerging::mergeEventTemplates : mapping 0: ignore "<< (*elt1).first << LENDL;
            }
          }
        }
        // plan to remove first template
        toRemove.insert(numTemplate);
        // do not break, merge with all following compatible events
        //break;
      }
      else{
        LDEBUG << "EventTemplateMerging::mergeEventTemplates : template " 
        << (numTemplate+1) << " not compatible with template "
        << (numOtherTemplate+1) << LENDL;
      }
    }
    numTemplate++;
  }

  // remove merged templates : get template number in reverse
  for (set<uint64_t>::reverse_iterator it=toRemove.rbegin(),
    it_end=toRemove.rend(); it!=it_end; it++) 
  {
    LDEBUG << "Erase template " << (*it)+1 << LENDL;
    eventData->erase(eventData->begin()+(*it));
  }

  // additional step if mandatory elements are specified : keep only templates with mandatory elements
  if (! m_mandatoryElements.empty()) {
    EventTemplateData::iterator it=eventData->begin();
    uint64_t n=1; // only for debug messages
    while (it!=eventData->end()) {
      bool toRemove=false;
      const std::map<std::string,EventTemplateElement>& elements=(*it).getTemplateElements();
      for (deque<string>::const_iterator m=m_mandatoryElements.begin(),m_end=m_mandatoryElements.end();m!=m_end;m++) {
        if (elements.find(*m)==elements.end()) {
          toRemove=true;
          break;
        }
      }
      if (toRemove) {
        LDEBUG << "Erase template " << n << " because it does not contain mandatory elements" <<  LENDL;
        it=eventData->erase(it);
      }
      else {
        it++;
        n++;
      }
    }
  }
  
  return SUCCESS_ID;
}

} // end namespace
} // end namespace
} // end namespace
