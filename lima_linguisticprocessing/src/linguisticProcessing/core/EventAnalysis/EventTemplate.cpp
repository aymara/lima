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

EventTemplate::EventTemplate(const std::string& type):
m_template(),
m_weight(0.0),
m_mainEvent(false),
m_type(type)
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
