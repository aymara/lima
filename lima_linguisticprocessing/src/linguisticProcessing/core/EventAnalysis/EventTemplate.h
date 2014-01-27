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
 * @file       EventTemplate.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Sep  1 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     MM
 * 
 * @brief      template for event representation
 * 
 * 
 ***********************************************************************/

#ifndef EVENTTEMPLATE_H
#define EVENTTEMPLATE_H

#include "EventAnalysisExport.h"
#include "EventTemplateElement.h"
#include <map>
#include <string>

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

typedef std::string EventRole;

class LIMA_EVENTANALISYS_EXPORT EventTemplate
{
 public:
  EventTemplate(); 
  ~EventTemplate();

  void addElement(const std::string& role, const EventTemplateElement& elt);
  void setWeight(double w) { m_weight=w; }
  void setMain(bool isMainEvent) { m_mainEvent=isMainEvent; }
  void clear();
  
  const EventTemplateElement& getElement(const std::string& role) const;
  const std::map<std::string,EventTemplateElement>& getTemplateElements() const { return m_template; }
  std::map<std::string,EventTemplateElement>& getTemplateElements() { return m_template; }
  double getWeight() const { return m_weight; }
  bool isMainEvent() const { return m_mainEvent; }
  
 private:
  std::map<std::string,EventTemplateElement> m_template;
  double m_weight;
  bool m_mainEvent;
};

} // end namespace
} // end namespace
} // end namespace

#endif
