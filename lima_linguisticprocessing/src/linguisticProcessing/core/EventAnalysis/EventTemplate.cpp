// Copyright (C) 2011 by CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
                               const EventTemplateElement& elt, 
                               unsigned int cardinality)
{
  if (m_template.count(role)>=cardinality) {
    // too many values for the same role: which ones to keep ?
    // at this stage, we do not have any info on the order in which the values
    // were inserted (they are sorted by the multimap) : 
    // we choose to just ignore the new values to be added
    LOGINIT("LP::EventAnalysis");
    LDEBUG << "EventTemplate::addElement: too many values for role" << role <<": ignore new value '"<< elt << "'";
    return;
  }
  m_template.emplace(role,elt);
  uint64_t posBegin=elt.getPositionBegin();
  uint64_t posEnd=elt.getPositionEnd();
  if (posEnd > m_posEnd) {
    m_posEnd=posEnd;
  }
  if (posBegin < m_posBegin) {
    m_posBegin=posBegin;
  }
}

void EventTemplate::setTemplateElements(const TemplateElements& elts)
{
  // overwrite existing elements
  clear();
  m_template=elts;
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplate::setTemplateElements: set"<<m_template.size()<<"new elements";
  // set global position
  for (const auto& elt: m_template) {
    uint64_t posBegin=elt.second.getPositionBegin();
    uint64_t posEnd=elt.second.getPositionEnd();
    if (posEnd > m_posEnd) {
      m_posEnd=posEnd;
    }
    if (posBegin < m_posBegin) {
      m_posBegin=posBegin;
    }
  }
  LDEBUG << "EventTemplate::setTemplateElements: new position is ["<<m_posBegin<<","<<m_posEnd<<"]";
}

void EventTemplate::clear()
{
  m_template.clear();
  m_weight=0.0;
  m_mainEvent=false;
  m_posBegin=std::numeric_limits<uint64_t>::max();
  m_posEnd=0;
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
