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
 * @file       EventDataDumper.h
 * @author     Olivier Mesnard (olivier.mesnard@cea.fr)
 * @date       Mon Oct 21 2011
 * copyright   Copyright (C) 2011 by CEA LIST (LVIC)
 * Project     MM
 * 
 * @brief a dumper for EventData that outputs an object of type Events
 * (for compatibility with existing event extraction webservice)
 * 
 * 
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
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;

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
