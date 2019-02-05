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
 ***************************************************************************/

#include "common/LimaCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"

#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
// #include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <QtCore/QCoreApplication>

using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima;
using namespace std;

void usage(int argc, char* argv[]);
void listunits();

#include "common/tools/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

int run(int aargc,char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  LimaMainTaskRunner* task = new LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, SIGNAL(finished(int)), &a, SLOT(quit()));

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();

}


int run(int argc,char** argv)
{
  QsLogging::initQsLog();
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  
  string resourcesPath=string(getenv("LIMA_RESOURCES"));
  string configDir=string(getenv("LIMA_CONF"));
  if (resourcesPath.empty())
  {
    resourcesPath = "/usr/share/apps/lima/resources/";
  }
  if (configDir.empty())
  {
    configDir = "/usr/share/config/lima/";
  }



  string lpConfigFile=string("lima-analysis.xml");
  string commonConfigFile=string("lima-common.xml");
  string pipeline=string("main");
  string clientId=string("lima-coreclient");

  deque<string> langs;
  deque<string> pipelines;

  if (argc>1)
  {
    for (int i = 1 ; i < argc; i++)
    {
      std::string arg(argv[i]);
      std::string::size_type pos = std::string::npos;
      if ( arg[0] == '-' )
      {
        if ( (pos = arg.find("--lp-config-file=")) != std::string::npos )
          lpConfigFile = arg.substr(pos+17);
        else if ( (pos = arg.find("--common-config-file=")) != std::string::npos )
          commonConfigFile = arg.substr(pos+21);
        else if ( (pos = arg.find("--config-dir=")) != std::string::npos )
          configDir = arg.substr(pos+13);
        else if ( (pos = arg.find("--resources-dir=")) != std::string::npos )
          resourcesPath = arg.substr(pos+16);
        else if ( (pos = arg.find("--language=")) != std::string::npos )
          langs.push_back(arg.substr(pos+11));
        else if ( (pos = arg.find("--pipeline=")) != std::string::npos )
          pipeline = arg.substr(pos+11);
        else if ( (pos = arg.find("--client=")) != std::string::npos )
          clientId=arg.substr(pos+9);
        else usage(argc, argv);
      }
    }
  }
  
  pipelines.push_back(pipeline);
  
  if (langs.size()<1)
  {
    std::cerr << "no language defined !" << std::endl;
    return -1;
  }

  uint64_t beginTime=TimeUtils::getCurrentTime();
    
  std::map<std::string, AbstractAnalysisHandler*> handlers;
  
  std::shared_ptr< AbstractLinguisticProcessingClient > client;
  try
  {
    // initialize common
    MediaticData::changeable().init(
      resourcesPath,
      configDir,
      commonConfigFile,
      langs);

    // initialize linguistic processing
    Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(
      QString::fromUtf8(configDir.c_str()) + "/" + lpConfigFile.c_str());
    LinguisticProcessingClientFactory::changeable().configureClientFactory(
      clientId,
      lpconfig,
      langs,
      pipelines);

    client = std::dynamic_pointer_cast<AbstractLinguisticProcessingClient>(LinguisticProcessingClientFactory::single().createClient(clientId));
  }
  catch (InvalidConfiguration& e)
  {
  std::cout << "ERROR: Invalid configuration: " << e.what() << "." << std::endl;
  }
  catch (std::exception& e)
  {
  std::cout << "ERROR: " << e.what() << "." << std::endl;
  }
  catch (...)
  {
  std::cout << "ERROR: unknown error." << std::endl;
  }
  
    map<string,string> metaData;

    std::string command;
    char buf[255];
    
    while (true)
    {
      try
      {
        std::cout << "analyzer\n";
        std::cin.getline(buf,255);
        buf[254] = 0;
        string commandline(buf);
        vector<string> commands;
        size_t index=0;
        while (true) 
        {
          size_t next=commandline.find_first_of(";",index);
          if (next!=string::npos)
          {
            commands.push_back(commandline.substr(index,next-index));
            index=next+1;
          } 
          else 
          {
            commands.push_back(commandline.substr(index));
            break;
          }
        }
        if (commands.size() != 3)
        {
          std::cerr << "Error: 3 arguments waited. Not " << commands.size() << std::endl;
          continue;
        }
        command = commands[0];
        std::string lang = commands[1];
        std::string file = commands[2];
        
        
        // loading of the input file
        TimeUtils::updateCurrentTime();
        std::ifstream fstr(file.c_str(), std::ifstream::binary);
        std::string text_s;
        readStream(fstr, text_s);
        if (text_s.size() == 0)
        {
          std::cerr << "file " << file << " has empty input ! " << std::endl;
          continue;
        }
        
        LimaString contentText;
        // The input text MUST be UTF-8 encoded !!!
        contentText = utf8stdstring2limastring(text_s);
        TimeUtils::logElapsedTime("ReadInputFile");
  
        // set the handler
        TimeUtils::updateCurrentTime();
        metaData["Lang"]=lang;
        metaData["FileName"]=file;
        
        std::map<std::string, AbstractAnalysisHandler*> handlers;
        if (command == "abd")
        {
//           dumper = "bowDumper";
/*          string outputFile=file + ".bin";
          std::ofstream fout(outputFile.c_str(), std::ofstream::binary);
          BowTextWriter bowWriter;
          bowWriter.setOut(&fout);
          client->setAnalysisHandler(&bowWriter);*/
          TimeUtils::logElapsedTime("PrepareAnalysis");
        
          // analyze it
          client->analyze(contentText,metaData,pipeline,handlers);
    
          // close stream
//           fout.close();
          std::cout << "analysis done" << std::endl;
        }
        else if (command == "afxd")
        {
//           dumper = "fullXmlDumper";
/*          std::string outputFile = file+ ".anal.xml";
          std::ofstream fout(outputFile.c_str(), std::ofstream::binary);
          SimpleStreamHandler handler;
          handler.setOut(&fout);
          client->setAnalysisHandler(&handler);*/
          TimeUtils::logElapsedTime("PrepareAnalysis");
        
          // analyze it
          client->analyze(contentText,metaData,pipeline,handlers);
          
          // close stream
//           fout.close();
          std::cout << "analysis done" << std::endl;
        }
        else if (command == "aagxd")
        {
//           dumper = "agXmlDumper";
/*          string outputFile=file + ".ag.xml";
          std::ofstream fout(outputFile.c_str(), std::ofstream::binary);
          SimpleStreamHandler handler;
          handler.setOut(&fout);
          client->setAnalysisHandler(&handler);*/
          TimeUtils::logElapsedTime("PrepareAnalysis");
  
          // analyze it
          client->analyze(contentText,metaData,pipeline,handlers);
          
          // close stream
//           fout.close();
          std::cout << "analysis done" << std::endl;
        }
        else if (command == "atd" || command == "acfd")
        {
          if (command == "atd")
          {
//             dumper = "textDumper";
          }
          else
          {
//             dumper = "compactFullDumper";
          }
/*          string outputFile=file + ".out";
          std::ofstream fout(outputFile.c_str(), std::ofstream::binary);
          SimpleStreamHandler handler;
          handler.setOut(&fout);
          client->setAnalysisHandler(&handler);*/
          TimeUtils::logElapsedTime("PrepareAnalysis");
  
          // analyze it
          client->analyze(contentText,metaData,pipeline,handlers);
          
          // close stream
//           fout.close();
          std::cout << "analysis done" << std::endl;
        }
        else if (command == "altrd") 
        {
//           dumper = "ltrd";
/*          string outputFile = file + ".bin";
          std::ofstream fout(outputFile.c_str(), std::ofstream::binary);
          SimpleStreamHandler handler;
          handler.setOut(&fout);
          client->setAnalysisHandler(&handler);*/
          TimeUtils::logElapsedTime("PrepareAnalysis");
          // analyze it
          client->analyze(contentText,metaData,pipeline,handlers);
          // close stream
//           fout.close();
          std::cout << "analysis done" << std::endl;
        }
        else if (command == "aexd")
        {
//           dumper = "easyXmlDumper";
/*          std::string outputFile = file+ ".easy.xml";
          std::ofstream fout(outputFile.c_str(), std::ofstream::binary);
          QTime time_t_epoch(QDate(1970,1,1));
          QDate today = boost::gregorian::day_clock::local_day();
          boost::posix_time::time_duration diff = boost::posix_time::second_clock::universal_time() - time_t_epoch;
          fout << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
          fout << "<!DOCTYPE DOCUMENT SYSTEM \"easy.dtd\">" << std::endl;
          fout << "<DOCUMENT fichier=\"" << file << "\""
              << " id=\"cea" << diff.total_seconds() << "\" date=\"" << boost::gregorian::day_clock::local_day() << "\""
              << " xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << std::endl;
            SimpleStreamHandler handler;
            handler.setOut(&fout);
            client->setAnalysisHandler(&handler);*/
          TimeUtils::logElapsedTime("PrepareAnalysis");
          // analyze it
          client->analyze(contentText,metaData,pipeline,handlers);
          //           fout << "</DOCUMENT>" << std::endl;
          // close stream
//           fout.close();
          std::cout << "analysis done" << std::endl;
        }
        else
          std::cout << "ERROR: Unknown dumper." << std::endl;
    }
    catch (InvalidConfiguration& e)
    {
    std::cout << "ERROR: Invalid configuration: " << e.what() << "." << std::endl;
    }
    catch (std::exception& e)
    {
    std::cout << "ERROR: " << e.what() << "." << std::endl;
    }
    catch (...)
    {
    std::cout << "ERROR: unknown error." << std::endl;
    }
  }
  TIMELOGINIT;
  LINFO << "Total: " << TimeUtils::diffTime(beginTime,TimeUtils::getCurrentTime()) << " ms";
  
  return SUCCESS_ID;
}

void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
  std::cout << "usage: " << argv[0] << " [OPTIONS] [file1 [file2 [...]]] " << std::endl;
  std::cout << "  --resources-dir=</path/to/the/resources> Optional. Default is $LIMA_RESOURCES" << std::endl;
  std::cout << "  --config-dir=</path/to/the/configuration/directory> Optional. Default is $LIMA_CONF" << std::endl;
  std::cout << "  --lp-config-file=<configuration/file/name>  Optional. Default is lima-analysis.xml" << std::endl;
  std::cout << "  --common-config-file=<configuration/file/name>  Optional. Default is lima-common.xml" << std::endl;
  std::cout << "  --client=<clientId> Optional. Default is 'lima-coreclient'" << std::endl;
  std::cout << "  --language=<language trigram> Optional. Language of document to analyze." << std::endl;
  std::cout << "  --pipeline=<pipelineId> Optional. Default is 'main'" << std::endl;
  std::cout << "  --availableUnits  show all available resources, processUnits and dumpers" << std::endl;
  std::cout << "  where files are files to analyze." << std::endl;
  std::cout << endl;
  std::cout << "Available client factories are : " << std::endl;
  {
    deque<string> ids=LinguisticProcessingClientFactory::single().getRegisteredFactories();
    for (deque<string>::iterator it=ids.begin();
         it!=ids.end();
         it++)
    {
      cout << "- " << *it << endl;
    }
    cout << endl;
  }
  exit(0);
}


void listunits()
{
  {
    cout << "Available resources factories : " << endl;
    deque<string> ids=AbstractResource::Factory::getRegisteredFactories();
    for (deque<string>::const_iterator it=ids.begin();
         it!=ids.end();
         it++)
    {
      cout << "- " << *it << endl;
    }
    cout << endl;
  }
  {
    cout << "Available process units factories : " << endl;
    deque<string> ids=MediaProcessUnit::Factory::getRegisteredFactories();
    for (deque<string>::const_iterator it=ids.begin();
         it!=ids.end();
         it++)
    {
      cout << "- " << *it << endl;
    }
    cout << endl;
  }
  std::cout << "Available client factories are : " << std::endl;
  {
    deque<string> ids=LinguisticProcessingClientFactory::single().getRegisteredFactories();
    for (deque<string>::iterator it=ids.begin();
         it!=ids.end();
         it++)
    {
      cout << "- " << *it << endl;
    }
    cout << endl;
  }
  exit(0);
}
