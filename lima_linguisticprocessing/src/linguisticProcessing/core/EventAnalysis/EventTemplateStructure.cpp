/************************************************************************
 *
 * @file       EventTemplateStructure.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Sep  2 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "EventTemplateStructure.h"
#include "common/MediaticData/mediaticData.h"
#include <common/Data/strwstrtools.h>

namespace Lima {
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
    LERROR << "In event " << m_name << ", element '"<< role <<"' is defined twice"  << LENDL;
  }
  else {
    Common::MediaticData::EntityType type=
    Common::MediaticData::MediaticData::single().getEntityType(Common::Misc::utf8stdstring2limastring(entityType));
    m_structure[role]=type;
  }
}



} // end namespace
} // end namespace
} // end namespace
