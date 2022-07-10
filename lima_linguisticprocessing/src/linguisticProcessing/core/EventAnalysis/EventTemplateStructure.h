// Copyright (C) 2011 by CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef EVENTTEMPLATESTRUCTURE_H
#define EVENTTEMPLATESTRUCTURE_H

#include "EventAnalysisExport.h"
#include "common/MediaticData/mediaticData.h"
#include <string>
#include <map>


namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

class LIMA_EVENTANALISYS_EXPORT EventTemplateStructure
{
 public:
  EventTemplateStructure(); 
  ~EventTemplateStructure();
  
  void setName(const std::string& name) { m_name=name; }
  void setMention(const std::string& name) { m_mention=name; }
  void addTemplateElement(const std::string& role, const std::string entityType);
  
  const std::string& getName(void) const { return m_name; }
  const std::string& getMention(void) const { return m_mention; }
  const std::map<std::string,Common::MediaticData::EntityType>& getStructure(void) const
  { return m_structure; }

 private:
  std::string m_name;
  std::map<std::string,Common::MediaticData::EntityType> m_structure;
  std::string m_mention;
};

} // end namespace
} // end namespace
} // end namespace

#endif
