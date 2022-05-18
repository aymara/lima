// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
#include <vector>

#include "common/LimaCommon.h"
// #include "common/linguisticData/linguisticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"

#include "linguisticProcessing/common/tgv/TestCase.h"
#include "linguisticProcessing/common/tgv/TestCasesHandler.h"

#include "linguisticProcessing/common/tgv/TestCase.h"
#include "linguisticProcessing/common/tgv/TestCaseError.hpp"

#include <QtXml/QXmlSimpleReader>
#include <QtCore/QCoreApplication>

using namespace Lima::Common::TGV;


void usage(int argc, char* argv[]);

// options
typedef struct ParamStruct {
  std::string resourcesPath;
  std::string configDir;
  std::string commonConfigFile;
  std::string clientId;
  std::string workingDir;
  std::vector<std::string> files;
} Param;

class TvgTestCaseProcessor : public TestCaseProcessor
{
public:

  TvgTestCaseProcessor( const std::string workingDirectory, std::ostream &os )
    : TestCaseProcessor( workingDirectory ), m_os(os)  {}

  virtual TestCaseError processTestCase(const TestCase& testCase) override;

private:
  std::ostream & m_os;
};

TestCaseError TvgTestCaseProcessor::processTestCase(const TestCase& testCase) {
  m_os << testCase;
  return TestCaseError();
}

#include "common/tools/LimaMainTaskRunner.h"
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

  Param param = {
    std::string(getenv("LIMA_RESOURCES")==0?"/usr/share/apps/lima/resources":getenv("LIMA_RESOURCES")),
    std::string(getenv("LIMA_CONF")==0?"/usr/share/config/lima":getenv("LIMA_CONF")),
    std::string("lima-common.xml"),
    std::string("XXX-coreclient"),
    std::string("."),
    std::vector<std::string>()
  };

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
        else if ( (pos = arg.find("--common-config-file=")) != std::string::npos )
          param.commonConfigFile = arg.substr(pos+20);
        else if ( (pos = arg.find("--config-dir=")) != std::string::npos )
          param.configDir = arg.substr(pos+13);
        else if ( (pos = arg.find("--resources-dir=")) != std::string::npos )
          param.resourcesPath = arg.substr(pos+16);
        else if ( (pos = arg.find("--client=")) != std::string::npos )
          param.clientId=arg.substr(pos+9);
        else if ( (pos = arg.find("--working-dir=")) != std::string::npos )
          param.workingDir=arg.substr(pos+14);
        else usage(argc, argv);
      }
      else
      {
        param.files.push_back(std::string(arg));
      }
    }
  }

  setlocale(LC_ALL,"fr_FR.UTF-8");


  QXmlSimpleReader parser;

  // create TvgTestCaseProcessor
  // TvgTestCaseProcessor tvgTestCaseProcessor(param.workingDir, cerr);
  TvgTestCaseProcessor* tvgTestCaseProcessor(0);
  tvgTestCaseProcessor = new TvgTestCaseProcessor(param.workingDir, std::cerr);

  TestCasesHandler tch(*tvgTestCaseProcessor);

  parser.setContentHandler(&tch);
  parser.setErrorHandler(&tch);

  try
  {
    for( std::vector<std::string>::const_iterator fit = param.files.begin() ;
      fit != param.files.end() ; fit++ ) {
      std::string sfile(param.workingDir);
      sfile.append("/").append(*fit);
      std::cout << "parse " << sfile << std::endl;

    // cerr << "<?xml version='1.0' encoding='UTF-8'?>\n";
// cerr << "<testcases>\n";
    QFile file(sfile.c_str());
    if (!file.open(QIODevice::ReadOnly))
    {
      std::cerr << "Error opening " << sfile << std::endl;
      return 1;
    }
    if (!parser.parse( QXmlInputSource(&file)))
    {
      std::cerr << "Error parsing " << sfile << " : " << parser.errorHandler()->errorString().toUtf8().constData() << std::endl;
      return 1;
    }
    // cerr << "</testcases>\n";

  TestCasesHandler::TestReport resTotal;
  std::cout << std::endl;
  std::cout << "=========================================================" << std::endl;
  std::cout << std::endl;
  std::cout << "  TestReport :   " << sfile.c_str() << " " << std::endl;
  std::cout << std::endl;
  std::cout << "\ttype           \tsuccess\tcond.\tfailed\ttotal" << std::endl;
  std::cout << "---------------------------------------------------------" << std::endl;
  for (std::map<std::string, TestCasesHandler::TestReport>::const_iterator resItr=tch.m_reportByType.begin();
       resItr!=tch.m_reportByType.end();
       resItr++)
  {
    std::string label(resItr->first);
    label.resize(15,' ');
    std::cout << "\t" << label << "\t" << resItr->second.success
         << "\t" << resItr->second.conditional << "\t"
         << resItr->second.failed << "\t" << resItr->second.nbtests << std::endl;
    resTotal.success+=resItr->second.success;
    resTotal.conditional+=resItr->second.conditional;
    resTotal.failed+=resItr->second.failed;
    resTotal.nbtests+=resItr->second.nbtests;
  }
  std::cout << "---------------------------------------------------------" << std::endl;
  std::cout << "\ttotal          \t" << resTotal.success
       << "\t" << resTotal.conditional << "\t" << resTotal.failed
       << "\t" << resTotal.nbtests << std::endl;
  std::cout << "=========================================================" << std::endl;
  std::cout <<  std::endl;
  tch.m_reportByType.clear();
    }
  }
  catch (Lima::LimaException& e)
  {
    std::cerr << "caught LimaException : " << std::endl << e.what() << std::endl;
  }
  catch (std::logic_error& e)
  {
    std::cerr << "caught logic_error : " << std::endl << e.what() << std::endl;
  }

  if( tvgTestCaseProcessor != 0 )
    delete tvgTestCaseProcessor;
  // cerr << "main: after MLPlatformUtils::Terminate. before end..." << endl;
  return 0;
}


void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
  std::cout << "usage: " << argv[0] << " [OPTIONS] [file1 [file2 [...]]] "
            << std::endl;
  std::cout << "\t--working-dir=</path/to/the/working/dir> Optional. Default is ./"
            << std::endl;
  std::cout << "\t--resources-dir=</path/to/the/resources> Optional. Default is $LIMA_RESOURCES"
            << std::endl;
  std::cout << "\t--config-dir=</path/to/the/configuration/directory> Optional. Default is $LIMA_CONF"
            << std::endl;
  std::cout << "\t--common-config-file=<configuration/file/name>\tOptional. Default is lima-common.xml"
            << std::endl;
  std::cout << "\t--client=<clientId>\tOptional. Default is 'indexerreader-coreclient'"
            << std::endl;
  std::cout << "\twhere files are files to analyze." << std::endl;
  std::cout << std::endl;
  exit(0);
}
