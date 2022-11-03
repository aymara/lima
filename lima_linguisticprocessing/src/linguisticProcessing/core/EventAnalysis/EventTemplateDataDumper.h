// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Sep 19 2011
 * @brief a dumper for EventTemplateData that outputs an object of type Events (for compatibility
 * with existing event extraction webservice)
 ***********************************************************************/

#ifndef EVENTTEMPLATEDATADUMPER_H
#define EVENTTEMPLATEDATADUMPER_H

#include "EventAnalysisExport.h"
#include "EventTemplateData.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

#define EVENTTEMPLATEDATADUMPER_CLASSID "EventTemplateDataDumper"

class LIMA_EVENTANALISYS_EXPORT EventTemplateDataDumper : public AbstractTextualAnalysisDumper
{
 public:
  EventTemplateDataDumper(); 
  virtual ~EventTemplateDataDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

 private:
   std::string m_eventData;
                                                
};

} // end namespace
} // end namespace
} // end namespace

#endif
