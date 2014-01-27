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
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"

#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
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
using namespace Lima::Common::BagOfWords;
using namespace Lima;
using namespace std;

void usage(int argc, char* argv[]);
void listunits();
int dowork(int argc,char* argv[]);
multimap<LimaString,string> extractNormalization(const LimaString& source,const BoWText& bowText,MediaId lang);
pair<int,int> getStartEnd(const BoWToken* tok);

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


int dowork(int argc,char* argv[])
{

  string resourcesPath=string(getenv("LIMA_RESOURCES"));
  string configDir=string(getenv("LIMA_CONF"));
  string lpConfigFile=string("lima-analysis.xml");
  string commonConfigFile=string("lima-common.xml");
  string pipeline=string("normalization");
  string clientId=string("lima-coreclient");

  bool printCategs=false;

  deque<string> langs;
  deque<string> files;

  if (argc>1)
  {
    for (int i = 1 ; i < argc; i++)
    {
      std::string arg(argv[i]);
      std::string::size_type pos = std::string::npos;
      if ( arg[0] == '-' )
      {
        if (arg == "--help")
          usage(argc, argv);
        else if (arg== "--printCategs")
          printCategs=true;
        else if (arg== "--availableUnits")
          listunits();
        else if (arg== "--catch") ;
        else if ( (pos = arg.find("--lp-config-file=")) != std::string::npos )
          lpConfigFile = arg.substr(pos+14);
        else if ( (pos = arg.find("--common-config-file=")) != std::string::npos )
          commonConfigFile = arg.substr(pos+20);
        else if ( (pos = arg.find("--config-dir=")) != std::string::npos )
          configDir = arg.substr(pos+13);
        else if ( (pos = arg.find("--resources-dir=")) != std::string::npos )
          resourcesPath = arg.substr(pos+16);
        else if ( (pos = arg.find("--language=")) != std::string::npos )
          langs.push_back(arg.substr(pos+11));
//         else if ( (pos = arg.find("--pipeline=")) != std::string::npos )
//           pipeline = arg.substr(pos+11);
        else if ( (pos = arg.find("--client=")) != std::string::npos )
          clientId=arg.substr(pos+9);
        else usage(argc, argv);
      }
      else
      {
        files.push_back(arg);
      }
    }
  }

  if (langs.size()<1)
  {
    std::cerr << "no language defined !" << std::endl;
    return -1;
  }

  AbstractLinguisticProcessingClient* client(0);

  try
  {

    // initialize common
    MediaticData::changeable().init(
      resourcesPath,
      configDir,
      commonConfigFile,
      langs);

    // initialize linguistic processing
    deque<string> pipelines;
    pipelines.push_back(pipeline);
    Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configDir + "/" + lpConfigFile);
    LinguisticProcessingClientFactory::changeable().configureClientFactory(
      clientId,
      lpconfig,
      langs,
      pipelines);

    client=dynamic_cast<AbstractLinguisticProcessingClient*>(LinguisticProcessingClientFactory::single().createClient(clientId));
    
    // Set the handlers
    std::map<std::string, AbstractAnalysisHandler*> handlers;
    BowTextHandler bowTextHandler;
    handlers.insert(std::make_pair("bowTextHandler", &bowTextHandler));
    
    map<string,string> metaData;
    metaData["Lang"]=langs[0];
    MediaId lang=MediaticData::single().getMediaId(langs[0]);

    for (deque<string>::iterator fileItr=files.begin();
         fileItr!=files.end();
         fileItr++)
    {
      // open the output file
      ostringstream os;
      os << *fileItr << ".norm";
      ofstream fout(os.str().c_str(), std::ofstream::binary);

      // loading of the input file
      TimeUtils::updateCurrentTime();
      std::ifstream file(fileItr->c_str(), std::ifstream::binary);
      char buf[256];
      file.getline(buf,256);
      std::string line(buf);
      while (!file.eof())
      {
        if (line.size()==0)
        {
          file.getline(buf,256);
          line=string(buf);
          continue;
        }
        //        cout << "normalize " << line << endl;
        LimaString contentText;
        // The input text MUST be UTF-8 encoded !!!
        contentText = utf8stdstring2limastring(line);

        // analyze it
        metaData["FileName"]=*fileItr;

        client->analyze(contentText,metaData,pipeline,handlers);

        // analyze resulting bowText to extract normalization
        multimap<LimaString,string> norms=extractNormalization(contentText,bowTextHandler.getBowText(),lang);
        if (norms.empty())
        {
          norms.insert(make_pair(contentText,"NONE_1"));
        }
        for (multimap<LimaString,string>::iterator it=norms.begin();it!=norms.end();it++)
        {
          fout << limastring2utf8stdstring(it->first);
          if (printCategs)
          {
            fout << "#" << it->second;
          }
          fout << ";";
        }
        fout << endl;
        // read next line
        file.getline(buf,256);
        line=string(buf);
      }
    }
  }
  catch (InvalidConfiguration& e)
  {
    throw e;
  }

  delete client;
  return SUCCESS_ID;
}


multimap<LimaString,string> extractNormalization(const LimaString& source,const BoWText& bowText,MediaId lang)
{
  const Common::PropertyCode::PropertyManager& macroManager = static_cast<const Common::MediaticData::LanguageData&>(MediaticData::single().mediaData(lang)).getPropertyCodeManager().getPropertyManager("MACRO");
  multimap<LimaString,string> result;
  // si un seul bowtoken on le prend
  //  if (bowText.size()==1)
  //  {
  //    cerr << "- found only one norm : " << bowText.front()->getLemma() << endl;
  //    result.push_back(bowText.front()->getLemma());
  //  }
  // sinon on prend tous les bowtoken qui vont du d�ut �la fin
  //  else
  //  {
  //    cerr << "extractNormalisation : " << source << endl;
  for (BoWText::const_iterator bowItr=bowText.begin();
       bowItr!=bowText.end();
       bowItr++)
  {
    pair<int,int> posLen=getStartEnd(*bowItr);
    //      cerr << "  - " << (*bowItr)->getLemma() << " at " << posLen.first << "," << posLen.second;
    if ((posLen.first==1) && (posLen.second==int(source.size()+1)))
    {
      result.insert(make_pair(
                      (*bowItr)->getLemma(),
                      macroManager.getPropertySymbolicValue((*bowItr)->getCategory())));
      //        cerr << " keep it !";
    }
    //      cerr << endl;
  }
  //   }
  return result;
}

pair<int,int> getStartEnd(const BoWToken* tok)
{
  pair<int,int> res;
  if (tok->getType()==BOW_TOKEN)
  {
    res.first=tok->getPosition();
    res.second=tok->getPosition()+tok->getLength();
  }
  else
  {
    const BoWComplexToken* complextok=dynamic_cast<const BoWComplexToken*>(tok);
    if (complextok==0)
    {
      cerr << "ERROR ! complextok==0 ! should not happen !" << endl;
      exit(0);
    }
    const std::deque< BoWComplexToken::Part >& parts=complextok->getParts();
    if (parts.size()==0)
    {
      cerr << "ERROR ! complex token should have at least one part ! " << endl;
      exit(0);
    }
    std::deque< BoWComplexToken::Part >::const_iterator partItr=parts.begin();
    res=getStartEnd(partItr->get<1>());
    partItr++;
    for (;partItr!=parts.end();partItr++)
    {
      pair<int,int> tmp=getStartEnd(partItr->get<1>());
      if (tmp.first<res.first) res.first=tmp.first;
      if (tmp.second>res.second) res.second=tmp.second;
    }
  }
  return res;
}

void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
  std::cout << "usage: " << argv[0] << " [OPTIONS] [file1 [file2 [...]]] " << std::endl;
  std::cout << "\t--printCategs\t\tprint categories with normalizations" << std::endl;
  std::cout << "\t--resources-dir=</path/to/the/resources> Optional. Default is $LIMA_RESOURCES" << std::endl;
  std::cout << "\t--config-dir=</path/to/the/configuration/directory> Optional. Default is $LIMA_CONF" << std::endl;
  std::cout << "\t--lp-config-file=<configuration/file/name>\tOptional. Default is lima-analysis.xml" << std::endl;
  std::cout << "\t--common-config-file=<configuration/file/name>\tOptional. Default is lima-common.xml" << std::endl;
  std::cout << "\t--client=<clientId>\tOptional. Default is 'lima-coreclient'" << std::endl;
  std::cout << "\t--language=<language trigram>\tOptional. Language of document to analyze." << std::endl;
//   std::cout << "\t--pipeline=<pipelineId>\tOptional. Default is 'main'" << std::endl;
  std::cout << "\t--availableUnits\tshow all available resources, processUnits and dumpers" << std::endl;
  std::cout << "\twhere files are files to analyze." << std::endl;
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
