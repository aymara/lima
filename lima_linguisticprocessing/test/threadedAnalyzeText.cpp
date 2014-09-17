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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 *   Analyse de fichiers textes "bruts"                                    *
 *      Sortie : fichier *.bin                                             *
 ***************************************************************************/

#include "common/LimaCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"

#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "common/misc/gregoriannowarn.hpp"
#include "common/misc/posix_timenowarn.hpp"
#include <boost/program_options.hpp>

#include <boost/bind.hpp>//THREAD
#include <boost/thread.hpp>//THREAD
#include <boost/thread/mutex.hpp>

#include <QtCore/QCoreApplication>

namespace po = boost::program_options;

using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima;

#define STRING_SIZE_TYPE_MAX_VALUE numeric_limits<std::string::size_type>::max()

void listunits();
int dowork(int argc,char* argv[]);

class ProtectedFileList 
{
public:
  ProtectedFileList() {}
  
  std::string pop_front()
  {
    boost::mutex::scoped_lock(m_mutex);
    std::string result;
    if (m_list.empty())
      result = "ProtectedFileList::EMPTY";
    else
    {
      result = m_list.front();
      m_list.pop_front();
    }
    return result;
  }

  void push_back(const std::string& str)
  {
    m_list.push_back(str);
  }

private:
  ProtectedFileList(const ProtectedFileList& pfl);
  
  boost::mutex m_mutex;
  std::list<std::string> m_list;
};

int main(int argc,char* argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  bool docatch = false;
    if (argc>1)
    {
        for (int i = 1 ; i < argc; i++)
        {
            std::string arg(argv[i]);
            if (arg== "--catch")
                docatch = true;
        }
    }
    if (docatch)
    {
        try
        {
            std::cerr << "Doing work in try block." << std::endl;
            return dowork(argc, argv);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Catched an exception: " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Catched an unknown exception " << std::endl;
        }
    }
    else
        return dowork(argc,argv);
}

void analyze_thread(
                    AbstractLinguisticProcessingClient* client,
                    ProtectedFileList* files,
                    int use_thread,
                    std::map<std::string,std::string> metaDataCopy,
                    std::string pipeline,
                    std::set<std::string> inactiveUnits//,
                    )
{
  std::cerr << "thread " << use_thread<<": start analyze"<<std::endl;//THREAD
  std::string fileName = files->pop_front();
  std::map<std::string, AbstractAnalysisHandler*> handlers;
  
  while (fileName != "ProtectedFileList::EMPTY")
  {
    std::cerr << "thread " << use_thread << ": analyzing " << fileName << "'" << std::endl;
    std::map<std::string, AbstractAnalysisHandler*> handlers;

    // Set the handlers
    BowTextWriter* bowTextWriter = new BowTextWriter();
    handlers.insert(std::make_pair("bowTextWriter", bowTextWriter));
    SimpleStreamHandler* simpleStreamHandler = new SimpleStreamHandler();
    handlers.insert(std::make_pair("simpleStreamHandler", simpleStreamHandler));
    
    // set the output files
    std::ofstream bowofs((fileName+".bin").c_str(), std::ofstream::binary);
    bowTextWriter->setOut(&bowofs);
    std::ofstream txtofs((fileName+".out").c_str(), std::ofstream::binary);
    simpleStreamHandler->setOut(&txtofs);

    std::ifstream file(fileName.c_str(), std::ifstream::binary);
    std::string text_s;
    readStream(file, text_s);
    if (text_s.size() == 0)
    {
      std::cerr << "file " << fileName << " has empty input ! " << std::endl;
      continue;
    }
    
    LimaString contentText;
    // The input text MUST be UTF-8 encoded !!!
    contentText = utf8stdstring2limastring(text_s);
    TimeUtils::logElapsedTime("ReadInputFile");
    
    metaDataCopy["FileName"]=fileName;
    client->analyze(contentText,metaDataCopy,pipeline,handlers,inactiveUnits);//THREAD:lance l'analyseur linguistique

    bowofs.close();
    txtofs.close();
    fileName = files->pop_front();
  }
}

int dowork(int argc,char* argv[])
{
//   boost::mutex mutex;
  std::string resourcesPath;
  std::string configDir;
  std::string lpConfigFile;
  std::string commonConfigFile;
  std::string clientId;
  std::string cesartOutput;
  std::vector<std::string> languages;
  std::string pipeline;
  std::vector<std::string> files;
  std::vector<std::string> vinactiveUnits;
  std::string meta;
  int number_thread=1;//THREAD:nombre total de thread
  po::options_description desc("Usage");
  desc.add_options()
  ("help,h", "Display this help message")
  ("language,l", po::value< std::vector<std::string> >(&languages),
    "supported languages trigrams")
  ("mm-core-client", po::value<std::string>(&clientId)->default_value("lima-coreclient"),
    "Set the linguistic processing client to use")
  ("resources-dir", po::value<std::string>(&resourcesPath)->default_value(getenv("LIMA_RESOURCES")==0?"":getenv("LIMA_RESOURCES"),"$LIMA_RESOURCES"),
    "Set the directory containing the LIMA linguistic resources")
  ("config-dir", po::value<std::string>(&configDir)->default_value(getenv("LIMA_CONF")==0?"":getenv("LIMA_CONF"),"$LIMA_CONF"),
    "Set the directory containing the (LIMA) configuration files")
  ("common-config-file", po::value<std::string>(&commonConfigFile)->default_value("lima-common.xml"),
    "Set the LIMA common libraries configuration file to use")
  ("lp-config-file", po::value<std::string>(&lpConfigFile)->default_value("lima-analysis.xml"),
    "Set the linguistic processing configuration file to use")
  ("pipeline,p", po::value< std::string >(&pipeline)->default_value("main"),
    "Set the linguistic analysis pipeline to use")
  ("input-file", po::value< std::vector<std::string> >(&files),
    "Set a text file to analyze")
  ("inactive-units", po::value< std::vector<std::string> >(&vinactiveUnits),
    "Inactive some process units of the used pipeline")
  ("availableUnits", "Ask the program to list its known processing units")
  ("catch", "Executes the program inside a try/catch block")
  ("meta", po::value< std::string >(&meta), "Sets metadata values, in the format data1:value1,data2:value2,...")
  ("number_thread,t",po::value<int>(&number_thread),"Specify the number of threads to run on")
  ;
  po::positional_options_description p;
  p.add("input-file", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 1;
  }
  
  if (resourcesPath.empty())
  {
    resourcesPath = "/usr/share/apps/lima/resources/";
  }
  if (configDir.empty())
  {
    configDir = "/usr/share/config/lima/";
  }


  std::deque<std::string> langs(languages.size());
  std::copy(languages.begin(), languages.end(), langs.begin());

  if((number_thread<1)||(number_thread>1000))
  {
    std::cerr<<"ERREUR:le nombre de thread doit être compris entre 1 et 1000"<<std::endl;
    exit(1);
  }

  int use_thread=0;//THREAD:numero thread

  if (vm.count("availableUnits"))
  {
      listunits();
      return -1;
  }
  if (langs.size()<1)
  {
      std::cerr << "no language defined !" << std::endl;
      return -1;
  }

  std::vector<std::pair<std::string,std::string> > userMetaData;
  // parse 'meta' argument to add metadata
  std::string metaString(meta);
  std::string::size_type k=0;
  do
  {
      k=metaString.find(",");
      if (k==std::string::npos) continue;
      std::string str(metaString,0,k);
      std::string::size_type i=str.find(":");
      if (i==std::string::npos) {
        std::cerr << "meta argument '"<< str <<"' is not of the form XXX:YYY: ignored" << std::endl;
      }
      else {
          //cout << "add metadata " << string(str,0,i) << "=>" << string(str,i+1) << endl;
          userMetaData.push_back(std::make_pair(std::string(str,0,i),std::string(str,i+1)));
      }
      if (k!=std::string::npos) {
        metaString=std::string(metaString,k+1);
      }
  }  while (k!=std::string::npos);

  std::set<std::string> inactiveUnits;
  for (std::vector<std::string>::const_iterator it = vinactiveUnits.begin(); it != vinactiveUnits.end();it++)
  {
      inactiveUnits.insert(*it);
  }
  std::deque<std::string> pipelines;

  pipelines.push_back(pipeline);

  uint64_t beginTime=TimeUtils::getCurrentTime();

  AbstractLinguisticProcessingClient* client(0);

  try
  {
    // initialize common
    Common::MediaticData::MediaticData::changeable().init(
        resourcesPath,
        configDir,
        commonConfigFile,
        langs);

    // initialize linguistic processing
    Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configDir + "/" + lpConfigFile);
    LinguisticProcessingClientFactory::changeable().configureClientFactory(
        clientId,
        lpconfig,
        langs,
        pipelines);

    client=dynamic_cast<AbstractLinguisticProcessingClient*>(LinguisticProcessingClientFactory::single().createClient(clientId));

    std::map<std::string,std::string> metaData;

    metaData["Lang"]=langs[0];
    for (std::vector<std::pair<std::string,std::string> >::const_iterator
            it=userMetaData.begin(),it_end=userMetaData.end();
            it!=it_end; it++) {
        metaData[(*it).first]=(*it).second;
    }

    ProtectedFileList fileList;
    boost::thread_group threads;//THREAD:initialisation groupes threads
    for (std::vector<std::string>::iterator fileItr=files.begin();fileItr!=files.end();fileItr++)
    {
      fileList.push_back(*fileItr);
    }
    for (int i = 0; i != number_thread; i++)
    {
      use_thread++;//THREAD:incrémente thread courant
      std::map<std::string,std::string> metaDataCopy = metaData;
      TimeUtils::updateCurrentTime();
      
      threads.create_thread(boost::bind(&analyze_thread, client, &fileList, use_thread, metaDataCopy, pipeline, inactiveUnits));//THREAD:lancement de l'analyse en parallèle sur plusieurs threads
    }
    threads.join_all();//THREAD:blocage des threads jusqu'à ce que tous les threads finissent

  }

  catch (InvalidConfiguration& e)
  {
      throw e;
  }
  /*
    catch (std::exception& e)
    {
      std::cerr << "Catched exception: " << e.what() << std::endl;
      throw;
    }
  */
  delete client;
  TIMELOGINIT;
  LINFO << "Total: " << TimeUtils::diffTime(beginTime,TimeUtils::getCurrentTime()) << " ms";

  return SUCCESS_ID;
}

void listunits()
{
    {
      std::cout << "Available resources factories : " << std::endl;
      std::deque<std::string> ids=AbstractResource::Factory::getRegisteredFactories();
      for (std::deque<std::string>::const_iterator it=ids.begin();
                it!=ids.end();
                it++)
        {
          std::cout << "- " << *it << std::endl;
        }
        std::cout << std::endl;
    }
    {
      std::cout << "Available process units factories : " << std::endl;
      std::deque<std::string> ids=MediaProcessUnit::Factory::getRegisteredFactories();
      for (std::deque<std::string>::const_iterator it=ids.begin();
                it!=ids.end();
                it++)
        {
          std::cout << "- " << *it << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "Available client factories are : " << std::endl;
    {
      std::deque<std::string> ids=LinguisticProcessingClientFactory::single().getRegisteredFactories();
      for (std::deque<std::string>::iterator it=ids.begin();
                it!=ids.end();
                it++)
        {
          std::cout << "- " << *it << std::endl;
        }
        std::cout << std::endl;
    }
    exit(0);
}
