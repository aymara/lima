// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventXmlLogger.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Mar 24 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     M
 * 
 * @brief      XML logger for recognized events
 * 
 * 
 ***********************************************************************/

#ifndef EVENTXMLLOGGER_H
#define EVENTXMLLOGGER_H

#include "EventAnalysisExport.h"
#include "Events.h"
#include "EventData.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

#define EVENTXMLLOGGER_CLASSID "EventXmlLogger"

class LIMA_EVENTANALISYS_EXPORT EventXmlLogger : public AbstractTextualAnalysisDumper
{
 public:
  EventXmlLogger(); 
  virtual ~EventXmlLogger();
  
  virtual void init(
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

 private:
  std::string m_eventData;

  void outputEvents(std::ostream& out,
                    const Events* eventData) const;
  void outputEventData(std::ostream& out,
                       const EventData* eventData) const;
  void outputEntity(std::ostream& out, 
                    const Entity& e)  const;
  std::string xmlString(const std::string& inputStr) const;
                       
};

} // end namespace
} // end namespace
} // end namespace

#endif
