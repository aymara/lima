// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2019 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/Data/strwstrtools.h"
#include "common/LimaCommon.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"
#include "common/tools/LimaMainTaskRunner.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"

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
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

 #include <QtCore/QList>
 #include <QtCore/QThread>
 #include <QtConcurrent/qtconcurrentmap.h>

#include <boost/shared_ptr.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>


using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::Common::BagOfWords;
using namespace Lima;

void usage(int argc, char* argv[]);
void listunits();
int dowork(int argc,char* argv[]);
std::multimap<LimaString, std::string> extractNormalization(const LimaString& source,const BoWText& bowText,MediaId lang);
std::pair<int,int> getStartEnd(const BoWToken* tok);

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
//       std::cerr << "Doing work in try block." << std::endl;
      return dowork(argc, argv);
    }
    catch (const std::exception& e)
    {
      std::cerr << "Catched an exception: " << e.what() << std::endl;
      return 1;
    }
    catch (...)
    {
      std::cerr << "Catched an unknown exception " << std::endl;
      return 1;
    }
  }
  else
    return dowork(argc,argv);
}


struct Parameters {
  bool printCategs;
  QStringList configDirs;
  QString configPath;
  QStringList resourcesDirs;
  QString resourcesPath;

  std::string resourcesPathParam;
  std::string configPathParam;
  std::string lpConfigFile;
  std::string commonConfigFile;
  std::string pipeline;
  std::string clientId;
  std::deque<std::string> langs;
  std::deque<std::string> files;

  int chunk_size;

  Parameters(){

    printCategs=false;
    lpConfigFile = "lima-analysis.xml";
    commonConfigFile = "lima-common.xml";
    pipeline = "normalization";
    clientId = "lima-coreclient";
    chunk_size = 500;
  }
};
Parameters params;

int read_cmd_line(int argc, char** argv) {

  params.configDirs = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                             QStringList());
  params.configPath = params.configDirs.join(LIMA_PATH_SEPARATOR);

  params.resourcesDirs = buildResourcesDirectoriesList(QStringList({"lima"}),
                                                            QStringList());
  params.resourcesPath = params.resourcesDirs.join(LIMA_PATH_SEPARATOR);


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
          params.printCategs=true;
        else if (arg== "--availableUnits")
          listunits();
        else if (arg== "--catch") ;
        else if ( (pos = arg.find("--lp-config-file=")) != std::string::npos )
          params.lpConfigFile = arg.substr(pos+17);
        else if ( (pos = arg.find("--common-config-file=")) != std::string::npos )
          params.commonConfigFile = arg.substr(pos+21);
        else if ( (pos = arg.find("--config-dir=")) != std::string::npos )
          params.configPathParam = arg.substr(pos+13);
        else if ( (pos = arg.find("--resources-dir=")) != std::string::npos )
          params.resourcesPathParam = arg.substr(pos+16);
        else if ( (pos = arg.find("--language=")) != std::string::npos )
          params.langs.push_back(arg.substr(pos+11));
//         else if ( (pos = arg.find("--pipeline=")) != std::string::npos )
//           pipeline = arg.substr(pos+11);
        else if ( (pos = arg.find("--client=")) != std::string::npos )
          params.clientId=arg.substr(pos+9);
        else if ( (pos = arg.find("--chunk=")) != std::string::npos )
          params.chunk_size = atoi( arg.substr(pos+8).c_str() );
        else usage(argc, argv);
      }
      else
      {
          params.files.push_back(arg);
      }
    }
  }

  if (params.langs.size()<1)
  {
    std::cerr << "no language defined !" << std::endl;
    return -1;
  }

  if (!params.configPathParam.empty())
  {
      params.configPath = QString::fromUtf8(params.configPathParam.c_str());
      params.configDirs = params.configPath.split(LIMA_PATH_SEPARATOR);
  }
  if (!params.resourcesPathParam.empty())
  {
      params.resourcesPath = QString::fromUtf8(params.resourcesPathParam.c_str());
      params.resourcesDirs = params.resourcesPath.split(LIMA_PATH_SEPARATOR);
  }
  return 0;
}


struct MyFunctor {

  MyFunctor(
          std::shared_ptr <AbstractLinguisticProcessingClient > _client,
                         const std::map<std::string, std::string>& _metaData,
                         MediaId _lang)
  {
    this->client = _client;
    this->metaData = _metaData;
    this->lang = _lang;
  }

  std::shared_ptr <AbstractLinguisticProcessingClient > client;
  std::map<std::string, std::string> metaData;
  MediaId lang;

  typedef QString result_type;


  QString operator()(const QString& contentText)
  {
      // Set the handlers
      std::map<std::string, AbstractAnalysisHandler*> handlers;
      BowTextHandler bowTextHandler;
      handlers.insert(std::make_pair("bowTextHandler", &bowTextHandler));

      //        cout << "normalize " << contentText << endl;

      client->analyze(contentText,metaData,params.pipeline,handlers);

      // analyze resulting bowText to extract normalization
      std::multimap<LimaString, std::string> norms=extractNormalization(contentText,
                                                             bowTextHandler.getBowText(),
                                                             lang);
      std::ostringstream oss;
      if (norms.empty())
      {
        norms.insert(std::make_pair(contentText,"NONE_1"));
      }
      for (auto it = norms.cbegin(); it != norms.cend(); it++)
      {
          oss << limastring2utf8stdstring(it->first);
        if (params.printCategs)
        {
            oss << "#" << it->second;
        }
        oss << ";";
      }
      oss << std::endl;

      return QString::fromStdString(oss.str());
  }
};

int dowork(int argc,char* argv[])
{
  int st = read_cmd_line(argc, argv);
  if (st != SUCCESS_ID) return st;

  QsLogging::initQsLog(params.configPath);
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  if (!Lima::AmosePluginsManager::changeable().loadPlugins(params.configPath))
    throw InvalidConfiguration("loadPlugins method failed.");

#ifndef DEBUG_LP
  try
  {
#endif
    // initialize common
    MediaticData::changeable().init(
        params.resourcesPath.toUtf8().constData(),
        params.configPath.toUtf8().constData(),
        params.commonConfigFile,
        params.langs);

    // initialize linguistic processing
    std::deque<std::string> pipelines;
    pipelines.push_back(params.pipeline);

    QString lpConfigFileFound = Common::Misc::findFileInPaths(params.configPath,
                                                              params.lpConfigFile.c_str(),
                                                              LIMA_PATH_SEPARATOR);

    Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(
        lpConfigFileFound.toUtf8().constData());
    LinguisticProcessingClientFactory::changeable().configureClientFactory(
        params.clientId,
        lpconfig,
        params.langs,
        pipelines);

    std::shared_ptr <AbstractLinguisticProcessingClient > client=
        std::dynamic_pointer_cast<AbstractLinguisticProcessingClient>(
          LinguisticProcessingClientFactory::single().createClient(params.clientId));

//    // Set the handlers
//    std::map<std::string, AbstractAnalysisHandler*> handlers;
//    BowTextHandler bowTextHandler;
//    handlers.insert(std::make_pair("bowTextHandler", &bowTextHandler));

    std::map<std::string, std::string> metaData;
    metaData["Lang"]=params.langs[0];
    MediaId lang=MediaticData::single().getMediaId(params.langs[0]);

    for (auto fileItr = params.files.cbegin(); fileItr != params.files.cend(); fileItr++)
    {
      // open the output file
      std::ostringstream os;
      os << *fileItr << ".norm";
      std::ofstream fout(os.str().c_str());

      // prepare chunk of lines for parallel processing
      QStringList contentList;

      // loading of the input file
      TimeUtils::updateCurrentTime();
      std::ifstream file(fileItr->c_str());
      char buf[256];
      file.getline(buf,256);
      std::string line(buf);
      while (file.good())
      {
        contentList.clear();
        while (file.good() && contentList.size() < params.chunk_size )
        {
          if (line.size()==0)
          {
            file.getline(buf,256);
            line = std::string(buf);
            continue;
          }

          LimaString contentText;
          // The input text MUST be UTF-8 encoded !!!
          contentText = QString::fromStdString(line).trimmed();
          contentList.push_back(contentText);

          // read next line
          file.getline(buf,256);
          line=std::string(buf);
        }

        // analyze it
        metaData["FileName"] = *fileItr;
        QFuture<QString> future = QtConcurrent::mapped(contentList, MyFunctor(client, metaData, lang));

        //future.waitForFinished();
        QStringList outputList = future.results();
        for(int i=0; i<outputList.size(); i++)
        {
          fout << outputList[i].toStdString();
        }

      }
    }
#ifndef DEBUG_LP
  }
  catch (InvalidConfiguration& e)
  {
    throw e;
  }
#endif
  return SUCCESS_ID;
}

std::multimap<LimaString, std::string> extractNormalization(const LimaString& source,
                                                 const BoWText& bowText,
                                                 MediaId lang)
{
#ifdef DEBUG_LP
  LOGINIT("Refo::Compiler");
  LDEBUG << "extractNormalization" << source;
#endif
  const Common::PropertyCode::PropertyManager& macroManager =
      static_cast<const Common::MediaticData::LanguageData&>(
        MediaticData::single().mediaData(lang))
                    .getPropertyCodeManager().getPropertyManager("MACRO");
  std::multimap<LimaString, std::string> result;
  // si un seul bowtoken on le prend
  //  if (bowText.size()==1)
  //  {
  //    cerr << "- found only one norm : " << bowText.front()->getLemma() << endl;
  //    result.push_back(bowText.front()->getLemma());
  //  }
  // sinon on prend tous les bowtoken qui vont du debut a la fin
  //  else
  //  {
//   std::cerr << "extractNormalisation : '" << source.toUtf8().constData() << "' "
//             << source.size() << std::endl;
  for (auto bowItr = bowText.cbegin(); bowItr != bowText.cend(); bowItr++)
  {
    if ((*bowItr)->getType() != BoWType::BOW_PREDICATE)
    {
      std::pair<int,int> posLen = getStartEnd(static_cast<const BoWToken*>(&**bowItr));
#ifdef DEBUG_LP
      LDEBUG << "extractNormalization"
              << boost::dynamic_pointer_cast<BoWToken>(*bowItr)->getLemma()
              << macroManager.getPropertySymbolicValue(
                    boost::dynamic_pointer_cast<BoWToken>(*bowItr)->getCategory())
              << "at" << posLen.first << "," << posLen.second;
#endif
      if ((posLen.first==0) && (posLen.second==source.size()))
      {
        result.insert(std::make_pair(
                        boost::dynamic_pointer_cast<BoWToken>(*bowItr)->getLemma(),
                        macroManager.getPropertySymbolicValue(
                            boost::dynamic_pointer_cast<BoWToken>(*bowItr)->getCategory())));
#ifdef DEBUG_LP
        LDEBUG << "extractNormalization keep it !";
#endif
      }
      else
      {
#ifdef DEBUG_LP
        LDEBUG << "extractNormalization IGNORE it !";
#endif
      }
    }
  }
  //   }
  return result;
}

std::pair<int,int> getStartEnd(const BoWToken* tok)
{
  std::pair<int,int> res;
  if (tok->getType()==BoWType::BOW_TOKEN)
  {
    res.first=tok->getPosition();
    res.second=tok->getPosition()+tok->getLength();
  }
  else
  {
    const BoWComplexToken* complextok=dynamic_cast<const BoWComplexToken*>(tok);
    if (complextok==0)
    {
      std::cerr << "ERROR ! complextok==0 ! should not happen !" << std::endl;
      exit(0);
    }
    const std::deque< BoWComplexToken::Part >& parts=complextok->getParts();
    if (parts.size()==0)
    {
      std::cerr << "ERROR ! complex token should have at least one part ! " << std::endl;
      exit(0);
    }
    auto partItr=parts.cbegin();
    res=getStartEnd(&*partItr->get<1>());
    partItr++;
    for (; partItr != parts.end(); partItr++)
    {
      std::pair<int,int> tmp = getStartEnd(&*partItr->get<1>());
      if (tmp.first<res.first) res.first = tmp.first;
      if (tmp.second>res.second) res.second = tmp.second;
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
  std::cout << std::endl;
  std::cout << "Available client factories are : " << std::endl;
  {
    std::deque<std::string> ids=LinguisticProcessingClientFactory::single().getRegisteredFactories();
    for (auto it = ids.cbegin(); it != ids.end(); it++)
    {
      std::cout << "- " << *it << std::endl;
    }
    std::cout << std::endl;
  }
  exit(0);
}


void listunits()
{
  {
    std::cout << "Available resources factories : " << std::endl;
    std::deque<std::string> ids = AbstractResource::Factory::getRegisteredFactories();
    for (auto it = ids.cbegin(); it != ids.cend(); it++)
    {
      std::cout << "- " << *it << std::endl;
    }
    std::cout << std::endl;
  }
  {
    std::cout << "Available process units factories : " << std::endl;
    std::deque<std::string> ids = MediaProcessUnit::Factory::getRegisteredFactories();
    for (auto it = ids.cbegin(); it != ids.cend(); it++)
    {
      std::cout << "- " << *it << std::endl;
    }
    std::cout << std::endl;
  }
  std::cout << "Available client factories are : " << std::endl;
  {
    std::deque<std::string> ids = LinguisticProcessingClientFactory::single().getRegisteredFactories();
    for (auto it = ids.cbegin(); it != ids.cend(); it++)
    {
      std::cout << "- " << *it << std::endl;
    }
    std::cout << std::endl;
  }
  exit(0);
}
