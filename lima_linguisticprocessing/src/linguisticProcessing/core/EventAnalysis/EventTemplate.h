/************************************************************************
 *
 * @file       EventTemplate.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Sep  1 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     LIMA
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
  EventTemplate(const std::string&);
  ~EventTemplate();

  void addElement(const std::string& role, const EventTemplateElement& elt);
  void setWeight(double w) { m_weight=w; }
  void setType(const std::string type) { m_type=type; }
  void setMain(bool isMainEvent) { m_mainEvent=isMainEvent; }
  void clear();

  const EventTemplateElement& getElement(const std::string& role) const;
  const std::map<std::string,EventTemplateElement>& getTemplateElements() const { return m_template; }
  std::map<std::string,EventTemplateElement>& getTemplateElements() { return m_template; }
  double getWeight() const { return m_weight; }
  const std::string getType() const { return m_type; }
  bool isMainEvent() const { return m_mainEvent; }

 private:
  std::map<std::string,EventTemplateElement> m_template;
  double m_weight;
  bool m_mainEvent;
  std::string m_type;
};

} // end namespace
} // end namespace
} // end namespace

#endif
