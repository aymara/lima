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
 * @file       Modex.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Wed Jan 31 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * 
 ***********************************************************************/

#include "Modex.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "common/MediaProcessors/MediaProcessors.h"
// #include "linguisticProcessing/common/linguisticData/linguisticData.h"
#include "common/Data/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {

//***********************************************************************
// constructors and destructors
Modex::Modex():
m_language(0),
m_pipeline(0)
{
}

Modex::~Modex() {
}

//***********************************************************************
void Modex::init(GroupConfigurationStructure& unitConfiguration,
                 Manager* manager)
  
{
  MODEXLOGINIT;
  m_language=manager->getInitializationParameters().language;
  
  // initialize all resources needed by modex from config file
  try {
    // try to get a single automaton
    string filename=unitConfiguration.getParamsValueAtKey("modexConfig");
    string configFile=Common::Misc::findFileInPaths(LinguisticData::single().getConfigPath().c_str(),filename.c_str()).toUtf8().constData();

    initModex(configFile,m_language);
  }
  catch (NoSuchParam& ) {
    LERROR << "No param 'modexConfig' in Modex definition group";
    throw InvalidConfiguration();
  }
}

void Modex::
initModex(const std::string& configFile,
          MediaId lang)
{
  TimeUtils::updateCurrentTime();
  MODEXLOGINIT;
  LINFO << "InformationExtraction resources initialization";

  XMLConfigurationFileParser configParser(configFile);

  // first, add resources and processors to common resources and processors

  try {
    ModuleConfigurationStructure& resources=LinguisticResources::changeable().getModuleConfiguration(lang);
    ModuleConfigurationStructure& modexResourcesModule=configParser.getModuleConfiguration("Resources");
    // get resources for this language
    map<string,string>& resourcesByLanguage=modexResourcesModule.
      getMapAtKeyOfGroupNamed("resourcesByLanguage","resourceList");

    map<string,string>::iterator it=resourcesByLanguage.find(LinguisticData::single().getMediaId(m_language));
    if (it == resourcesByLanguage.end()) {
      LERROR << "no resources for language '"
             << LinguisticData::single().getMediaId(m_language) 
             << "' in Resources for Modex";
      throw InvalidConfiguration();
    }

    std::string resourcesModule=(*it).second;
    ModuleConfigurationStructure& modexResources=configParser.getModuleConfiguration(resourcesModule);
    addConfiguration(modexResources,resources);
  }
  catch (NoSuchGroup& e) {
    LERROR << "missing param 'resourceList' in Resources for Modex in " << configFile;
    throw InvalidConfiguration();
  }
  catch (NoSuchParam& ) {
    LERROR << "missing param 'resourcesByLanguage' in Resources for Modex in " << configFile;
    throw InvalidConfiguration();
  }
  catch (NoSuchModule &) {
    LDEBUG << "no module 'Resources' for Modex in " << configFile;
    // not mandatory
  }

  try {
    ModuleConfigurationStructure& processors=LinguisticResources::changeable().getModuleConfiguration(lang);
    ModuleConfigurationStructure& modexProcessors=configParser.getModuleConfiguration("Processors");
    addConfiguration(modexProcessors,processors);
  }
  catch (NoSuchModule &) {
    LERROR << "no module 'Processors' for Modex in " << configFile;
    throw InvalidConfiguration();
  }

  // init modex
  initModexConfig(configParser);

  LINFO << "Modex initialization finished";
  TimeUtils::logElapsedTime("ModexInit");
}

void Modex::
addConfiguration(ModuleConfigurationStructure& modexConfig,
                 ModuleConfigurationStructure& commonConfig)
{
  // copy groups from modex resources to resources
  for (ModuleConfigurationStructure::const_iterator it=modexConfig.begin(),
         it_end=modexConfig.end(); it!=it_end; it++) {
    // check if exists (safe add)
    if (commonConfig.find((*it).first)!=commonConfig.end()) {
      MODEXLOGINIT;
      LERROR << "cannot add modex configuration '"<<(*it).first<<"' : already exists";
    }
    else {
      commonConfig.insert(make_pair((*it).first,(*it).second));
    }
  }
}

void Modex::
initEntities(const std::string& filename)
{
  XMLConfigurationFileParser configuration(Common::Misc::findFileInPaths(LinguisticData::single().getConfigPath().c_str(),filename.c_str()).toUtf8().constData());
  initEntities(configuration);
}

void Modex::
initEntities(XMLConfigurationFileParser& configParser)
{
  MODEXLOGINIT;
  LINFO << "intialize entities";

  ModuleConfigurationStructure& moduleConf=configParser.getModuleConfiguration("entities");
  
  // look at all groups : ModuleConfigurationStructure is a map
  try {
    for (ModuleConfigurationStructure::iterator it=moduleConf.begin(),
           it_end=moduleConf.end(); it!=it_end; it++) {
      LimaString groupName=Common::Misc::utf8stdstring2limastring((*it).first);
      LinguisticData::changeable().addEntityGroup(groupName);
      
      GroupConfigurationStructure& groupConf=(*it).second;
      
      deque<string>& entityList=groupConf.getListsValueAtKey("entityList");
      for (deque<string>::const_iterator ent=entityList.begin(),
             ent_end=entityList.end(); ent!=ent_end; ent++) {

        LimaString entityName=Common::Misc::utf8stdstring2limastring(*ent);
        LinguisticData::changeable().addEntity(groupName,entityName);
      }
    }
  }
  catch(NoSuchModule &) {
    MODEXLOGINIT;
    LERROR << "missing module 'entities' in modex configuration file";
    throw InvalidConfiguration();
  }
  catch(NoSuchList& ) {
    MODEXLOGINIT;
    LERROR << "missing list 'entityList' in modex configuration file";
    throw InvalidConfiguration();
  }
}

void Modex::
initModexConfig(XMLConfigurationFileParser& configParser)
{

  MODEXLOGINIT;
  LINFO << "intialize Modex";
  
  try {
    
    const deque<string>& modexList=
      configParser.getModuleConfiguration("modex").getGroupNamed("modexDefinitions").getListsValueAtKey("modexList");
    
    for (deque<string>::const_iterator it=modexList.begin(),
           it_end=modexList.end(); it!=it_end; it++) {
      const std::string& modexName=(*it);
      
      // get module for this modex
      ModuleConfigurationStructure& modexConf=configParser.getModuleConfiguration(modexName);
      
      // get group for general modex definition
      GroupConfigurationStructure& modexDef=modexConf.getGroupNamed("modexDefinition");
      
      // initialize all needed entities
      const deque<string>& includeEntities=modexDef.getListsValueAtKey("includeEntities");
      for (deque<string>::const_iterator entityFile=includeEntities.begin(),
             entityFile_end=includeEntities.end(); 
           entityFile!=entityFile_end; entityFile++) {
        if (*entityFile == "this") {
          initEntities(configParser);
        }
        else {
          // open file
          initEntities(*entityFile);
        }
      }
      
      // initialize entities in modex definition
//       const deque<string>& entities=modexDef.getListsValueAtKey("entities");
      // TODO... (is this necessary ?)
      
      // initialize pipeline
      const string& pipelineName=modexDef.getParamsValueAtKey("pipeline");
      m_pipeline=MediaProcessors::changeable().initPipeline(m_language,pipelineName);
    }
  } 
  catch (NoSuchModule &) {
    LERROR << "missing module 'modex' in modex configuration file";
    throw InvalidConfiguration();
  } 
  catch (NoSuchGroup& e) {
    LERROR << "missing group 'modexDefinitions' in modex configuration file";
    throw InvalidConfiguration();
  } 
  catch (NoSuchList& ) {
    LERROR << "missing list 'modexList' in modex configuration file";
    throw InvalidConfiguration();
  }
}


LimaStatusCode 
Modex::process(AnalysisContent& analysis) const
{
  return m_pipeline->process(analysis);
}
    



} // end namespace
} // end namespace
