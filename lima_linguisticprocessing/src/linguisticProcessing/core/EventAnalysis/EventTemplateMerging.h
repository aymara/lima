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
  
  // private member function
  LimaStatusCode mergeEventTemplates(AnalysisContent& analysis) const;
};

} // end namespace
} // end namespace
} // end namespace

#endif
