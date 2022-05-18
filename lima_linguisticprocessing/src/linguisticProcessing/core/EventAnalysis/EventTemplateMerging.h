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
  
  // private member function
  LimaStatusCode mergeEventTemplates(AnalysisContent& analysis) const;
  bool compatibleTemplates(const EventTemplate& e1, const EventTemplate& e2,
                           unsigned int numTemplate, unsigned int numOtherTemplate,
                           std::map<std::string, std::pair<std::string, int> >& matchingElements) const;
                           
  // return the template to be removed (either numTemplate or numOtherTemplate)
  bool mergeTemplates(EventTemplate& e1, EventTemplate& e2,
                      unsigned int numTemplate, unsigned int numOtherTemplate,
                      unsigned int& templateToRemove) const;

  
};

} // end namespace
} // end namespace
} // end namespace

#endif
