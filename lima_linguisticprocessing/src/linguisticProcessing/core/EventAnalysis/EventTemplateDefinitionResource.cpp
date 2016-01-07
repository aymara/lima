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
m_language(0),
m_templates(),
m_elementMapping()
{
}

EventTemplateDefinitionResource::~EventTemplateDefinitionResource() {
}

const std::string& EventTemplateDefinitionResource::getMention (const std::string name) const
{
  std::string mention="";
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "getMention m_templates.size() " << m_templates.size() << LENDL;
  for(std::vector<EventTemplateStructure>::const_iterator it=m_templates.begin();it!=m_templates.end();it++)
  {
    LDEBUG << "Cuurent Mention " << it->getMention()<< LENDL;
    if (name.compare(it->getName())==0) return it->getMention();
  }
  return mention;
}

const std::map<std::string,Common::MediaticData::EntityType>& EventTemplateDefinitionResource::getStructure (const std::string name) const
{
  std::map<std::string,Common::MediaticData::EntityType> structure;
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "getMention m_templates.size() " << m_templates.size() << LENDL;
  for(std::vector<EventTemplateStructure>::const_iterator it=m_templates.begin();it!=m_templates.end();it++)
  {
    //LDEBUG << "Cuurent Mention " << it->getMention()<< LENDL;
    if (name.compare(it->getName())==0) return it->getStructure();
  }
  return structure;
}

//----------------------------------------------------------------------
void EventTemplateDefinitionResource::
init(GroupConfigurationStructure& unitConfiguration,
     Manager* manager)
   
{
  LOGINIT("LP::EventAnalysis");

  m_language=manager->getInitializationParameters().language;
  string resourcesPath=Common::MediaticData::MediaticData::single().getResourcesPath();
  LDEBUG << "resourcesPath = "<< resourcesPath << LENDL;
  EventTemplateStructure structure;
  // get name
  try
  {
    string name = unitConfiguration.getParamsValueAtKey("templateName");
    structure.setName(name);
    LDEBUG << "Template name = "<< name << LENDL;
    
  }
  catch (NoSuchParam& ) {
    LERROR << "No param 'templateName' in EventTemplateDefinitionResource for language " << (int)m_language << LENDL;
    throw InvalidConfiguration();
  }
  try{
  
    string nameMention = unitConfiguration.getParamsValueAtKey("templateMention");
    LDEBUG << "Template mention = "<< nameMention << LENDL;
    structure.setMention(nameMention);
  }
  
  catch (NoSuchParam& ) {
    LERROR << "No param 'templateMention' in EventTemplateDefinitionResource for language " << (int)m_language << LENDL;
    //throw InvalidConfiguration();
  }

  // get template elements: role and entity types
  try
  {
    map<string,string> elts  = unitConfiguration.getMapAtKey("templateElements");
    LDEBUG << "templateElements .size " << elts.size() << LENDL;
    for(map<string,string>::const_iterator it=elts.begin(),it_end=elts.end();it!=it_end;it++) {
      LDEBUG << "templateElement =" << (*it).first << LENDL;
      structure.addTemplateElement((*it).first,(*it).second);
    }
  }
  catch (NoSuchParam& ) {
    LERROR << "No param 'templateName' in EventTemplateDefinition for language " << (int)m_language << LENDL;
    throw InvalidConfiguration();
  }

  // get element mapping, for template merging
  LDEBUG << "get elementMapping " << LENDL;
  try
  {
    map<string,string> mapping  = unitConfiguration.getMapAtKey("elementMapping");
    LDEBUG << "after Getting map " << LENDL;
    for(map<string,string>::const_iterator it=mapping.begin(),it_end=mapping.end();it!=it_end;it++) {
      const std::string& elements=(*it).second;
      // comma-separated list of elements
      boost::char_separator<char> sep(",; ");
      boost::tokenizer<boost::char_separator<char> > tok(elements,sep);
      for(boost::tokenizer<boost::char_separator<char> >::iterator e=tok.begin(),e_end=tok.end(); e!=e_end;e++) {
        LDEBUG << "EventTemplateDefinitionResource: add mapping " << (*it).first << ":" << *e << LENDL;
        m_elementMapping[(*it).first].insert(*e);
      }
    }
  }
  catch (NoSuchMap& ) {
    LDEBUG << "No param 'elementMapping' in EventTemplateDefinition for language " << (int)m_language << LENDL;
  }
  LDEBUG << "Adding Structure " << LENDL;
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
  LDEBUG << "EventTemplateDefinitionResource::existsMapping : compare " << eltName1 << " and " << eltName2 << "->" << res << LENDL;
  return res;
}



} // end namespace
} // end namespace
} // end namespace
