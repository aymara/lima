// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       convertBoWFormat.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Thu Oct  9 2003
 * @version    $Id: convertXmlToSBoW.cpp 9080 2008-02-25 18:33:51Z de-chalendarg $
 * copyright   Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLReader.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLWriter.h"
#include <fstream>

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima;
using namespace Lima::Common::BagOfWords;

//****************************************************************************
// declarations
//****************************************************************************
// help mode & usage
Q_GLOBAL_STATIC_WITH_ARGS(string, USAGE0, ("(Use readBowFile --xml to convert sBoW bin file to XML)\n"));
Q_GLOBAL_STATIC_WITH_ARGS(string, USAGE, ("usage : convertXmlToSBoW [options] fileIn fileOut\n"));

Q_GLOBAL_STATIC_WITH_ARGS(string, HELP, (std::string("convert structured-bag-of-words representations of texts from XML to bin (SBoW)\n")
+*USAGE0
+*USAGE
+"\n"
+"--help : this help page\n"
));

//****************************************************************************
// GLOBAL variable -> the command line arguments 
struct Param {
  string inputFile;           // input file
  string outputFile;          // output file
  bool help;                  // help mode
  ifstream*  fileIn;          // stored in global for convenience
  ofstream*  fileOut;         // (not a really pretty solution, I guess)
  BoWXMLReader* reader;
} param={"",
         "",
         false,
         0,
         0,
         0};

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  for(uint64_t i(1); i<argc; i++){
    string s(argv[i]);
    if (s=="-h" || s=="--help")
      param.help=true;
    else if (s[0]=='-') {
      cerr << "unrecognized option " <<  s 
        << endl;
      cerr << *USAGE << endl;
      exit(1);
    }
    else if (param.inputFile.empty()) {
      param.inputFile=s;
    }
    else {
      param.outputFile=s;
    }
  }
}

//**********************************************************************
//
// M A I N
//
//**********************************************************************
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
  
  if (argc<2) {    cerr << *USAGE; exit(1); }
  readCommandLineArguments(argc,argv);
  if (param.help) { cerr << *HELP; exit(1); }
  
  ofstream fileOut(param.outputFile.c_str(), std::ofstream::binary);
  if (! fileOut.good()) {
    cerr << "cannot open output file [" << param.outputFile << "]" << endl;
    exit(1);
  }
  BoWBinaryWriter writer;
  writer.writeHeader(fileOut,BOWFILE_SDOCUMENT);
  BoWXMLReader reader(param.inputFile,fileOut);
  // @todo
  return EXIT_SUCCESS;
}
