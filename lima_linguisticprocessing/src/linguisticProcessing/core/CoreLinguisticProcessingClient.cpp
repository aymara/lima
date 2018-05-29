/*
    Copyright 2002-2020 CEA LIST

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
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef WIN32
#include <cstdint> //uint*_t
#endif
#include "CoreLinguisticProcessingClient.h"

#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/time/timeUtilsController.h"
#include "common/tools/FileUtils.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "common/MediaProcessors/MediaProcessors.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "common/MediaProcessors/MediaProcessUnitPipeline.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <QtCore/QDate>
#include <QtCore/QFileInfo>

uint64_t t1;

using namespace std;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;

//using namespace boost;

uint64_t t2;

namespace Lima
{

namespace LinguisticProcessing
{
std::unique_ptr<CoreLinguisticProcessingClientFactory> CoreLinguisticProcessingClientFactory::s_instance=std::unique_ptr<CoreLinguisticProcessingClientFactory>(new CoreLinguisticProcessingClientFactory());


CoreLinguisticProcessingClient::CoreLinguisticProcessingClient()
{
  //CORECLIENTLOGINIT;
  //LERROR << "CoreLinguisticProcessingClient::CoreLinguisticProcessingClient()";
}

CoreLinguisticProcessingClient::~CoreLinguisticProcessingClient()
{
  //CORECLIENTLOGINIT;
  //LERROR << "CoreLinguisticProcessingClient::~CoreLinguisticProcessingClient()";
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
    const LimaString& text,
    const std::map<std::string,std::string>& metaData,
    const std::string& pipelineId,
    const std::map<std::string, AbstractAnalysisHandler*>& handlers,
    const std::set<std::string>& inactiveUnits) const

{
  Lima::TimeUtilsController timer("CoreLinguisticProcessingClient::analyze");
  CORECLIENTLOGINIT;

 //if (text.isEmpty())
 static const QRegExp whitespaceOnly("\\s*");
 if (whitespaceOnly.exactMatch(text))
 {
   LWARN << "Empty text given to LIMA linguistic processing client. Nothing to do.";
   return;
 }

  // create analysis content
  AnalysisContent analysis;
  LinguisticMetaData* metadataholder=new LinguisticMetaData(); // will be destroyed in AnalysisContent destructor
  analysis.setData("LinguisticMetaData",metadataholder);

  metadataholder->setMetaData(metaData);
  LimaStringText* lstexte=new LimaStringText(text); // will be destroyed in AnalysisContent destructor
  analysis.setData("Text", lstexte);

  LINFO << "CoreLinguisticProcessingClient::analyze(";
  for(auto attrIt = metaData.cbegin() ; attrIt != metaData.cend() ; attrIt++ ) {
    LINFO << "attr:" << attrIt->first << "value:" << attrIt->second << ", " ;
  }
  LINFO;

  // add date/time/location metadata in LinguisticMetaData
#ifdef DEBUG_LP
  if (metaData.empty()) {
    LDEBUG << "CoreLinguisticProcessingClient::analyze: no metadata";
  }
#endif
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

#ifdef DEBUG_LP
        LDEBUG << "use '"<< date << "' as document date";
        LDEBUG << "use boost'"<< docDate.day() <<"/"<< docDate.month() <<"/"<< docDate.year() << "' as document date";
#endif
      }
      catch (std::exception& e) {
        LERROR << "Error in date conversion (date '"<< (*it).second
               << "' will be ignored): " << e.what();
      }
    }
    else if ((*it).first=="location") {
      metadataholder->setLocation("document",(*it).second);
#ifdef DEBUG_LP
        LDEBUG << "use '"<< (*it).second<< "' as document location";
#endif
    }
    else if ((*it).first=="time") {
      try {
        QTime docTime= QTime::fromString((*it).second.c_str(),"hh:mm:ss.z" );
        metadataholder->setTime("document",docTime);
#ifdef DEBUG_LP
        LDEBUG << "use '"<< (*it).second<< "' as document time";
#endif
      }
      catch (std::exception& e) {
        LERROR << "Error in ptime conversion (time '"<< (*it).second
               << "' will be ignored): " << e.what();
      }
    }
    else if ((*it).first=="docid") {
#ifdef DEBUG_LP
      LDEBUG << "use '"<< (*it).second<< "' as document id";
#endif
      metadataholder->setMetaData("DocId",(*it).second);
    }
  }

  std::string docId;
  try
  {
    docId = metadataholder->getMetaData("DocId");
  }
  catch (LinguisticProcessingException& )
  {
    metadataholder->setMetaData("DocId", docId);
  }
  LINFO << "CoreLinguisticProcessingClient::analyze(" << docId << "...)";


  // try to retrieve offset
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
#ifdef DEBUG_LP
  LDEBUG << "texte : " << text;
#endif
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
#ifdef DEBUG_LP
  LDEBUG << "add handler to analysis" ;
  for (auto hit = handlers.begin(); hit != handlers.end(); hit++)
  {
    LDEBUG << "    " << (*hit).first << (*hit).second;
  }
#endif
  AnalysisHandlerContainer* h = new AnalysisHandlerContainer(const_cast<std::map<std::string, AbstractAnalysisHandler*>& >(handlers));
#ifdef DEBUG_LP
  LDEBUG << "set data" ;
#endif
  analysis.setData("AnalysisHandlerContainer", h);

  // process analysis
#ifdef DEBUG_LP
  LDEBUG << "Process pipeline..." ;
#endif
  LimaStatusCode status=pipeline->process(analysis);
#ifdef DEBUG_LP
  LDEBUG << "pipeline process returned status " << (int)status ;
#endif
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
  //std::cerr << "CoreLinguisticProcessingClientFactory::CoreLinguisticProcessingClientFactory()" << std::endl;
  //std::cerr << "    calling AbstractLinguisticProcessingClientFactory(\"lima-coreclient\")" << std::endl;
}

CoreLinguisticProcessingClientFactory::~CoreLinguisticProcessingClientFactory()
{
  //std::cerr << "CoreLinguisticProcessingClientFactory::~CoreLinguisticProcessingClientFactory()" << std::endl;
}

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
    catch (NoSuchList& e)
    {
      LERROR << "no parameter lima-coreclient/mediaProcessingDefinitionFiles/available !" << e.what();
      throw InvalidConfiguration("no parameter lima-coreclient/mediaProcessingDefinitionFiles/available !");
    }
  }

  for (deque<string>::const_iterator langItr=langToload.begin();
       langItr!=langToload.end();
       langItr++)
  {
    LINFO << "CoreLinguisticProcessingClientFactory::configure load language " << *langItr;
    MediaId langid=MediaticData::single().getMediaId(*langItr);
    QString file;
    QString mediaProcessingDefinitionFile;
    try
    {
      QStringList configPaths = QString::fromUtf8(Common::MediaticData::MediaticData::single().getConfigPath().c_str()).split(LIMA_PATH_SEPARATOR);
      if (configPaths.isEmpty())
      {
        LERROR << "no config paths available in MediaticData";
        throw InvalidConfiguration("no config paths available in MediaticData");
      }
      QString mediaProcessingDefinitionFile = QString::fromUtf8(configuration.getModuleGroupParamValue(
            "lima-coreclient",
            "mediaProcessingDefinitionFiles",
            *langItr).c_str());
      Q_FOREACH(QString confPath, configPaths)
      {
        if (QFileInfo::exists(confPath + "/" + mediaProcessingDefinitionFile))
        {
          file = confPath + "/" + mediaProcessingDefinitionFile;
          break;
        }
      }
      if (file.isEmpty())
      {
        LERROR << "no language definition file"<< mediaProcessingDefinitionFile
               << "for language" << *langItr << "found in config paths"
               << configPaths;
        throw InvalidConfiguration("no language definition file for language ");
      }
    }
    catch (NoSuchParam& )
    {
      LERROR << "No such param lima-coreclient/mediaProcessingDefinitionFiles/" << *langItr;
      throw InvalidConfiguration("no language definition file for language ");
    }
    XMLConfigurationFileParser langParser(file.toUtf8().constData());

    //initialize SpecificEntities
    Common::MediaticData::MediaticData::changeable().initEntityTypes(langParser);

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
    catch (NoSuchModule& e)
    {
      LERROR << "no module 'Resources' in configuration file "
              << file << e.what();
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
    catch (NoSuchModule& e)
    {
      LERROR << "missing module 'Processors' in language configuration file "
              << file << e.what();
      throw InvalidConfiguration("missing module 'Processors' in language configuration file ");
    }

  }

  LINFO << "initialize Pipelines";
  try
  {
    GroupConfigurationStructure& group=configuration.getModuleGroupConfiguration("lima-coreclient","pipelines");
    MediaProcessors::changeable().initPipelines(group,pipelines);
  }
  catch (NoSuchModule& e)
  {
    LERROR << "no module 'pipelines' in lima-analysis.xml (configuration file)" << e.what();
    throw InvalidConfiguration("no module 'pipelines' in mm-LP.xml (configuration file)");
  }
}

std::shared_ptr< AbstractProcessingClient > CoreLinguisticProcessingClientFactory::createClient() const
{
  return std::shared_ptr< AbstractProcessingClient >(new CoreLinguisticProcessingClient());
}


} // LinguisticProcessing

} // Lima
