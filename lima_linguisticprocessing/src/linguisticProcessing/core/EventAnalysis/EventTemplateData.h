// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventTemplateData.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Sep  2 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     LIMA
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
