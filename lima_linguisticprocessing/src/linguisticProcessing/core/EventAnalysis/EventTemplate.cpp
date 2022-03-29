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
m_mainEvent(false),
m_type(),
m_posBegin(std::numeric_limits<uint64_t>::max()),
m_posEnd(0)
{
}

EventTemplate::EventTemplate(const std::string& type):
m_template(),
m_weight(0.0),
m_mainEvent(false),
m_type(type),
m_posBegin(0),
m_posEnd(0)
{
}

EventTemplate::~EventTemplate() {
}

void EventTemplate::addElement(const std::string& role, 
                               const EventTemplateElement& elt)
{
  m_template[role]=elt;
  uint64_t posBegin=elt.getPositionBegin();
  uint64_t posEnd=elt.getPositionEnd();
  if (posEnd > m_posEnd) {
    m_posEnd=posEnd;
  }
  if (posBegin < m_posBegin) {
    m_posBegin=posBegin;
  }
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
    LERROR << "No element '" << role << "' in EventTemplate";
    return emptyElement;
  }
  return (*it).second;
}

std::ostream& operator<<(std::ostream& os, const EventTemplate& e)
{
  os << e.m_type << "/[" << e.m_posBegin << "-" << e.m_posEnd << "]";
  for (const auto& elt: e.m_template) {
    os << "/" << elt.first << ":" << elt.second;
  }
  return os;
}
QDebug& operator<<(QDebug& os, const EventTemplate& e)
{
  std::ostringstream oss;
  oss << e;
  os << oss.str();
  return os;
}


} // end namespace
} // end namespace
} // end namespace
