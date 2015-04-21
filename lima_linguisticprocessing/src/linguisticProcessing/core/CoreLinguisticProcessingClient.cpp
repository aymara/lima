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
#ifdef WIN32
#define _WINSOCKAPI_
#endif

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "CoreLinguisticProcessingClient.h"

#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/time/timeUtilsController.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "common/MediaProcessors/MediaProcessors.h"
#include "linguisticProcessing/core/LinguisticProcessors/LimaStringText.h"
#include "common/MediaProcessors/MediaProcessUnitPipeline.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <QtCore/QDate>

using namespace std;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;

using namespace boost;

namespace Lima
{

namespace LinguisticProcessing
{
CoreLinguisticProcessingClientFactory* CoreLinguisticProcessingClientFactory::s_instance=new CoreLinguisticProcessingClientFactory();
  
  
CoreLinguisticProcessingClient::CoreLinguisticProcessingClient()
{}

CoreLinguisticProcessingClient::~CoreLinguisticProcessingClient() {
  delete LinguisticResources::pchangeable();
  delete MediaProcessors::pchangeable();
}

void CoreLinguisticProcessingClient::analyze(
    const std::string& texte,
    const std::map<std::string,std::string>& metaData,
    const std::string& pipelineId,
    const std::map<std::string, AbstractAnalysisHandler*>& handlers,
    const std::set<std::string>& inactiveUnits) const

{
  LimaString limatexte=Common::Misc::utf8stdstring2limastring(texte);
  
  analyze(limatexte,metaData,pipelineId,handlers,inactiveUnits);
}

void CoreLinguisticProcessingClient::analyze(
    const LimaString& texte,
    const std::map<std::string,std::string>& metaData,
    const std::string& pipelineId,
    const std::map<std::string, AbstractAnalysisHandler*>& handlers,
    const std::set<std::string>& inactiveUnits) const

{
  Lima::TimeUtilsController timer("CoreLinguisticProcessingClient::analyze");
  CORECLIENTLOGINIT;
  // create analysis content
  AnalysisContent analysis;
  LinguisticMetaData* metadataholder=new LinguisticMetaData(); // will be destroyed in AnalysisContent destructor
  analysis.setData("LinguisticMetaData",metadataholder);

  metadataholder->setMetaData(metaData);
  LimaStringText* lstexte=new LimaStringText(texte);
  analysis.setData("Text",lstexte);
  
  LINFO << "CoreLinguisticProcessingClient::analyze(";
  for( std::map<std::string,std::string>::const_iterator attrIt = metaData.begin() ;
	attrIt != metaData.end() ; attrIt++ ) {
	LINFO << "attr:" << attrIt->first << "value:" << attrIt->second << ", " ;
  }
  LINFO;
  
  std::map<std::string,std::string>* metaDataPtr = const_cast<std::map<std::string,std::string>*>(&metaData);
  LINFO << "CoreLinguisticProcessingClient::analyze(" << (*metaDataPtr)["docid"] << "...)";

  // add date/time/location metadata in LinguisticMetaData
  if (metaData.empty()) {
    LDEBUG << "CoreLinguisticProcessingClient::analyze: no metadata";
  }
  for (map<string,string>::const_iterator it=metaData.begin(),
         it_end=metaData.end(); it!=it_end; it++) {
    if ((*it).first=="date") {
      try {
        const std::string& str=(*it).second;
        uint64_t i=str.find("T"); //2006-12-11T12:44:00
        /*if (i!=std::string::npos) {
          QTime docTime=posix_time::time_from_string(str);
          metadataholder->setTime("document",docTime);
          LDEBUG << "use '"<< str << "' as document time";
          }*/
        string date(str,0,i);
        QDate docDate=QDate::fromString(date.c_str(),Qt::ISODate);
        metadataholder->setDate("document",docDate);
        
        LDEBUG << "use '"<< date << "' as document date";
        LDEBUG << "use boost'"<< docDate.day() <<"/"<< docDate.month() <<"/"<< docDate.year() << "' as document date";
      }
      catch (std::exception& e) {
        LERROR << "Error in date conversion (date '"<< (*it).second
               << "' will be ignored): " << e.what();
      }
    }
    else if ((*it).first=="location") {
      metadataholder->setLocation("document",(*it).second);
        LDEBUG << "use '"<< (*it).second<< "' as document location";
    }
    else if ((*it).first=="time") {
      try {
        QTime docTime= QTime::fromString((*it).second.c_str(),"hh:mm:ss.z" );
        metadataholder->setTime("document",docTime);
        LDEBUG << "use '"<< (*it).second<< "' as document time";
      }
      catch (std::exception& e) {
        LERROR << "Error in ptime conversion (time '"<< (*it).second
               << "' will be ignored): " << e.what();
      }
    }
    else if ((*it).first=="docid") {
      LDEBUG << "use '"<< (*it).second<< "' as document id";
      metadataholder->setMetaData("DocId",(*it).second);
    }
  }
 
  // try to retreive offset
  try
  {
    const std::string& offsetStr=metadataholder->getMetaData("StartOffset");
    metadataholder->setStartOffset(atoi(offsetStr.c_str()));
  }
  catch (LinguisticProcessingException& )
  {
    metadataholder->setStartOffset(0);
  }

  const std::string& fileName=metadataholder->getMetaData("FileName");
  // get language
  const std::string& lang=metadataholder->getMetaData("Lang");
  LINFO  << "analyze file is: '" << fileName << "'";
  LINFO  << "analyze pipeline is '" << pipelineId << "'";
  LINFO  << "analyze language is '" << lang << "'";
  LDEBUG << "texte : " << texte;
  //LDEBUG << "texte : " << Common::Misc::limastring2utf8stdstring(texte);

  MediaId langId=MediaticData::single().getMediaId(lang);

  // get pipeline
  const MediaProcessUnitPipeline* pipeline=MediaProcessors::single().getPipelineForId(langId,pipelineId);
  if (pipeline==0)
  {
    LERROR << "can't get pipeline '" << pipelineId << "'";
    throw LinguisticProcessingException( std::string("can't get pipeline '" + pipelineId + "'") );
  }
  InactiveUnitsData* inactiveUnitsData = new InactiveUnitsData();
  for (std::set<std::string>::const_iterator it = inactiveUnits.begin(); it != inactiveUnits.end(); it++)
  {
//     const_cast<MediaProcessUnitPipeline*>(pipeline)->setInactiveProcessUnit(*it);
    inactiveUnitsData->insert(*it);
  }
  analysis.setData("InactiveUnits", inactiveUnitsData);
  
  // add handler to analysis
  LDEBUG << "add handler to analysis" ;
  for (auto hit = handlers.begin(); hit != handlers.end(); hit++)
  {
    LDEBUG << "    " << (*hit).first << (*hit).second;
  }
  AnalysisHandlerContainer* h = new AnalysisHandlerContainer(const_cast<std::map<std::string, AbstractAnalysisHandler*>& >(handlers));
  LDEBUG << "set data" ;
  analysis.setData("AnalysisHandlerContainer", h);

  // process analysis
  LDEBUG << "Process pipeline..." ;
  LimaStatusCode status=pipeline->process(analysis);
  LDEBUG << "pipeline process returned status " << (int)status ;
  if (status!=SUCCESS_ID)
  {
    std::stringstream s_mess;
    s_mess << "analysis failed : receive status " << (int)status << " from pipeline. exit";
    LERROR << s_mess.str();
    throw LinguisticProcessingException( s_mess.str() );
  }
}

CoreLinguisticProcessingClientFactory::CoreLinguisticProcessingClientFactory() :
  AbstractLinguisticProcessingClientFactory("lima-coreclient")
{
//   std::cerr << "CoreLinguisticProcessingClientFactory::CoreLinguisticProcessingClientFactory()" << std::endl;
//   std::cerr << "    calling AbstractLinguisticProcessingClientFactory(\"lima-coreclient\")" << std::endl;
}

CoreLinguisticProcessingClientFactory::~CoreLinguisticProcessingClientFactory()
{}

void CoreLinguisticProcessingClientFactory::configure(
  Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
  std::deque<std::string> langs,
  std::deque<std::string> pipelines)
{
  Lima::TimeUtilsController timer("LPCoreClientInit");
  LPCLIENTFACTORYLOGINIT;
  LINFO << "CoreLinguisticProcessingClientFactory::configure";
  
  // initialize some entity types internally used in linguistic processing
  Common::MediaticData::MediaticData::changeable().initEntityTypes(configuration);

  deque<string> langToload(langs);
  if (langToload.empty())
  {
    try
    {
      langToload=configuration.getModuleGroupListValues(
                   "lima-coreclient",
                   "mediaProcessingDefinitionFiles",
                   "available");
    }
    catch (NoSuchList& )
    {
      LERROR << "no parameter lima-coreclient/mediaProcessingDefinitionFiles/available !";
      throw InvalidConfiguration("no parameter lima-coreclient/mediaProcessingDefinitionFiles/available !");
    }
  }

  string configPath=Common::MediaticData::MediaticData::single().getConfigPath();
  for (deque<string>::const_iterator langItr=langToload.begin();
       langItr!=langToload.end();
       langItr++)
  {
    LINFO << "CoreLinguisticProcessingClientFactory::configure load language " << *langItr;
    MediaId langid=MediaticData::single().getMediaId(*langItr);
    string file;
    try
    {
      file=configPath + "/" + configuration.getModuleGroupParamValue(
             "lima-coreclient",
             "mediaProcessingDefinitionFiles",
             *langItr);
    }
    catch (NoSuchParam& )
    {
      LERROR << "no language definition file for language " << *langItr;
      throw InvalidConfiguration("no language definition file for language ");
    }

    XMLConfigurationFileParser langParser(file);

    // initialize resources
    LINFO << "configure resources for language " << *langItr;
    try
    {
      ModuleConfigurationStructure& module=langParser.getModuleConfiguration("Resources");
      LinguisticResources::changeable().initLanguage(
        langid,
        module,
        true); // load main keys
    }
    catch (NoSuchModule& )
    {
      LERROR << "no module 'Resources' in configuration file " << file;
      throw InvalidConfiguration("no module 'Resources' in configuration file ");
    }

    // initialize processors
    LINFO << "initialize processors";
    try
    {
      ModuleConfigurationStructure& procmodule=langParser.getModuleConfiguration("Processors");
      MediaProcessors::changeable().initMedia(
        langid,
        procmodule/*,
        dumpmodule*/);
    }
    catch (NoSuchModule& )
    {
      LERROR << "missing module 'Processors' in language configuration file " << file;
      throw InvalidConfiguration("missing module 'Processors' in language configuration file ");
    }

  }

  LINFO << "initialize Pipelines";
  try
  {
    GroupConfigurationStructure& group=configuration.getModuleGroupConfiguration("lima-coreclient","pipelines");
    MediaProcessors::changeable().initPipelines(group,pipelines);
  }
  catch (NoSuchModule& )
  {
    LERROR << "no module 'pipelines' in lima-analysis.xml (configuration file)";
    throw InvalidConfiguration("no module 'pipelines' in mm-LP.xml (configuration file)");
  }
}

AbstractLinguisticProcessingClient* CoreLinguisticProcessingClientFactory::createClient() const
{
  return new CoreLinguisticProcessingClient();
}


} // LinguisticProcessing

} // Lima
