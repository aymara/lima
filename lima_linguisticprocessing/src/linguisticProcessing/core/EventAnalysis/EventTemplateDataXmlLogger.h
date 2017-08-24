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
 * @file       EventTemplateDataXmlLogger.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Sep 05 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     MM
 * 
 * @brief      XML logger for recognized events
 * 
 * 
 ***********************************************************************/

#ifndef EVENTTEMPLATEDATAXMLLOGGER_H
#define EVENTTEMPLATEDATAXMLLOGGER_H

#include "EventAnalysisExport.h"
#include "EventTemplateData.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

#define EVENTTEMPLATEDATAXMLLOGGER_CLASSID "EventTemplateDataXmlLogger"

class LIMA_EVENTANALISYS_EXPORT EventTemplateDataXmlLogger : public AbstractTextualAnalysisDumper
{
 public:
  EventTemplateDataXmlLogger(); 
  virtual ~EventTemplateDataXmlLogger();
  
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

 private:
  std::string m_eventData;

  void outputEventData(std::ostream& out,
                       const EventTemplateData* eventData,
                       const Common::AnnotationGraphs::AnnotationData* annotationData) const;
  void outputEntity(std::ostream& out, 
                    const LinguisticAnalysisStructure::AnalysisGraph* graph,
                    LinguisticGraphVertex v,
                    const Common::AnnotationGraphs::AnnotationData* annotationData)  const;
  std::string xmlString(const std::string& inputStr) const;
                       
};

} // end namespace
} // end namespace
} // end namespace

#endif
