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
  EventTemplateDefinitionResource* m_templateDefinition;

};

} // end namespace
} // end namespace
} // end namespace

#endif
