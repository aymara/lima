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
    LERROR << "In event " << m_name << ", element '"<< role <<"' is defined twice" ;
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
