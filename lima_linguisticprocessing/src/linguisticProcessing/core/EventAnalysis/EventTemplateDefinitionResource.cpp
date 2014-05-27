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
 * @file       EventTemplateDefinitionResource.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Sep  2 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "EventTemplateDefinitionResource.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include <boost/tokenizer.hpp>

using namespace Lima::Common::XMLConfigurationFiles;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

//----------------------------------------------------------------------
SimpleFactory<AbstractResource,EventTemplateDefinitionResource> 
EventTemplateDefinitionResourceFactory(EVENTTEMPLATEDEFINITIONRESOURCE_CLASSID);


//----------------------------------------------------------------------
EventTemplateDefinitionResource::EventTemplateDefinitionResource():
m_language(0)
{
}

EventTemplateDefinitionResource::~EventTemplateDefinitionResource() {
}

//----------------------------------------------------------------------
void EventTemplateDefinitionResource::
init(GroupConfigurationStructure& unitConfiguration,
     Manager* manager)
   
{
  LOGINIT("LP::EventAnalysis");

  m_language=manager->getInitializationParameters().language;
  string resourcesPath=Common::MediaticData::MediaticData::single().getResourcesPath();
  
  EventTemplateStructure structure;
  // get name
  try
  {
    string name = unitConfiguration.getParamsValueAtKey("templateName");
    structure.setName(name);
  }
  catch (NoSuchParam& ) {
    LERROR << "No param 'templateName' in EventTemplateDefinitionResource for language " << (int)m_language;
    throw InvalidConfiguration();
  }

  // get template elements: role and entity types
  try
  {
    map<string,string> elts  = unitConfiguration.getMapAtKey("templateElements");
    for(map<string,string>::const_iterator it=elts.begin(),it_end=elts.end();it!=it_end;it++) {
      structure.addTemplateElement((*it).first,(*it).second);
    }
  }
  catch (NoSuchParam& ) {
    LERROR << "No param 'templateName' in EventTemplateDefinition for language " << (int)m_language;
    throw InvalidConfiguration();
  }

  // get element mapping, for template merging
  try
  {
    map<string,string> mapping  = unitConfiguration.getMapAtKey("elementMapping");
    for(map<string,string>::const_iterator it=mapping.begin(),it_end=mapping.end();it!=it_end;it++) {
      const std::string& elements=(*it).second;
      // comma-separated list of elements
      boost::char_separator<char> sep(",; ");
      boost::tokenizer<boost::char_separator<char> > tok(elements,sep);
      for(boost::tokenizer<boost::char_separator<char> >::iterator e=tok.begin(),e_end=tok.end(); e!=e_end;e++) {
        LDEBUG << "EventTemplateDefinitionResource: add mapping " << (*it).first << ":" << *e;
        m_elementMapping[(*it).first].insert(*e);
      }
    }
  }
  catch (NoSuchParam& ) {
    LDEBUG << "No param 'elementMapping' in EventTemplateDefinition for language " << (int)m_language;
  }
  
  m_templates.push_back(structure);
}

int EventTemplateDefinitionResource::
existsMapping(const std::string& eltName1, const std::string& eltName2) const
{
  int res=0; 
  map<string,set<string> >::const_iterator it=m_elementMapping.find(eltName1);
  if (it!=m_elementMapping.end()) {
    if ( (*it).second.find(eltName2) != (*it).second.end() ) {
      res=1;
    }
  }
  else {
    // try other way
    map<string,set<string> >::const_iterator it=m_elementMapping.find(eltName2);
    if (it!=m_elementMapping.end()) {
      if ( (*it).second.find(eltName1) != (*it).second.end() ) {
        res=-1;
      }
    }
  }
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "EventTemplateDefinitionResource::existsMapping : compare " << eltName1 << " and " << eltName2 << "->" << res;
  return res;
}



} // end namespace
} // end namespace
} // end namespace
