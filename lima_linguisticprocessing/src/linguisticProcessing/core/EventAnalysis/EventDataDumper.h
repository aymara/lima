// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     Olivier Mesnard (olivier.mesnard@cea.fr)
 * @date       Mon Oct 21 2011
 * @brief a dumper for EventData that outputs an object of type Events
 * (for compatibility with existing event extraction webservice)
 ***********************************************************************/
#ifndef EVENTDATADUMPER_H
#define EVENTDATADUMPER_H

#include "EventAnalysisExport.h"
#include "EventData.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

#define EVENTDATADUMPER_CLASSID "EventDataDumper"

class LIMA_EVENTANALISYS_EXPORT EventDataDumper : public AbstractTextualAnalysisDumper
{
 public:
  EventDataDumper(); 
  virtual ~EventDataDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

 private:
   // key to access annotationData about events in AnalysisContent
   // initalized to "EventData"
   std::string m_eventData;
   // key to access segmentationData in AnalysisContent
   // initalized to "SegmentationData"
   std::string m_segmentationData;
                                                
};



} // end namespace
} // end namespace
} // end namespace

#endif // EVENTDATADUMPER_H
