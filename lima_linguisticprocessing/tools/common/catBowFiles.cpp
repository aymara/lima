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
/************************************************************************
 *
 * @file       catBoWFiles.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Thu Oct  9 2003
 * @version    $Id: catBowFiles.cpp 9085 2008-02-26 14:15:55Z de-chalendarg $
 * copyright   Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "linguisticProcessing/common/BagOfWords/BinaryWriterBoWDocumentHandler.h"
#include <fstream>

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima;
using namespace Lima::Common::BagOfWords;

//****************************************************************************
// declarations
//****************************************************************************
// help mode & usage
static const string USAGE("usage : catBowFiles files\n");

static const string HELP("concatenates several BoW files into one, output on stdout\n"
+USAGE
+"\n"
);

//****************************************************************************
// GLOBAL variable -> the command line arguments
struct Param {
  vector<string> inputFiles;  // input files
  string outputFile;         // output file
  ostream* fileOut;         // output stream
  bool help;                 // help mode
} param={vector<string>(0),
         "",
   &cout,
         false};

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  for(uint64_t i(1); i<argc; i++){
    string s(argv[i]); 
    if (s=="-h" || s=="--help")
      param.help=true;
    else if (s.find("--output=",0)==0) {
      param.outputFile=string(s,9);
    }
    else if (s[0]=='-') {
      cerr << "unrecognized option " <<  s
           << endl;
      cerr << USAGE << endl;
      exit(1);
    }
    else {
      param.inputFiles.push_back(s);
    }
  }
  if (param.outputFile.empty()) {
    cerr << "no --output specified: use stdout" << endl;
    param.fileOut=&cout;
  }
  else {
    param.fileOut=new ofstream(param.outputFile.c_str(), std::ofstream::binary);
    if (! *(param.fileOut)) {
      cerr << "cannot open output file \"" << param.outputFile << "\"" << endl;
      exit(1);
    }
  }
}

// read documents in a file, using the document buffer given as argument
// (can be BoWDocument* or BoWDocumentST*)
void readAndWriteBoWDocuments(ifstream& fileIn, 
                              ostream& fileOut,
                              BoWDocument* document,
                              BoWBinaryReader& reader) {
  BinaryWriterBoWDocumentHandler writer(fileOut);
  while (! fileIn.eof()) {
    document->reinit();
    reader.readBoWDocumentBlock(fileIn, *document, writer, false);
  }
}

void readAndWriteBoWText(ifstream& fileIn, 
                         ostream& fileOut,
                         BoWText* text,
                         BoWBinaryReader& reader) {
  BoWBinaryWriter writer;
  while (! fileIn.eof()) {
    text->clear();
    reader.readBoWText(fileIn,*text);
    writer.writeBoWText(fileOut,*text);
  }
}

//**********************************************************************
//
// M A I N
//
//**********************************************************************
#include "common/misc/LimaMainTaskRunner.h"
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
  
  if (argc<1) {    cerr << USAGE; exit(1); }
  readCommandLineArguments(argc,argv);
  if (param.help) { cerr << HELP; exit(0); }

  BOWLOGINIT;

  // read BoWFile and output documents

  bool firstFile(true);
  BoWFileType type(BOWFILE_NOTYPE);

  for (vector<string>::const_iterator it=param.inputFiles.begin(), 
         it_end=param.inputFiles.end(); it!=it_end; it++) {

    ifstream fileIn((*it).c_str(), std::ifstream::binary);
    if (! fileIn) {
      LERROR << "file \"" << *it << "\" ignored: cannot open file";
      continue;
    }

    BoWBinaryReader reader;
    BoWBinaryWriter writer;
    try {
      reader.readHeader(fileIn);
    }
    catch (exception& e) {
      LERROR << "Error: " << e.what() << ": file " << *it << " ignored";
      continue;
    }
    
    if (firstFile) {
      type=reader.getFileType();
      writer.writeHeader(*(param.fileOut),type);
      LINFO << "catBowFiles: concatenate files of type " 
            << reader.getFileTypeString();
      firstFile=false;
    }
    else if (reader.getFileType() != type) {
      LERROR << "file \"" <<  *it
             << "\" ignored: incompatible file type "
             << reader.getFileTypeString() 
            ;
      continue;
    }
    
    switch (type) {
    case BOWFILE_TEXT: {
      BoWText* text=new BoWText();
      try {
        readAndWriteBoWText(fileIn,*(param.fileOut),text,reader);
      }
      catch (exception& e) { 
        cerr << "Error: " << e.what() << endl; 
      }
      delete text;
      break;
    }
    case BOWFILE_DOCUMENT: {
      BoWDocument* document=new BoWDocument();
      try {
        readAndWriteBoWDocuments(fileIn,*(param.fileOut),
                                 document,reader);
      }
      catch (exception& e) { 
        cerr << "Error: " << e.what() << endl; 
      }
      fileIn.close();
      delete document;
      break;
    }
    default: {
      cerr << "format of file " << *it << " (type: '" 
           << reader.getFileTypeString() << "'"
           << " not managed" << endl;
      continue;
    }
    }
  }
  ofstream* fileOut=dynamic_cast<ofstream*>(param.fileOut);
  if (fileOut) {
    fileOut->close();
  }
  return EXIT_SUCCESS;
}
