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
 * @file       EventTemplate.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Sep  2 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "EventTemplate.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

EventTemplate::EventTemplate():
m_template(),
m_weight(0.0),
m_mainEvent(false)
{
}

EventTemplate::~EventTemplate() {
}

void EventTemplate::addElement(const std::string& role, 
                               const EventTemplateElement& elt)
{
  m_template[role]=elt;
}

void EventTemplate::clear()
{
  m_template.clear();
  m_weight=0.0;
  m_mainEvent=false;
}

const EventTemplateElement& EventTemplate::getElement(const std::string& role) const
{
  static EventTemplateElement emptyElement;
  std::map<std::string,EventTemplateElement>::const_iterator 
    it=m_template.find(role);
  if (it==m_template.end()) {
    LOGINIT("LP::EventAnalysis");
    LERROR << "No element '" << role << "' in EventTemplate" << LENDL;
    return emptyElement;
  }
  return (*it).second;
}




} // end namespace
} // end namespace
} // end namespace
