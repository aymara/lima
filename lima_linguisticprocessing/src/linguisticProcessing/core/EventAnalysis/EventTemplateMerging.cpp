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
m_maxCharCompatibleEvents(200),
m_useSentenceBounds(false),
m_sentenceBoundsData("SentenceBoundaries")
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

  bool optionalMention(false);
  try {
    // by default, the event mention should be mandatory (it is for brat, for instance): 
    // add a parameter to specify if the event mention can be optional
    optionalMention=unitConfiguration.getBooleanParameter("optionalMention");
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& ) { } // optional

  if (! optionalMention) {
    // add event mention as a mandatory element
    m_mandatoryElements.push_back(m_templateDefinition->getMention());
  }
  
  try {
    std::string s=unitConfiguration.getParamsValueAtKey("maxCharCompatibleEvents");
    m_maxCharCompatibleEvents=std::stoul(s);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) { } // optional: keep default value
  try {
    m_useSentenceBounds=
      unitConfiguration.getBooleanParameter("useSentenceBounds");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try
  {
    string sentenceBoundsData=unitConfiguration.getParamsValueAtKey("sentenceBoundsData");
    if (! sentenceBoundsData.empty()) {
      m_sentenceBoundsData=sentenceBoundsData;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    // optional parameter: keep default value
  }

}

LimaStatusCode EventTemplateMerging::process(AnalysisContent& analysis) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplateMerging process";
  TimeUtils::updateCurrentTime();

  // get EventTemplateData
  EventTemplateData* eventData=static_cast<EventTemplateData*>(analysis.getData("EventTemplateData"));
  if (eventData==0) {
    LERROR << "TemplateMerging:missing data 'EventTemplateData'";
    return MISSING_DATA;
  }

  LimaStatusCode returnCode=SUCCESS_ID;
  if (m_useSentenceBounds) {
    returnCode=mergeEventTemplatesOnEachSentence(analysis,eventData);
  }
  else {
    returnCode=mergeEventTemplates(eventData,0,0);
  }
  
  cleanEventTemplates(eventData);
  
  TimeUtils::logElapsedTime("EventTemplateMerging");
  return returnCode;
}

LimaStatusCode EventTemplateMerging::mergeEventTemplatesOnEachSentence(AnalysisContent& analysis, EventTemplateData* eventData) const
{
  LOGINIT("LP::EventAnalysis");
  // get sentence bounds
  SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData(m_sentenceBoundsData));
  if (nullptr==sb)
  {
    LERROR << "no sentence bounds "<< m_sentenceBoundsData << " defined ! abort";
    return MISSING_DATA;
  }
  LimaStatusCode res(SUCCESS_ID);
  for (const auto& segment: sb->getSegments()) {
    res = mergeEventTemplates(eventData,segment.getPosBegin(),segment.getPosEnd());
    if (res!=SUCCESS_ID) {
      break;
    }
  }
  return res;
}

// utility function to check if a template is considered or ignored
bool EventTemplateMerging::ignoreTemplate(const EventTemplate& event, uint64_t numTemplate,
                                          uint64_t posBegin, uint64_t posEnd, 
                                          const std::set<uint64_t>& toRemove) const
{
  if (event.getPosEnd()<=posBegin || (posEnd>0 && event.getPosBegin()>=posEnd) ) {
    LOGINIT("LP::EventAnalysis");
    LDEBUG << "template"<< numTemplate << "ignored: not in the considered segment "
           <<"["<<event.getPosBegin()<<","<<event.getPosEnd()<<"]";
    return true;
  }
  
  if (event.getType()!=m_templateDefinition->getMention()) {
    LOGINIT("LP::EventAnalysis");
    LDEBUG << "template"<< numTemplate << "ignored: not the considered event type ("<<event.getType()<<"/"<<m_templateDefinition->getMention()<<")";
    return true;
  }

  // do we keep merging with other templates if this one is to be removed ?
  if (toRemove.find(numTemplate)!=toRemove.end()) {
    LOGINIT("LP::EventAnalysis");
    LDEBUG << "template"<< numTemplate << "ignored: already merged";
    return true;
  }
  
  return false;  
}


LimaStatusCode EventTemplateMerging::mergeEventTemplates(EventTemplateData* eventData, uint64_t posBegin, uint64_t posEnd) const
{
  // ad hoc strategy for merging event templates
  LOGINIT("LP::EventAnalysis");

  // merge templates according to their positions and positions of intermediate entities
  LDEBUG << "TemplateMerging: merge templates of type" << m_templateDefinition->getName() << "in ["<<posBegin<<","<<posEnd<<"]";
  LDEBUG << "nb events =" << eventData->size();
  
  std::set<uint64_t> toRemove;
  uint64_t numTemplate=0;
  for (EventTemplateData::iterator it1=eventData->begin(),it_end=eventData->end();it1!=it_end;it1++,numTemplate++)
  {
    if (ignoreTemplate(*it1,numTemplate,posBegin,posEnd,toRemove)) {
      continue;
    }

    // begin comparison with next template
    EventTemplateData::iterator it2=it1;
    uint64_t numOtherTemplate=numTemplate; 
    for (it2++,numOtherTemplate++; it2!=it_end; it2++,numOtherTemplate++)
    {
      if (ignoreTemplate(*it2,numOtherTemplate,posBegin,posEnd,toRemove)) {
        continue;
      }

      TemplateElements& templateElements2=(*it2).getTemplateElements();
      if (templateElements2.empty()) {
        continue;
      }

      LDEBUG << "TemplateMerging: compare templates"
      << numTemplate << "and" << numOtherTemplate << ":\n"
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
    }
  }

  // remove merged templates : get template number in reverse
  for (set<uint64_t>::reverse_iterator it=toRemove.rbegin(),
    it_end=toRemove.rend(); it!=it_end; it++)
  {
    LDEBUG << "Erase template" << (*it)+1;
    eventData->erase(eventData->begin()+(*it));
  }

  return SUCCESS_ID;
}

void EventTemplateMerging::cleanEventTemplates(EventTemplateData* eventData) const
{
  LOGINIT("LP::EventAnalysis");
  // additional step if mandatory elements are specified : keep only templates with mandatory elements
  if (! m_mandatoryElements.empty()) {
    EventTemplateData::iterator it=eventData->begin();
    uint64_t n=1; // only for debug messages
    while (it!=eventData->end()) {
      bool toRemove=false;
      const TemplateElements& elements=(*it).getTemplateElements();
      for (deque<string>::const_iterator m=m_mandatoryElements.begin(),m_end=m_mandatoryElements.end();m!=m_end;m++) {
        if (elements.find(*m)==elements.end()) {
          LDEBUG << "Erase template" << n << "because it does not contain mandatory element" << *m << ":" << elements;
          toRemove=true;
          break;
        }
      }
      if (toRemove) {
        it=eventData->erase(it);
      }
      else {
        it++;
        n++;
      }
    }
  }
}

bool EventTemplateMerging::compatibleTemplates(const EventTemplate& e1, const EventTemplate& e2,
                                               unsigned int numTemplate1, unsigned int numTemplate2,
                                               TemplateElements& mergedElements) const
//                                               map<string,pair<string, int> >& matchingElements) const
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
    << numTemplate1 << "and" << numTemplate2
    << "not compatible: too much apart ("<< posdiff << "chars)";
    return false;
  }

  const TemplateElements& templateElements1=e1.getTemplateElements();
  const TemplateElements& templateElements2=e2.getTemplateElements();

  std::vector<EventTemplateElement> merged;
  // merge template1 in template2: store the roles in template 1 that do correspond to one in template2
  // (to add the ones that do not, at the end)
  std::set<std::string> alreadyMerged;
  
  // templateElements is a multimap: for each role, each template has a set of values
  for (TemplateElements::const_iterator elt2=templateElements2.begin(),
    elt2_end=templateElements2.end(); elt2!=elt2_end; )
  {
    const std::string& role2=(*elt2).first;
    unsigned int cardinality=m_templateDefinition->getCardinality(role2);
    // get all values for this role: use pointers to avoid copies
    vector<const EventTemplateElement*> values2;

    // Advance to next non-duplicate entry.
    while (elt2 != templateElements2.end() && role2 == (*elt2).first) {
      values2.push_back(&((*elt2).second));
      ++elt2;
    }
    
    pair<TemplateElements::const_iterator,TemplateElements::const_iterator> found1=templateElements1.equal_range(role2);
    if (found1.first!=found1.second) {
      // the same role exists in other template, check values
      vector<const EventTemplateElement*> values1;
      for (TemplateElements::const_iterator it=found1.first,it_end=found1.second;it!=it_end;++it) {
        values1.push_back(&((*it).second));
      }
      if (compatibleValues(values2,values1,cardinality,merged)) {
        for (const auto& value: merged) {
          mergedElements.emplace(role2,value);
        }
        alreadyMerged.insert(role2);
      }
      else {
        // different values, assume templates of different elements
        LDEBUG << "TemplateMerging: templates"
        << numTemplate1 << "and" << numTemplate2
        << ":" << role2 << ": incompatible values";
        mergedElements.clear();
        return false;
      }
    }
    else { // try to find mapping through constraint
      bool mappingFound=false;
      for (TemplateElements::const_iterator elt1=templateElements1.begin(),
        elt1_end=templateElements1.end(); elt1!=elt1_end; )
      {
        const std::string& role1=(*elt1).first;
        int mapping=m_templateDefinition->existsMapping(role1,role2);
        if (mapping!=0) {
          mappingFound=true;
          // compatible role, check values
          // gather values from other template : advance to next non-duplicate entry
          vector<const EventTemplateElement*> values1;
          while (elt1 != templateElements1.end() && role1 == (*elt1).first) {
            values1.push_back(&((*elt1).second));
            ++elt1;
          }
          if (compatibleValues(values2,values1,cardinality,merged)) {
            for (const auto& value: merged) {
              mergedElements.emplace(role2,value);
            }
            alreadyMerged.insert(role1);
          }
          else {
            // different values, assume templates of different elements
            LDEBUG << "TemplateMerging: templates"
            << numTemplate1 << "and" << numTemplate2
            << ":" << role2 << "/" << (*elt1).first << ": incompatible values";
            mergedElements.clear();
            return false;
          }
          // do not search for another match in mapping, take first
          break;
        }
        else {
          // no mapping: advance to next non-duplicate entry
          do {
              elt1++;
          } while (elt1 != templateElements1.end() && role1== (*elt1).first);
        }
      }
      if (! mappingFound) {
        LDEBUG << "TemplateMerging: templates"
        << numTemplate1 << "and" << numTemplate2
        << ":" << role2 << ": not found in" << numTemplate1;
        // add it to the merged template
        for (const auto& v2: values2) {
          mergedElements.emplace(role2,*v2);
        }
      }
    }

    /*
    //--begin old code--
    const EventTemplateElement& elt=(*fill).second;

    TemplateElements::const_iterator found=templateElements1.find(role);
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
      for (TemplateElements::const_iterator e=templateElements1.begin(),
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
    //--end old code--
    */
  }
  // add in mergedElements the one from template1 that were not already merged
  for (const auto& elt1: templateElements1) {
    if (alreadyMerged.find(elt1.first)==alreadyMerged.end()) {
      mergedElements.emplace(elt1.first,elt1.second);
    }
  }
  
  return true;
}

bool EventTemplateMerging::compatibleValues(const std::vector<const EventTemplateElement*> values1, 
                                            const std::vector<const EventTemplateElement*> values2,
                                            unsigned int cardinality,
                                            std::vector<EventTemplateElement>& merged) const
{
  // a clean way to check compatibility of multiple values could be to use a general assignment problem solution 
  // (such as Kuhn-Munkres hungarian algorithm)
  // seems a bit complex to implement: use a simpler greedy method (not optimal, but suppose the cardinality of 
  // values is generally one anyway)
  vector<bool> done(values2.size(),false);
  for (const auto val1: values1) {
    for (unsigned int i(0),n=values2.size();i<n;i++) {
      if (done[i]) {
        continue;
      }
      if (val1->isSimilar(*(values2[i]))) {
        done[i]=true;
      }
    }
    merged.push_back(*val1);
  }
  for (unsigned int i(0),n=values2.size();i<n;i++) {
    if (! done[i]) {
      if (merged.size()+1>cardinality) {
        merged.clear();
        return false;
      }
      merged.push_back(*(values2[i]));
    }
  }
  return true;
}


bool EventTemplateMerging::
mergeTemplates(EventTemplate& e1, EventTemplate& e2,
               unsigned int numTemplate1, unsigned int numTemplate2,
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
  //map<string,pair<string, int> > matchingElements;
  
  // while comparing templates for compatibility, create the merged version of the template
  // replace the whole template by the merged template (not as efficient as changing each template element, 
  // but easier to code/maintain)
  TemplateElements mergedElements;

  // by default merge 1 into 2
  // use pointers instead of references to reassign them if merge is the other way
  //TemplateElements* sourceElements=&(e1.getTemplateElements());
  //TemplateElements* targetElements=&(e2.getTemplateElements());
  EventTemplate* templateToModify=&e2;

  // heuristic: keep the templates with the most elements
  if (e1.getTemplateElements().size() > e2.getTemplateElements().size()) {
    if (!compatibleTemplates(e2,e1,numTemplate2,numTemplate1,mergedElements)) {
      return false;
    }
    //     sourceElements=&(e2.getTemplateElements());
    //     targetElements=&(e1.getTemplateElements());
    templateToModify=&e1;
    templateToRemove=numTemplate2;
  }
  else if (!compatibleTemplates(e1,e2,numTemplate1,numTemplate2,mergedElements)) {
      return false;
  }
  else {
    LDEBUG << "TemplateMerging: merge" << numTemplate1 << "into" << numTemplate2;
    templateToRemove=numTemplate1;
  }

  templateToModify->setTemplateElements(mergedElements);
    
  /*
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
  */
  // returns the template to remove
  if (templateToRemove==numTemplate1) {
    LDEBUG << "TemplateMerging: keep template" << numTemplate2 << e2;
  }
  else {
    LDEBUG << "TemplateMerging: keep template"<< numTemplate1 << e1;
  }
  return true;
}


} // end namespace
} // end namespace
} // end namespace
