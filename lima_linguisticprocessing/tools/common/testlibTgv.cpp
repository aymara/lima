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

using namespace std;

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

  virtual TestCaseError processTestCase(const TestCase& testCase);

private:
  std::ostream & m_os;
};
  
TestCaseError TvgTestCaseProcessor::processTestCase(const TestCase& testCase) {
  m_os << testCase;
  return TestCaseError();
}

int main(int argc,char* argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();

  Param param = {
    std::string(getenv("LIMA_RESOURCES")),
    std::string(getenv("LIMA_CONF")),
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
  tvgTestCaseProcessor = new TvgTestCaseProcessor(param.workingDir, cerr);

  TestCasesHandler tch(*tvgTestCaseProcessor);

  parser.setContentHandler(&tch);
  parser.setErrorHandler(&tch);

  try
  {
    for( std::vector<std::string>::const_iterator fit = param.files.begin() ;
      fit != param.files.end() ; fit++ ) {
      string sfile(param.workingDir);
      sfile.append("/").append(*fit); 
      cout << "parse " << sfile << endl;

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
  cout << endl;
  cout << "=========================================================" << endl;
  cout << endl;
  cout << "  TestReport :   " << sfile.c_str() << " " << endl;
  cout << endl;
  cout << "\ttype           \tsuccess\tcond.\tfailed\ttotal" << endl;
  cout << "---------------------------------------------------------" << endl;
  for (map<string,TestCasesHandler::TestReport>::const_iterator resItr=tch.m_reportByType.begin();
       resItr!=tch.m_reportByType.end();
       resItr++)
  {
    string label(resItr->first);
    label.resize(15,' ');
    cout << "\t" << label << "\t" << resItr->second.success
         << "\t" << resItr->second.conditional << "\t"
         << resItr->second.failed << "\t" << resItr->second.nbtests << endl;
    resTotal.success+=resItr->second.success;
    resTotal.conditional+=resItr->second.conditional;
    resTotal.failed+=resItr->second.failed;
    resTotal.nbtests+=resItr->second.nbtests;
  }
  cout << "---------------------------------------------------------" << endl;
  cout << "\ttotal          \t" << resTotal.success
       << "\t" << resTotal.conditional << "\t" << resTotal.failed
       << "\t" << resTotal.nbtests << endl;
  cout << "=========================================================" << endl;
  cout << endl;
  tch.m_reportByType.clear();
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

  if( tvgTestCaseProcessor != 0 )
    delete tvgTestCaseProcessor;
  // cerr << "main: after MLPlatformUtils::Terminate. before end..." << endl;
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
  std::cout << endl;
  exit(0);
}
