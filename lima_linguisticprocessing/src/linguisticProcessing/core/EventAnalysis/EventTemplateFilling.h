// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventTemplateFilling.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Thu Sep 01 2011
 * copyright   Copyright (C) 2011 by CEA - LIST 
 * Project     LIMA
 * 
 * @brief 
 * 
 * 
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_EVENTTEMPLATEFILLING_H
#define LIMA_LINGUISTICPROCESSING_EVENTTEMPLATEFILLING_H

#include "EventAnalysisExport.h"
#include "EventTemplateStructure.h"
#include "EventTemplateDefinitionResource.h"
#include "linguisticProcessing/core/ApplyRecognizer/applyRecognizer.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

//**********************************************************************
// process unit
#define EVENTTEMPLATEFILLING_CLASSID "EventTemplateFilling"

class LIMA_EVENTANALISYS_EXPORT EventTemplateFilling : public ApplyRecognizer::ApplyRecognizer
{
public:
  EventTemplateFilling();

  virtual ~EventTemplateFilling();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;
    
private:
  std::shared_ptr<EventTemplateDefinitionResource> m_templateDefinition;

};

} // end namespace
} // end namespace
} // end namespace

#endif
