// Copyright (C) 2011 by CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
AbstractResource(),
m_language(0),
m_structure(),
m_elementMapping()
{
}

EventTemplateDefinitionResource::~EventTemplateDefinitionResource() {
}

const std::string& EventTemplateDefinitionResource::getMention () const
{
  return m_structure.getMention();
}

const std::string& EventTemplateDefinitionResource::getName () const
{
  return m_structure.getName();
}

const std::map<std::string,Common::MediaticData::EntityType>& EventTemplateDefinitionResource::getStructure () const
{
  return m_structure.getStructure();
}

//----------------------------------------------------------------------
void EventTemplateDefinitionResource::
init(GroupConfigurationStructure& unitConfiguration,
     Manager* manager)
   
{
  LOGINIT("LP::EventAnalysis");

  m_language=manager->getInitializationParameters().language;
  LDEBUG << "initialize EventTemplateDefinitionResource for language " << (int)m_language;
  // get name
  try
  {
    string name = unitConfiguration.getParamsValueAtKey("templateName");
    m_structure.setName(name);
    LDEBUG << "Template name = "<< name;
    
  }
  catch (NoSuchParam& ) {
    LERROR << "No param 'templateName' in EventTemplateDefinitionResource for language " << (int)m_language;
    throw InvalidConfiguration();
  }
  try{
  
    string nameMention = unitConfiguration.getParamsValueAtKey("templateMention");
    LDEBUG << "Template mention = "<< nameMention;
    m_structure.setMention(nameMention);
  }
  
  catch (NoSuchParam& ) {
    LERROR << "No param 'templateMention' in EventTemplateDefinitionResource for language " << (int)m_language;
    //throw InvalidConfiguration();
  }

  // get template elements: role and entity types
  try
  {
    map<string,string> elts  = unitConfiguration.getMapAtKey("templateElements");
    LDEBUG << "templateElements .size " << elts.size();
    for(auto it=elts.begin(),it_end=elts.end();it!=it_end;it++) {
      LDEBUG << "templateElement =" << (*it).first;
      m_structure.addTemplateElement((*it).first,(*it).second);
    }
  }
  catch (NoSuchMap& ) {
    LERROR << "No map 'templateElements' in EventTemplateDefinition for language " << (int)m_language;
    throw InvalidConfiguration();
  }

  // get element mapping, for template merging
  LDEBUG << "get elementMapping ";
  try
  {
    map<string,string> mapping  = unitConfiguration.getMapAtKey("elementMapping");
    LDEBUG << "after Getting map ";
    for(auto it=mapping.cbegin(),it_end=mapping.cend();it!=it_end;it++) {
      const std::string& elements=(*it).second;
      // comma-separated list of elements
      boost::char_separator<char> sep(",; ");
      boost::tokenizer<boost::char_separator<char> > tok(elements,sep);
      for(auto e=tok.begin(),e_end=tok.end(); e!=e_end;e++) {
        LDEBUG << "EventTemplateDefinitionResource: add mapping " 
                << (*it).first << ":" << *e;
        m_elementMapping[(*it).first].insert(*e);
      }
    }
  }
  catch (NoSuchMap& ) {
    LDEBUG << "No param 'elementMapping' in EventTemplateDefinition for language " 
            << (int)m_language;
  }
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
  //LOGINIT("LP::EventAnalysis");
  //LDEBUG << "EventTemplateDefinitionResource::existsMapping : compare " << eltName1 << " and " << eltName2 << "->" << res;
  return res;
}



} // end namespace
} // end namespace
} // end namespace
