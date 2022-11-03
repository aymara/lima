// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventTemplateMerging.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Sep 26 2011
 * copyright   Copyright (C) 2011 by CEA - LIST 
 * Project     LIMA
 * 
 * @brief 
 * 
 * 
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_EVENTTEMPLATEMERGING_H
#define LIMA_LINGUISTICPROCESSING_EVENTTEMPLATEMERGING_H

#include "EventAnalysisExport.h"
#include "EventTemplateStructure.h"
#include "EventTemplateData.h"
#include "EventTemplate.h"
#include "EventTemplateDefinitionResource.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

//**********************************************************************
// process unit
#define EVENTTEMPLATEMERGING_CLASSID "EventTemplateMerging"

class LIMA_EVENTANALISYS_EXPORT EventTemplateMerging : public MediaProcessUnit
{
public:
  EventTemplateMerging();

  virtual ~EventTemplateMerging();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;
    
private:
  EventTemplateDefinitionResource* m_templateDefinition;
  std::deque<std::string> m_mandatoryElements;
  uint64_t m_maxCharCompatibleEvents;
  bool m_useSentenceBounds;
  std::string m_sentenceBoundsData; // name of data containing the boundaries if m_useSentenceBounds
  
  // private member function
  LimaStatusCode mergeEventTemplatesOnEachSentence(AnalysisContent& analysis, EventTemplateData* eventData) const;
  LimaStatusCode mergeEventTemplates(EventTemplateData* eventData, uint64_t posBegin, uint64_t posEnd) const;
  bool compatibleTemplates(const EventTemplate& e1, const EventTemplate& e2,
                           unsigned int numTemplate, unsigned int numOtherTemplate,
                           TemplateElements& mergedElements) const;
                           
  bool compatibleValues(const std::vector<const EventTemplateElement*> values1, 
                        const std::vector<const EventTemplateElement*> values2,
                        unsigned int cardinality,
                        std::vector<EventTemplateElement>& merged) const;
                           
  // return the template to be removed (either numTemplate or numOtherTemplate)
  bool mergeTemplates(EventTemplate& e1, EventTemplate& e2,
                      unsigned int numTemplate, unsigned int numOtherTemplate,
                      unsigned int& templateToRemove) const;
                      
  bool ignoreTemplate(const EventTemplate& event, uint64_t numTemplate,
                      uint64_t posBegin, uint64_t posEnd, 
                      const std::set<uint64_t>& toRemove) const;
                      
  void cleanEventTemplates(EventTemplateData* eventData) const;
                      
  
};

} // end namespace
} // end namespace
} // end namespace

#endif
