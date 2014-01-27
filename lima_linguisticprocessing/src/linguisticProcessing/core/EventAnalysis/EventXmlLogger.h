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
    Manager* manager)
  ;

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const;

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
