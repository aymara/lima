// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventTemplateDataXmlLogger.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Sep 05 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     LIMA
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
