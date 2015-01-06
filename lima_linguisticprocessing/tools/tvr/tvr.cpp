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
 *   Copyright (C) 2006 by Olivier Mesnard                                  *
 *   olivier.mesnard@cea.fr                                                *
 *                                                                         *
 ***************************************************************************/

/**
 * This program is designed to test the AMOSE document readers whose role is to dispatch
 * analysis tasks to the various media analysis clients and to collect and transmit metadata
 * such as positions for example.
 *
 * Based on the tgv framework, the tests are described in XML files. For each data file listed,
 * the program calls the analysis and then reads the generated XML to check that it conforms to
 * what is specified in the test.
 */

#include "linguisticProcessing/common/tgv/TestCasesHandler.h"
#include "tools/tvr/ReaderTestCase.h"

#include <iostream>

#include "common/LimaCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"

#include "linguisticProcessing/client/AbstractLinguisticProcessingClient.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"

#include <QtXml/QXmlSimpleReader>
#include <QtCore/QCoreApplication>

using namespace std;
//using namespace xercesc_2_5;
using namespace Lima::Common::TGV;
using namespace Lima::ReaderValidation;
using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;



void usage(int argc, char* argv[]);

#include "common/misc/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

int run(int aargc,char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  Lima::LimaMainTaskRunner* task = new Lima::LimaMainTaskRunner(argc, argv, run, &a);

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
  string lpConfigFile=string("lima-lp-tvr.xml");
  string commonConfigFile=string("lima-common.xml");
  string clientId=string("lp-structuredXmlreaderclient");
  string workingDir=string(".");

  if (resourcesPath.empty())
  {
    resourcesPath = "/usr/share/apps/lima/resources/";
  }
  if (configDir.empty())
  {
    configDir = "/usr/share/config/lima/";
  }

  deque<string> files;
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
        if (arg == "--help")
          usage(argc, argv);
        else if ( (pos = arg.find("--lp-config-file=")) != std::string::npos )
          lpConfigFile = arg.substr(pos+17);
        else if ( (pos = arg.find("--common-config-file=")) != std::string::npos )
          commonConfigFile = arg.substr(pos+21);
        else if ( (pos = arg.find("--config-dir=")) != std::string::npos )
          configDir = arg.substr(pos+13);
        else if ( (pos = arg.find("--resources-dir=")) != std::string::npos )
          resourcesPath = arg.substr(pos+16);
        else if ( (pos = arg.find("--client=")) != std::string::npos )
          clientId=arg.substr(pos+9);
        else if ( (pos = arg.find("--working-dir=")) != std::string::npos )
          workingDir=arg.substr(pos+14);
        else if ( (pos = arg.find("--language=")) != std::string::npos )
          langs.push_back(arg.substr(pos+11));
        else usage(argc, argv);
      }
      else
      {
        files.push_back(arg);
      }
    }
  }

  setlocale(LC_ALL,"fr_FR.UTF-8");

  AbstractLinguisticProcessingClient* client(0);

  // initialize common
  MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);

  // initialize linguistic processing
  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configDir + "/" + lpConfigFile);
  LinguisticProcessingClientFactory::changeable().configureClientFactory(
    clientId,
    lpconfig,
    MediaticData::single().getMedias());

  client=dynamic_cast<AbstractLinguisticProcessingClient*>(LinguisticProcessingClientFactory::single().createClient(clientId));
  
  
  ReaderTestCaseProcessor
    readerTestCaseProcessor(workingDir, client);
    
  QXmlSimpleReader parser;
  TestCasesHandler tch(readerTestCaseProcessor);

  parser.setContentHandler(&tch);
  parser.setErrorHandler(&tch);

  for (deque<string>::const_iterator it=files.begin();
       it!=files.end();
       it++)
  {
    cout << "process tests in " << *it << endl;
    try
    {
      QFile file(it->c_str());
      if (!file.open(QIODevice::ReadOnly))
      {
        std::cerr << "Error opening " << *it << std::endl;
        return 1;
      }
      if (!parser.parse( QXmlInputSource(&file)))
      {
        std::cerr << "Error parsing " << *it << " : " << parser.errorHandler()->errorString().toUtf8().constData() << std::endl;
        return 1;
      }
    }
    catch (Lima::LimaException& e)
    {
      cerr << "caught LimaException : " << endl << e.what() << endl;
    }
    catch (logic_error& e)
    {
      cerr << "caught logic_error : " << endl << e.what() << endl;
    }

    TestCasesHandler::TestReport resTotal;
    cout << endl;
    cout << "=========================================================" << endl;
    cout << endl;
    cout << "  TestReport :   " << *it << " " << endl;
    cout << endl;
    cout << "\ttype           \tsuccess\tcond.\tfailed\ttotal" << endl;
    cout << "---------------------------------------------------------" << endl;
    for (map<string,TestCasesHandler::TestReport>::const_iterator resItr=tch.m_reportByType.begin();
         resItr!=tch.m_reportByType.end();
         resItr++)
    {
      string label(resItr->first);
      label.resize(15,' ');
      cout << "\t" << label << "\t" << resItr->second.success << "\t" << resItr->second.conditional << "\t" << resItr->second.failed << "\t" << resItr->second.nbtests << endl;
      resTotal.success+=resItr->second.success;
      resTotal.conditional+=resItr->second.conditional;
      resTotal.failed+=resItr->second.failed;
      resTotal.nbtests+=resItr->second.nbtests;
    }
    cout << "---------------------------------------------------------" << endl;
    cout << "\ttotal          \t" << resTotal.success << "\t" << resTotal.conditional << "\t" << resTotal.failed << "\t" << resTotal.nbtests << endl;
    cout << "=========================================================" << endl;
    cout << endl;
    tch.m_reportByType.clear();
  }
  return 0;
}


void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
  std::cout << "usage: " << argv[0] << " [OPTIONS] [file1 [file2 [...]]] " << std::endl;
  std::cout << "\t--working-dir=</path/to/the/working/dir> Optional. Default is ./" << std::endl;
  std::cout << "\t--resources-dir=</path/to/the/resources> Optional. Default is $LIMA_RESOURCES" << std::endl;
  std::cout << "\t--config-dir=</path/to/the/configuration/directory> Optional. Default is $LIMA_CONF" << std::endl;
  std::cout << "\t--lp-config-file=<configuration/file/name>\tOptional. Default is lima-lp-tva.xml" << std::endl;
  std::cout << "\t--common-config-file=<configuration/file/name>\tOptional. Default is lima-common.xml" << std::endl;
  std::cout << "\t--client=<clientId>\tOptional. Default is 'lima-coreclient'" << std::endl;
  std::cout << "\t--pipeline=<pipelineId>\tOptional. Default is 'main'" << std::endl;
  std::cout << "\t--language=<lang>\tOptional. Default initialize all available languages in common config file" << std::endl;
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
