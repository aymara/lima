// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
m_mandatoryElements(),
m_maxCharCompatibleEvents(200)
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
    LERROR << "TemplateMerging: Missing 'eventTemplate' parameter in EventTemplateMerging definition";
    //throw InvalidConfiguration;
  }
  catch (std::exception& e) {
    LOGINIT("LP::EventAnalysis");
    LERROR << "TemplateMerging: Missing ressource for 'eventTemplate' parameter" << e.what();
    //throw InvalidConfiguration;
  }

  try {
    m_mandatoryElements=unitConfiguration.getListsValueAtKey("mandatoryElements");
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& ) { } // optional

  try {
    std::string s=unitConfiguration.getParamsValueAtKey("maxCharCompatibleEvents");
    m_maxCharCompatibleEvents=std::stoul(s);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) { } // optional: keep default value

}

LimaStatusCode EventTemplateMerging::process(AnalysisContent& analysis) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplateMerging process";
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
  TimeUtils::updateCurrentTime();

  // get EventTemplateData
  EventTemplateData* eventData=static_cast<EventTemplateData*>(analysis.getData("EventTemplateData"));
  if (eventData==0) {
    LERROR << "TemplateMerging:missing data 'EventTemplateData'";
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

  LDEBUG << "TemplateMerging: merge templates of type" << m_templateDefinition->getName();

  std::set<uint64_t> toRemove;
  uint64_t numTemplate=0;
  for (EventTemplateData::iterator it1=eventData->begin(),it_end=eventData->end();it1!=it_end;it1++,numTemplate++)
  {
    if ((*it1).getType()!=m_templateDefinition->getName()) {
      continue;
    }

    // do we keep merging with other templates if this one is to be removed ?
    if (toRemove.find(numTemplate)!=toRemove.end()) {
      continue;
    }

    // begin comparison with next template
    EventTemplateData::iterator it2=it1;
    uint64_t numOtherTemplate=numTemplate; // only used for debug messages
    for (it2++,numOtherTemplate++; it2!=it_end; it2++,numOtherTemplate++)
    {
      if ((*it2).getType()!=m_templateDefinition->getName()) {
        continue;
      }

      if (toRemove.find(numOtherTemplate)!=toRemove.end()) {
        continue;
      }

      map<string,EventTemplateElement>& templateElements2=(*it2).getTemplateElements();
      if (templateElements2.empty()) {
        continue;
      }

      LDEBUG << "TemplateMerging: compare templates"
      << numTemplate+1 << "and" << numOtherTemplate+1 << ":\n"
      << *it1 << "\n" << *it2;

      unsigned int templateToRemove(0);
      // single function to test compatibility and perform the merge, returns true if the merge is successful
      // in this case, the templateToRemove contains the index of the event template that has been merged and
      // can be removed
      if (mergeTemplates(*it1,*it2,numTemplate,numOtherTemplate,templateToRemove)) {
        toRemove.insert(templateToRemove);
        // stop if this template has been merged into another
        if (templateToRemove==numTemplate) {
          break;
        }
      }
//       if (compatibleTemplates(*it1,*it2,numTemplate,numOtherTemplate,matchingElements)) {
//         // plan to remove first template
//         uint64_t templateToRemove=mergeTemplates(*it1,*it2,numTemplate,numOtherTemplate,matchingElements);
//         toRemove.insert(templateToRemove);
//         // do not break, merge with all following compatible events
//         //break;
//       }
//       else{
//         //LDEBUG << "TemplateMerging: template"
//         //<< (numTemplate+1) << " not compatible with template"
//         //<< (numOtherTemplate+1);
//       }
    }
  }

  // remove merged templates : get template number in reverse
  for (set<uint64_t>::reverse_iterator it=toRemove.rbegin(),
    it_end=toRemove.rend(); it!=it_end; it++)
  {
    LDEBUG << "Erase template" << (*it)+1;
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
        LDEBUG << "Erase template" << n << "because it does not contain mandatory elements" ;
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

bool EventTemplateMerging::compatibleTemplates(const EventTemplate& e1, const EventTemplate& e2,
                                               unsigned int numTemplate, unsigned int numOtherTemplate,
                                               map<string,pair<string, int> >& matchingElements) const
{
  // numTemplate and numOtherTemplate are only there for debug messages

  LOGINIT("LP::EventAnalysis");
  // possible incompatibility based on positions
  uint64_t posdiff=0;
  // if overlap, posdiff=0
  if (e1.getPosBegin() <= e2.getPosEnd() && e2.getPosBegin() <= e1.getPosEnd()) {
    posdiff=0;
  }
  else if (e1.getPosBegin()>e2.getPosBegin()) {
    posdiff=e1.getPosBegin()-e2.getPosEnd();
  }
  else {
    posdiff=e2.getPosBegin()-e1.getPosEnd();
  }
  if (posdiff >m_maxCharCompatibleEvents) {
    LDEBUG << "TemplateMerging: templates"
    << (numTemplate+1) << "and" << (numOtherTemplate+1)
    << "not compatible: too much apart ("<< posdiff << "chars)";
    return false;
  }

  const map<string,EventTemplateElement>& templateElements1=e1.getTemplateElements();
  const map<string,EventTemplateElement>& templateElements2=e2.getTemplateElements();

  for (map<string,EventTemplateElement>::const_iterator fill=templateElements2.begin(),
    fill_end=templateElements2.end(); fill!=fill_end; fill++)
  {
    const std::string& role=(*fill).first;
    const EventTemplateElement& elt=(*fill).second;

    map<string,EventTemplateElement>::const_iterator found=templateElements1.find(role);
    if (found!=templateElements1.end()) {
      // same role, check value
      if (elt.isSimilar((*found).second)) {
        LDEBUG << "TemplateMerging: templates"
        << (numTemplate+1) << "and" << (numOtherTemplate+1)
        << ":" << role << ": compatible value";
        matchingElements[role]=make_pair(role,0);
      }
      else {
        // different values, assume templates of different elements
        LDEBUG << "TemplateMerging: templates"
        << (numTemplate+1) << "and" << (numOtherTemplate+1)
        << ":" << role << ": incompatible value";
        return false;
      }
    }
    else { // try to find mapping through constraint
      bool mappingFound=false;
      for (map<string,EventTemplateElement>::const_iterator e=templateElements1.begin(),
        e_end=templateElements1.end(); e!=e_end; e++)
      {
        int mapping=m_templateDefinition->existsMapping((*e).first,role);
        if (mapping!=0) {
          mappingFound=true;
          // same role, check value
          if (elt.isSimilar((*e).second)) {
            matchingElements[(*e).first]=make_pair(role,mapping);
          }
          else {
            // different values, assume templates of different elements
            LDEBUG << "TemplateMerging: templates"
            << (numTemplate+1) << "and" << (numOtherTemplate+1)
            << ":" << role << "/" << (*e).first << ": incompatible value";
            return false;
          }
          // do not search for another match in mapping, take first
          break;
        }
      }
      if (! mappingFound) {
        LDEBUG << "TemplateMerging: templates"
        << (numTemplate+1) << "and" << (numOtherTemplate+1)
        << ":" << role << ": not found";
      }
    }
  }
  return true;
}

bool EventTemplateMerging::
mergeTemplates(EventTemplate& e1, EventTemplate& e2,
               unsigned int numTemplate, unsigned int numOtherTemplate,
               unsigned int& templateToRemove) const
{
  // merge isn't always in the same order: we want to keep elements from the template with the smallest span,
  // to avoid having info from all over the place, even if they are equal (e.g. for brat visualization after extraction)
  // sometimes, info is merged from e1 to e2, sometimes from e1 to e2
  // templateToRemove indicates which (between numTemplate and numOtherTemplate) has been merged into the other
  // and can be removed

  // for the moment, reuse the existing compatibleTemplates function
  // (probably not optimal)

  LOGINIT("LP::EventAnalysis");

  // while comparing templates for compatibility, store matching elements:
  // map associating a role1 with a pair (role2,mapping) where the other_role is the role that matches
  // role1 and the mapping: 0 if equality, otherwise oriented mapping: 1 if mapping elt1 -> elt2,
  // -1 if mapping elt2 -> elt1
  map<string,pair<string, int> > matchingElements;

  // by default merge 1 into 2
  // use pointers instead of references to reassign them if merge is the other way
  map<string,EventTemplateElement>* sourceElements=&(e1.getTemplateElements());
  map<string,EventTemplateElement>* targetElements=&(e2.getTemplateElements());


  // heuristic: keep the templates with the most elements
  if (e1.getTemplateElements().size() > e2.getTemplateElements().size()) {
    if (!compatibleTemplates(e2,e1,numOtherTemplate,numTemplate,matchingElements)) {
      return false;
    }
    sourceElements=&(e2.getTemplateElements());
    targetElements=&(e1.getTemplateElements());
    templateToRemove=numOtherTemplate;
  }
  else if (!compatibleTemplates(e1,e2,numTemplate,numOtherTemplate,matchingElements)) {
      return false;
  }
  else {
    LDEBUG << "TemplateMerging: merge" << numTemplate+1 << "into" << numOtherTemplate+1;
    templateToRemove=numTemplate;
  }

  for (const auto& elt1: *sourceElements)
  {
    map<string,pair<string,int> >::const_iterator match=matchingElements.find(elt1.first);
    if (match==matchingElements.end()) {
      // no matching: add element from first template in second one
      LDEBUG << "TemplateMerging: add element" << elt1.first;
      targetElements->insert(elt1);
    }
    else {
      // matching exists : check value
      if ((*match).second.second==-1) {
        // mapping from 1 to 2 : remove role from 2 and insert role from 1
        LDEBUG << "TemplateMerging: replace element" << (*match).second.first << "with" << elt1.first;
        targetElements->insert(elt1);
        targetElements->erase((*match).second.first);
      }
      else if ((*match).second.second==1) {
        LDEBUG << "TemplateMerging: mapping 1: ignore"<< elt1.first << "(keep" << (*match).second.first << ")";
        // mapping from 2 to 1 : do not insert element in second template, keep role
      }
      else {
        // else (0) : same role : do not insert element in second template
        LDEBUG << "TemplateMerging: mapping 0: ignore"<< elt1.first;
      }
    }
  }
  // returns the template to remove
  if (templateToRemove==numTemplate) {
    LDEBUG << "TemplateMerging: keep template" << numOtherTemplate+1 << e2;
  }
  else {
    LDEBUG << "TemplateMerging: keep template"<< numTemplate+1 << e1;
  }
  return true;
}


} // end namespace
} // end namespace
} // end namespace
