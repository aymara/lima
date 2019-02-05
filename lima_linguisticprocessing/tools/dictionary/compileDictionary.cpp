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
/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST                                      *
 *                                                                         *
 ***************************************************************************/

#include <fstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <cstdlib>

#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/FsaAccess/FsaAccessSpare16.h"
#include "common/misc/AbstractAccessByString.h"
#include "common/tools/FileUtils.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
// #include "linguisticProcessing/core/Tokenizer/ParseChar.h"
// #include "linguisticProcessing/core/Tokenizer/ParseCharClass.h"

#include "KeysLogger.h"
#include "DictionaryHandler.h"

#include <QtXml/QXmlSimpleReader>
#include <QtCore/QCoreApplication>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::FsaAccess;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::FlatTokenizer;

void usage(int argc, char *argv[]);

//****************************************************************************
// GLOBAL variable -> the command line arguments
typedef struct
{
  std::string extractKeys;
  std::string charChart;
  std::string fsaKey;
  std::string propertyFile;
  std::string symbolicCodes;
  std::string output;
  std::string input;
  string configDir;          // this path overwrites environment variable
  bool reverseKeys;
  bool help;
} Param;
Q_GLOBAL_STATIC_WITH_ARGS(Param, param, ({"",
       "",
       "",
       "",
       "",
       "",
       "",
       "",
       false,
       false}));

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  for(uint64_t i(1); i<argc; i++)
  {
    std::string arg(argv[i]);
    size_t pos = -1;

    if (arg == "-h" || arg == "--help")
      param->help=true;

    if ( (pos = arg.find("--extractKeyList=")) != std::string::npos )
    {
      param->extractKeys = arg.substr(pos+17);
    }
    else if ( (pos = arg.find("--fsaKey=")) != std::string::npos )
    {
      param->fsaKey = arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--charChart=")) != std::string::npos )
    {
      param->charChart = arg.substr(pos+12);
    }
    else if ( (pos = arg.find("--propertyFile=")) != std::string::npos )
    {
      param->propertyFile = arg.substr(pos+15);
    }
    else if ( (pos = arg.find("--symbolicCodes=")) != std::string::npos )
    {
      param->symbolicCodes = arg.substr(pos+16);
    }
    else if ( (pos = arg.find("--output=")) != std::string::npos )
    {
      param->output = arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--reverse-keys")) != std::string::npos )
    {
      param->reverseKeys = true;
    }
    else if ( (pos = arg.find("--configDir=")) != std::string::npos )
    {
      param->configDir = arg.substr(pos+12);
    }
    else
    {
      param->input = arg;
    }
  }
}

//****************************************************************************
#include "common/tools/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

int run(int aargc, char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  LimaMainTaskRunner* task = new LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, &Lima::LimaMainTaskRunner::finished, [](int returnCode){ QCoreApplication::exit(returnCode); } );

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();
}

int run(int argc, char** argv)
{
  readCommandLineArguments(argc, argv);

  if (param->help)
  {
    usage(argc, argv);
    exit(0);
  }

  auto configDirs = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                      QStringList());
  auto configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  if (!param->configDir.empty())
  {
    configPath = QString::fromUtf8(param->configDir.c_str());
    configDirs = configPath.split(LIMA_PATH_SEPARATOR);
  }

  if (QsLogging::initQsLog(configPath) != 0)
  {
    LOGINIT("Common::Misc");
    LERROR << "Call to QsLogging::initQsLog(\"" << configPath << "\") failed.";
    return EXIT_FAILURE;
  }

  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();

  setlocale(LC_ALL,"fr_FR.UTF-8");

  // check that input file exists
  {
    ifstream fin(param->input.c_str(), std::ifstream::binary);
    if (!fin.good())
    {
      std::cerr << "can't open input file " << param->input << std::endl;
      exit(-1);
    }
    fin.close();
  }

  // parse charchart
  if (param->charChart == "")
  {
    LOGINIT("Common::Misc");
    LERROR << "please specify CharChart file with --charChart=<file> option";
    return EXIT_FAILURE;
  }
  LINFO << "parse charChart file : " << param.charChart;
  CharChart charChart;
  charChart.loadFromFile(param->charChart);

  if (param->extractKeys != "")
  {
    // just extract keys
    ofstream fout(param->extractKeys.c_str(), std::ofstream::binary);
    if (!fout.good())
    {
      std::cerr << "can't open file " << param->extractKeys << std::endl;
      exit(-1);
    }
    KeysLogger keysLogger(fout, &charChart, param->reverseKeys);

    std::cerr << "parse input file : " << param->input << std::endl;
    try
    {
      QXmlSimpleReader parser;
      //     parser->setValidationScheme(SAXParser::Val_Auto);
      //     parser->setDoNamespaces(false);
      //     parser->setDoSchema(false);
      //     parser->setValidationSchemaFullChecking(false);
      parser.setContentHandler(&keysLogger);
      parser.setErrorHandler(&keysLogger);
      QFile file(param->input.c_str());
      if (!file.open(QIODevice::ReadOnly))
      {
        std::cerr << "Error opening " << param->input << std::endl;
        return 1;
      }
      if (!parser.parse( QXmlInputSource(&file)))
      {
        std::cerr << "Error parsing " << param->input << " : "
                  << parser.errorHandler()->errorString().toUtf8().constData()
                  << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cerr << std::endl;
      }
    }
    catch (const XMLException& toCatch)
    {
      std::cerr << "An error occurred  Error: " << toCatch.what() << std::endl;
      throw;
    }
    fout.close();
  }
  else
  {
    // compile dictionaries

    std::cerr << "parse property code file : " << param->propertyFile << std::endl;
    PropertyCodeManager propcodemanager;
    propcodemanager.readFromXmlFile(param->propertyFile);

    std::cerr << "parse symbolicCode file : " << param->symbolicCodes << std::endl;
    map<string,LinguisticCode> conversionMap;
    propcodemanager.convertSymbolicCodes(param->symbolicCodes,conversionMap);
    std::cerr << conversionMap.size() << " code read from symbolicCode file" << std::endl;

    AbstractAccessByString* access = nullptr;
    if (param->fsaKey!="")
    {
      std::cerr << "load fsa access method : " << param->fsaKey << std::endl;
      auto fsaAccess = new FsaAccessSpare16();
      fsaAccess->read(param->fsaKey);
      access = fsaAccess;
    }
    else
    {
      std::cerr << "ERROR : no access Keys defined !" << std::endl;
      return EXIT_FAILURE;
    }
    std::cerr << access->getSize() << " keys loaded" << std::endl;

    std::cerr << "parse input file : " << param->input << std::endl;
    DictionaryCompiler handler(&charChart,
                               access,
                               conversionMap,
                               param->reverseKeys);

    QXmlSimpleReader parser;
    try
    {
      parser.setContentHandler(&handler);
      parser.setErrorHandler(&handler);
      QFile file(param->input.c_str());
      if (!file.open(QIODevice::ReadOnly))
      {
        std::cerr << "Error opening " << param->input << std::endl;
        return 1;
      }
      if (!parser.parse( QXmlInputSource(&file)))
      {
        std::cerr << "Error parsing " << param->input << " : "
                  << parser.errorHandler()->errorString().toUtf8().constData()
                  << std::endl;
        return EXIT_FAILURE;
      }
    }
    catch (const XMLException& toCatch)
    {
      std::cerr << "An error occurred  Error: " << toCatch.what() << std::endl;
      throw;
    }

    std::cerr << "write data to output file : " << param->output << std::endl;
    ofstream fout(param->output.c_str(),ios::out | ios::binary);
    if (!fout.good())
    {
      std::cerr << "can't open file " << param->output << std::endl;
      return EXIT_FAILURE;
    }
    handler.writeBinaryDictionary(fout);
    fout.close();
    delete access;
  }
  return EXIT_SUCCESS;
}

void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
  LIMA_UNUSED(argv);
  std::cerr << "USAGE : compileDictionary [OPTIONS] file" << std::endl;
  std::cerr << "where [OPTIONS] are : " << std::endl;
  std::cerr << "  --extractKeyList=<outputfile> : only extract keys list to file, no compilation" << std::endl;
  std::cerr << "  --charChart=<charChatFile> : specify charchart file" << std::endl;
  std::cerr << "  --fsaKey=<fsaFile> : provide fsa access keys to compile" << std::endl;
  std::cerr << "  --propertyFile=<propFile> : specify property coding system (xml file)" << std::endl;
  std::cerr << "  --symbolicCodes=<codeFile> : specify symbolic codes file (xml)" << std::endl;
  std::cerr << "  --output=<outputFile> : specify output file" << std::endl;
  std::cerr << "  --reverse-keys : reverse entries keys" << std::endl;
}
