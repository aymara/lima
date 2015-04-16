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
 *   Copyright (C) 2004-2014 by CEA LIST
 *
 *   Main LIMA executable
 *
 ***************************************************************************/

#include "common/LimaCommon.h"
#include "common/tools/LimaMainTaskRunner.h"
#include "common/MediaticData/mediaticData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/QsLog/QsLog.h"
#include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/QsLog/QsDebugOutput.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"

#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "linguisticProcessing/core/EventAnalysis/EventHandler.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <boost/program_options.hpp>

#include <QtCore>

namespace po = boost::program_options;

using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima;

void listunits();
std::ostream* openHandlerOutputFile(AbstractTextualAnalysisHandler* handler, const std::string& fileName, const std::set< std::string >& dumpers, const std::string& dumperId);
void closeHandlerOutputFile(std::ostream* ofs);
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
  // Necessary to initialize factories under Windows
  Lima::AmosePluginsManager::single();
  //   std::cerr << "Amose plugins initialized" << std::endl;

  std::string resourcesPath;
  std::string configDir;
  std::string lpConfigFile;
  std::string commonConfigFile;
  std::string clientId;
  std::string cesartOutput;
  std::vector<std::string> languages;
  std::vector<std::string> dumpersv;
  std::vector<std::string> outputsv;
  std::string pipeline;
  std::vector<std::string> files;
  std::vector<std::string> vinactiveUnits;
  std::string meta;
  
  
  po::options_description desc("Usage");
  desc.add_options()
  ("help,h", "Display this help message")
  ("language,l", po::value< std::vector<std::string> >(&languages),
   "supported languages trigrams")
  ("dumper,d", po::value< std::vector<std::string> >(&dumpersv),
   "a dumper to use, can be repeated. Valid values are [bow (BowTextWriter),bowh (BowTextHandler),fullxml (SimpleStreamHandler),text (SimpleStreamHandler), event (EventHandler)]. To use any of them, the corresponding dumper must be available in the pipeline configuration. Default is bow but must be set if any other is set")
  ("output,o", po::value< std::vector<std::string> >(&outputsv),
   "where to write dumpers output. By default, each dumper writes its results on a file whose name is the input file with a predefined suffix  appended. This option allows to chose another suffix or to write on standard output. Its syntax  is the following: <dumper>:<destination> with <dumper> a  dumper name and destination, either the value 'stdout' or a suffix.")
  ("mm-core-client", po::value<std::string>(&clientId)->default_value("lima-coreclient"),
                                                                      "Set the linguistic processing client to use")
  ("resources-dir", po::value<std::string>(&resourcesPath)->default_value(qgetenv("LIMA_RESOURCES").constData()==0?"":qgetenv("LIMA_RESOURCES").constData(),"$LIMA_RESOURCES"),
                                                                                                                              "Set the directory containing the LIMA linguistic resources")
  ("config-dir", po::value<std::string>(&configDir)->default_value(qgetenv("LIMA_CONF").constData()==0?"":qgetenv("LIMA_CONF").constData(),"$LIMA_CONF"),
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
  ;
  
  po::positional_options_description p;
  p.add("input-file", -1);
  
  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).
    options(desc).positional(p).run(), vm);
    po::notify(vm);
  } catch (const boost::program_options::unknown_option& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return SUCCESS_ID;
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
  
  std::set<std::string> dumpers;
  if (dumpersv.empty())
  {
    dumpers.insert("text");
  }
  else
  {
    for (uint64_t i = 0; i < dumpersv.size(); i++)
    {
      dumpers.insert(dumpersv[i]);
    }
  }
  
  if (vm.count("availableUnits"))
  {
    listunits();
    return SUCCESS_ID;
  }
  if (langs.size()<1)
  {
    std::cerr << "no language defined !" << std::endl;
    return 1;
  }
  
  QMap< QString, QString > outputs;
  for(std::vector<std::string>::const_iterator outputsIt = outputsv.begin();
      outputsIt != outputsv.end(); outputsIt++)
  {
    QStringList output = QString::fromUtf8((*outputsIt).c_str()).split(":");
    if (output.size()==2)
    {
      outputs[output[0]] = output[1];
    }
    else
    {
      // Option syntax  error
      std::cerr << "syntax error in output setting:" << *outputsIt << std::endl;
    }
  }
  std::vector<std::pair<std::string,std::string> > userMetaData;
  // parse 'meta' argument to add metadata
  if(!meta.empty()) {
    std::string metaString(meta);
    std::string::size_type k=0;
    do {
      k=metaString.find(",");
      //if (k==std::string::npos) continue;
      std::string str(metaString,0,k);
      std::string::size_type i=str.find(":");
      if (i==std::string::npos) {
        std::cerr << "meta argument '"<< str <<"' is not of the form XXX:YYY: ignored" << std::endl;
      }
      else {
        //std::cout << "add metadata " << std::string(str,0,i) << "=>" << std::string(str,i+1) << std::endl;
        userMetaData.push_back(std::make_pair(std::string(str,0,i),std::string(str,i+1)));
      }
      if (k!=std::string::npos) {
        metaString=std::string(metaString,k+1);
      }
    }  while (k!=std::string::npos);
  }
  
  std::set<std::string> inactiveUnits;
  for (std::vector<std::string>::const_iterator it = vinactiveUnits.begin(); it != vinactiveUnits.end();it++)
  {
    inactiveUnits.insert(*it);
  }
  std::deque<std::string> pipelines;

  pipelines.push_back(pipeline);
  
  uint64_t beginTime=TimeUtils::getCurrentTime();
  
  AbstractLinguisticProcessingClient* client(0);
  
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
  
  client=static_cast<AbstractLinguisticProcessingClient*>(LinguisticProcessingClientFactory::single().createClient(clientId));
  
  // Set the handlers
  std::map<std::string, AbstractAnalysisHandler*> handlers;
  BowTextWriter* bowTextWriter = 0;
  EventAnalysis::EventHandler* eventHandler = 0;
  BowTextHandler* bowTextHandler = 0;
  SimpleStreamHandler* simpleStreamHandler = 0;
  SimpleStreamHandler* fullXmlSimpleStreamHandler = 0;
  
  if (dumpers.find("event") != dumpers.end())
  {
    eventHandler = new EventAnalysis::EventHandler();
    handlers.insert(std::make_pair("eventHandler", eventHandler));
  }
  if (dumpers.find("bow") != dumpers.end())
  {
    bowTextWriter = new BowTextWriter();
    handlers.insert(std::make_pair("bowTextWriter", bowTextWriter));
  }
  if (dumpers.find("bowh") != dumpers.end())
  {
    bowTextHandler = new BowTextHandler();
    handlers.insert(std::make_pair("bowTextHandler", bowTextHandler));
  }
  if (dumpers.find("text") != dumpers.end())
  {
    simpleStreamHandler = new SimpleStreamHandler();
    handlers.insert(std::make_pair("simpleStreamHandler", simpleStreamHandler));
  }
  if (dumpers.find("fullxml") != dumpers.end())
  {
    fullXmlSimpleStreamHandler = new SimpleStreamHandler();
    handlers.insert(std::make_pair("fullXmlSimpleStreamHandler", fullXmlSimpleStreamHandler));
  }
  
  std::map<std::string,std::string> metaData;
  
  metaData["Lang"]=langs[0];
  for (std::vector<std::pair<std::string,std::string> >::const_iterator
    it=userMetaData.begin(),it_end=userMetaData.end();
  it!=it_end; it++) {
    metaData[(*it).first]=(*it).second;
  }
  
  uint64_t i=1;
  for (std::vector<std::string>::iterator fileItr=files.begin();
       fileItr!=files.end();
  fileItr++, i++)
  {
    // display the progress of the analysis
    std::cout << "\rAnalyzing "<< i << "/" << files.size()
    << " ("  << std::setiosflags(std::ios::fixed) << std::setprecision(2) << (i*100.0/files.size()) <<"%) '"
    << *fileItr << "'" << std::endl << std::flush;
    
    // set the output files (to 0 if not in list)
    // remember to call closeHandlerOutputFile for each call to openHandlerOutputFile
    QString bowOut = outputs.contains("bow")
        ? (outputs["bow"] == "stdout"? "stdout" : QString::fromUtf8((*fileItr).c_str())+outputs["bow"])
        : QString::fromUtf8((*fileItr).c_str())+".bin";
    std::ostream* bowofs  = openHandlerOutputFile(bowTextWriter, std::string(bowOut.toUtf8().constData()), dumpers, "bow");
    QString textOut = outputs.contains("text")
        ? (outputs["text"] == "stdout"? "stdout" : QString::fromUtf8((*fileItr).c_str())+outputs["text"])
        : "stdout";
    std::ostream* txtofs  = openHandlerOutputFile(simpleStreamHandler, std::string(textOut.toUtf8().constData()), dumpers, "text");
    QString fullxmlOut = outputs.contains("fullxml")
        ? (outputs["fullxml"] == "stdout"? "stdout" : QString::fromUtf8((*fileItr).c_str())+outputs["fullxml"])
        : "stdout";
    std::ostream* fullxmlofs  = openHandlerOutputFile(fullXmlSimpleStreamHandler, std::string(fullxmlOut.toUtf8().constData()), dumpers, "fullxml");
    
    // loading of the input file
    TimeUtils::updateCurrentTime();
    std::ifstream file(fileItr->c_str(), std::ifstream::binary);
    std::string text_s;
    readStream(file, text_s);
    if (text_s.size() == 0)
    {
      std::cerr << "file " << *fileItr << " has empty input ! " << std::endl;
      continue;
    }
    
    LimaString contentText;
    // The input text MUST be UTF-8 encoded !!!
    contentText = utf8stdstring2limastring(text_s);
    TimeUtils::logElapsedTime("ReadInputFile");

    TimeUtils::updateCurrentTime();
    std::string outputFile = *fileItr + ".";
    // analyze it
    metaData["FileName"]=*fileItr;
    client->analyze(contentText,metaData, pipeline, handlers, inactiveUnits);
    
    // Close and delete opened output files
    closeHandlerOutputFile(bowofs);
    closeHandlerOutputFile(txtofs);
    closeHandlerOutputFile(fullxmlofs);
  }
  std::cout << std::endl;
  delete client;
  // free handlers
  if (eventHandler != 0)
    delete eventHandler;
  if (bowTextWriter!= 0)
    delete bowTextWriter;
  if (bowTextHandler!= 0)
    delete bowTextHandler;
  if (simpleStreamHandler!= 0)
    delete simpleStreamHandler;
  if (fullXmlSimpleStreamHandler!= 0)
    delete fullXmlSimpleStreamHandler;
  delete Common::MediaticData::MediaticData::pchangeable();
  delete LinguisticProcessingClientFactory::pchangeable();
  TIMELOGINIT;
  LINFO << "Total: " << TimeUtils::diffTime(beginTime,TimeUtils::getCurrentTime()) << " ms";
  
  return SUCCESS_ID;
}

std::ostream* openHandlerOutputFile(AbstractTextualAnalysisHandler* handler, const std::string& fileName, const std::set<std::string>&dumpers, const std::string& dumperId)
{
  std::ostream* ofs = 0;
  if (dumpers.find(dumperId)!=dumpers.end())
  {
    if (fileName=="stdout")
    {
      ofs = &std::cout;
    }
    else
    {
    ofs = new std::ofstream(fileName.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    }
    if (ofs->good()) {
      handler->setOut(ofs);
    }
    else {
      std::cerr << "failed to open file " << fileName << std::endl;
      delete ofs; ofs = 0;
    }
  }
  return ofs;
}

void closeHandlerOutputFile(std::ostream* ofs)
{
  if (ofs != 0 && dynamic_cast<std::ofstream*>(ofs)!=0)
  {
    dynamic_cast<std::ofstream*>(ofs)->close();
    delete ofs;
    ofs = 0;
  }
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
