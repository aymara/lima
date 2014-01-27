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
 * @file       EventTemplateStructure.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Sep  1 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     MM
 * 
 * @brief      structure of an event template (define the name and type of possible template elements)
 * 
 * 
 ***********************************************************************/

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
  void addTemplateElement(const std::string& role, const std::string entityType);
  
  const std::string& getName(void) const { return m_name; }
  const std::map<std::string,Common::MediaticData::EntityType>& getStructure(void) const
  { return m_structure; }

 private:
  std::string m_name;
  std::map<std::string,Common::MediaticData::EntityType> m_structure;
};

} // end namespace
} // end namespace
} // end namespace

#endif
