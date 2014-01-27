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
 * @file       EventTemplateDefinitionResource.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Sep  2 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     MM
 * 
 * @brief      Linguistic resource storing the defintion of event templates
 * 
 * 
 ***********************************************************************/

#ifndef EVENTTEMPLATEDEFINITIONRESOURCE_H
#define EVENTTEMPLATEDEFINITIONRESOURCE_H

#include "EventAnalysisExport.h"
#include "EventTemplateStructure.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

#define EVENTTEMPLATEDEFINITIONRESOURCE_CLASSID "EventTemplateDefinition"

class LIMA_EVENTANALISYS_EXPORT EventTemplateDefinitionResource : public AbstractResource
{
 public:
  EventTemplateDefinitionResource(); 
  ~EventTemplateDefinitionResource();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager)
    ;
  
  // mapping is oriented, return 1 if mapping elt1 -> elt2, -1 if mapping elt2 -> elt1, 0 otherwise
  int existsMapping(const std::string& eltName1, const std::string& eltName2) const;
    
 private:
  MediaId m_language;
  std::vector<EventTemplateStructure> m_templates;
  std::map<std::string, std::set<std::string> > m_elementMapping;
  
};

} // end namespace
} // end namespace
} // end namespace

#endif
