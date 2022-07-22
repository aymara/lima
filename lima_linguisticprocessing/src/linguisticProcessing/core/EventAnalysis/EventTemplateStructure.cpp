// Copyright (C) 2011 by CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "EventTemplateStructure.h"
#include "common/MediaticData/mediaticData.h"
#include <common/Data/strwstrtools.h>

namespace Lima {
  using namespace Common;
namespace LinguisticProcessing {
namespace EventAnalysis {

EventTemplateStructure::EventTemplateStructure():
m_name(),
m_structure()
{
}

EventTemplateStructure::~EventTemplateStructure() {
}

void EventTemplateStructure::addTemplateElement(const std::string& role, 
                                                const std::string entityType)
{
  if (m_structure.find(role)!=m_structure.end()) {
    LOGINIT("LP::EventAnalysis");
    LERROR << "In event " << m_name << ", element '"<< role 
            <<"' is defined twice";
  }
  else {
    try {
      auto type = MediaticData::MediaticData::single().getEntityType(
        QString::fromUtf8(entityType.c_str()));
      m_structure[role]=type;
    }
    catch (LimaException& e) {
      LOGINIT("LP::EventAnalysis");
      LERROR << "EventTemplateStructure: failed to resolve entity type" << entityType << ": ignore role" << role;    
    }
  }
}



} // end namespace
} // end namespace
} // end namespace
