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
 * @file       EventTemplateData.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Sep  2 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     MM
 * 
 * @brief      AnalysisData storing the result of event template filling
 * 
 * 
 ***********************************************************************/

#ifndef EVENTTEMPLATEDATA_H
#define EVENTTEMPLATEDATA_H

#include "EventAnalysisExport.h"
#include "EventTemplate.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

class Events;
class Entity;
  
class LIMA_EVENTANALISYS_EXPORT EventTemplateData : public AnalysisData, public std::vector<EventTemplate>
{
 public:
  EventTemplateData(); 
  ~EventTemplateData();
  
  void addTemplate();
  void addElementInCurrentTemplate(const std::string& role, const EventTemplateElement& elt);
  void clearCurrentTemplate();
  void setTypeInCurrentTemplate(const std::string&);

  Events* convertToEvents(const Common::AnnotationGraphs::AnnotationData* annotationData) const;
  
 private:
   // private member functions
   Entity* createEntity(const LinguisticAnalysisStructure::AnalysisGraph* graph,
                        LinguisticGraphVertex v,
                        const Common::AnnotationGraphs::AnnotationData* annotationData,
                        const std::string& role,
                        uint64_t eventId)  const;
};

} // end namespace
} // end namespace
} // end namespace

#endif
