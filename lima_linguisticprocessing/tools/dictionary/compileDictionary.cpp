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

#include <fstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <cstdlib>

#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "common/FsaAccess/FsaAccessSpare16.h"
#include "common/misc/AbstractAccessByString.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
// #include "linguisticProcessing/core/Tokenizer/ParseChar.h"
// #include "linguisticProcessing/core/Tokenizer/ParseCharClass.h"

#include "KeysLogger.h"
#include "DictionaryHandler.h"

#include <QtXml/QXmlSimpleReader>
#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::FsaAccess;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::FlatTokenizer;

void usage()
{
  std::cerr << "USAGE : compileDictionary [OPTIONS] file" << std::endl;
  std::cerr << "where [OPTIONS] are : " << std::endl;
  std::cerr << "  --extractKeyList=<outputfile> : only extract keys list to file, no compilation" << endl;
  std::cerr << "  --charChart=<charChatFile> : specify charchart file" << endl;
  std::cerr << "  --fsaKey=<fsaFile> : provide fsa access keys to compile" << endl;
  std::cerr << "  --propertyFile=<propFile> : specify property coding system (xml file)" << endl;
  std::cerr << "  --symbolicCodes=<codeFile> : specify symbolic codes file (xml)" << endl;
  std::cerr << "  --output=<outputFile> : specify output file" << endl;
  std::cerr << "  --reverse-keys : reverse entries keys" << endl;
}

// options
typedef struct ParamStruct
{
  std::string extractKeys;
  std::string charChart;
  std::string fsaKey;
  std::string propertyFile;
  std::string symbolicCodes;
  std::string output;
  std::string input;
  bool reverseKeys;
}
Param;


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
  
  setlocale(LC_ALL,"fr_FR.UTF-8");

  Param param = {
                  std::string(""),
                  std::string(""),
                  std::string(""),
                  std::string(""),
                  std::string(""),
                  std::string(""),
                  std::string(""),
                  false};


  for (int i = 1 ; i < argc; i++)
  {
    std::string arg(argv[i]);
    int pos = -1;
    if (arg == "--help")
    {
      usage();
      return 0;
    }
    if ( (pos = arg.find("--extractKeyList=")) != -1 )
    {
      param.extractKeys = arg.substr(pos+17);
    }
    else if ( (pos = arg.find("--fsaKey=")) != -1 )
    {
      param.fsaKey = arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--charChart=")) != -1 )
    {
      param.charChart = arg.substr(pos+12);
    }
    else if ( (pos = arg.find("--propertyFile=")) != -1 )
    {
      param.propertyFile = arg.substr(pos+15);
    }
    else if ( (pos = arg.find("--symbolicCodes=")) != -1 )
    {
      param.symbolicCodes = arg.substr(pos+16);
    }
    else if ( (pos = arg.find("--output=")) != -1 )
    {
      param.output = arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--reverse-keys")) != -1 )
    {
      param.reverseKeys = true;
    }
    else
    {
      param.input = arg;
    }
  }

  // check that input file exists
  {
    ifstream fin(param.input.c_str(), std::ifstream::binary);
    if (!fin.good())
    {
      cerr << "can't open input file " << param.input << endl;
      exit(-1);
    }
    fin.close();
  }

  // parse charchart
  if (param.charChart == "") {
    cerr << "please specify CharChart file with --charChart=<file> option" << endl;
    exit(0);
  }
  CharChart charChart;
  charChart.loadFromFile(param.charChart);

  try
  {
    cerr << "parse charChart file : " << param.charChart << endl;
//     cerr << "TODO: to implement at "<<__FILE__<<", line "<<__LINE__<<"!" <<std::endl;
//     exit(2);
//     charChart = 0;
/*    ParseCharClass parseCharClass;
    parseCharClass.parse(param.charChart);
    charChart = ParseChar::parse(param.charChart, parseCharClass);*/
  }
  catch (exception& e)
  {
    cerr << "Caught exception while parsing file " << param.charChart << endl;
    cerr << e.what() << endl;
    exit(-1);
  }

  if (param.extractKeys != "")
  {
    // just extract keys
    ofstream fout(param.extractKeys.c_str(), std::ofstream::binary);
    if (!fout.good())
    {
      cerr << "can't open file " << param.extractKeys << endl;
      exit(-1);
    }
    KeysLogger keysLogger(fout,&charChart,param.reverseKeys);

    cerr << "parse input file : " << param.input << endl;
    try
    {
      QXmlSimpleReader parser;
      //     parser->setValidationScheme(SAXParser::Val_Auto);
      //     parser->setDoNamespaces(false);
      //     parser->setDoSchema(false);
      //     parser->setValidationSchemaFullChecking(false);
      parser.setContentHandler(&keysLogger);
      parser.setErrorHandler(&keysLogger);
      QFile file(param.input.c_str());
      if (!file.open(QIODevice::ReadOnly))
      {
        std::cerr << "Error opening " << param.input << std::endl;
        return 1;
      }
      if (!parser.parse( QXmlInputSource(&file)))
      {
        std::cerr << "Error parsing " << param.input << " : " << parser.errorHandler()->errorString().toUtf8().constData() << std::endl;
        return 1;
      }
      else
      {
        std::cerr << std::endl;
      }
    }
    catch (const XMLException& toCatch)
    {
      std::cerr << "An error occurred  Error: " << toCatch.getMessage() << endl;
      throw;
    }
    fout.close();
  } else {
    // compile dictionaries
    
    cerr << "parse property code file : " << param.propertyFile << endl;
    PropertyCodeManager propcodemanager;
    propcodemanager.readFromXmlFile(param.propertyFile);
    
    cerr << "parse symbolicCode file : " << param.symbolicCodes << endl;
    map<string,LinguisticCode> conversionMap;
    propcodemanager.convertSymbolicCodes(param.symbolicCodes,conversionMap);
    cerr << conversionMap.size() << " code read from symbolicCode file" << endl;
/*    for (map<string,LinguisticCode>::const_iterator it=conversionMap.begin();
         it!=conversionMap.end();
         it++)
    {
      cerr << it->first << " -> " << it->second << endl;
    }*/
    
    AbstractAccessByString* access(0);
    if (param.fsaKey!="") {
      cerr << "load fsa access method : " << param.fsaKey << endl;
      FsaAccessSpare16* fsaAccess=new FsaAccessSpare16();
      fsaAccess->read(param.fsaKey);
      access=fsaAccess;
    } else {
      cerr << "ERROR : no access Keys defined !" << endl;
      exit(-1);
    }
    cerr << access->getSize() << " keys loaded" << endl;
    
    cerr << "parse input file : " << param.input << endl;
    DictionaryCompiler handler(&charChart,access,conversionMap,param.reverseKeys);

    QXmlSimpleReader parser;
//     parser->setValidationScheme(SAXParser::Val_Auto);
//     parser->setDoNamespaces(false);
//     parser->setDoSchema(false);
//     parser->setValidationSchemaFullChecking(false);
    try
    {
      parser.setContentHandler(&handler);
      parser.setErrorHandler(&handler);
      QFile file(param.input.c_str());
      if (!file.open(QIODevice::ReadOnly))
      {
        std::cerr << "Error opening " << param.input << std::endl;
        return 1;
      }
      if (!parser.parse( QXmlInputSource(&file)))
      {
        std::cerr << "Error parsing " << param.input << " : " << parser.errorHandler()->errorString().toUtf8().constData() << std::endl;
        return 1;
      }
    }
    catch (const XMLException& toCatch)
    {
      cerr << "An error occurred  Error: " << toCatch.getMessage() << endl;
      throw;
    }
    
    cerr << "write data to output file : " << param.output << endl;
    ofstream fout(param.output.c_str(),ios::out | ios::binary);
    if (!fout.good())
    {
      cerr << "can't open file " << param.output << endl;
      exit(-1);
    }
    handler.writeBinaryDictionary(fout);
    fout.close();
    delete access;
  }
  return EXIT_SUCCESS;
}
