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
 * @file       convertBoWFormat.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Thu Oct  9 2003
 * @version    $Id: convertBoWFormat.cpp 1612 2006-03-14 12:14:06Z romaric $
 * copyright   Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowFileHeader.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLReader.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLWriter.h"
#include <fstream>

#include <fstream>

using namespace std;
using namespace Lima;
using namespace Lima::Common::BagOfWords;

//****************************************************************************
// declarations
//****************************************************************************
// help mode & usage
static const string USAGE("usage : convertBoWFormat [options] fileIn fileOut\n");

static const string HELP("convert bag-of-words representations of texts\n"
+USAGE
+"\n"
+"--help : this help page\n"
);

typedef enum {
  FORMAT_BIN,
  FORMAT_TEXT,
  FORMAT_XML
} FormatType;

FormatType readFormatType(const std::string& str) {
  if (str == "bin" || str == "BIN" || 
      str == "bow" || str == "BoW" || str == "BOW") { return FORMAT_BIN; }
  else if (str == "text" ||
           str == "TEXT") { return FORMAT_TEXT; }
  else if (str == "xml" ||
           str == "XML") { return FORMAT_XML; }
  else {
    cerr << "format type " << str << " is not defined";
    exit(1);
  }
}

//****************************************************************************
// GLOBAL variable -> the command line arguments 
struct Param {
  string inputFile;           // input file
  string outputFile;          // output file
  FormatType inputFormat;     // format of the input file
  FormatType outputFormat;    // format of the output file
  bool documents;             // dealing with documents
  bool help;                  // help mode
  ifstream*  fileIn;          // stored in global for convenience
  ofstream*  fileOut;         // (not a really pretty solution, I guess)
  bool outputOnCout;
  BoWXMLReader* reader;
} param={"",
         "",
         FORMAT_BIN,
         FORMAT_BIN,
         false,
         false,
         0,
         0,
         false,
         0};

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  for(uint64_t i(1); i<argc; i++){
    string s(argv[i]);
    if (s=="-h" || s=="--help")
      param.help=true;
    else if (s.find("--input-format=")==0)
      param.inputFormat=readFormatType(string(s,15));
    else if (s.find("--output-format=")==0)
      param.outputFormat=readFormatType(string(s,16));
    else if (s =="--documents")
      param.documents=true;
    else if (s[0]=='-') {
      cerr << "unrecognized option " <<  s 
        << endl;
      cerr << USAGE << endl;
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
void openInputFile() {
  switch(param.inputFormat) {
  case FORMAT_BIN: {
    param.fileIn=new ifstream(param.inputFile.c_str());
    if (! *(param.fileIn)) {
      cerr << "cannot open input file [" << param.inputFile << "]" << endl;
      exit(1);
    }
    BoWFileHeader header;
    try {
      header.read(*(param.fileIn));
    }
    catch (VersionException& e) {
      cerr << e.what(); exit(1);
    }
    switch (header.getFileType()) {
    case BOWFILE_DOCUMENT:
    case BOWFILE_DOCUMENTST:
      param.documents=true;
      break;
    default:
      param.documents=false;
      break;
    }
    break;
  }
  case FORMAT_XML: {
    //not treated in this function
    break;
  }
  default: 
    cerr << "input format " << param.inputFormat << " not handled" << endl;
    exit(1);
  }
}

void openOutputFile() {
  switch (param.outputFormat) {
  case FORMAT_BIN: {
    if (param.inputFile.empty()) {
      cerr << "must specify an output file for binary output" << endl;
      exit(1);
    }
    param.fileOut=new ofstream(param.outputFile.c_str());
    if (! *(param.fileOut)) {
      cerr << "cannot open output file [" << param.outputFile << "]" << endl;
      exit(1);
    }
    BoWFileHeader header;
    if (param.documents) {
      header.setFileType(BOWFILE_DOCUMENT);
    }
    else {
      header.setFileType(BOWFILE_TEXT);
    }
    header.write(*(param.fileOut));
    break;
  }
  case FORMAT_TEXT: {
    if (param.outputFile.empty()) {
      param.outputOnCout=true;
    }
    else {
      param.fileOut=new ofstream(param.outputFile.c_str());
      if (! *(param.fileOut)) {
        cerr << "cannot open output file [" << param.outputFile << "]" << endl;
        exit(1);
      }
    }
    break;
  }
  case FORMAT_XML: {
    if (param.outputFile.empty()) {
      param.outputOnCout=true;
    }
    else {
      param.fileOut=new ofstream(param.outputFile.c_str());
      if (! *(param.fileOut)) {
        cerr << "cannot open output file [" << param.outputFile << "]" << endl;
        exit(1);
      }
    }
    break;
  }
  }
}

bool endOfInputFile() {
  if (param.fileIn) { return param.fileIn->eof();  }
//  if (param.wfileIn) { return param.wfileIn->eof();  }
  return true;
}

void closeFiles() {
  switch(param.inputFormat) {
  case FORMAT_BIN:          { param.fileIn->close(); break; }
  case FORMAT_XML:          { break; } // already closed
  case FORMAT_TEXT:         { break; } // not handled
  }
  switch (param.outputFormat) {
  case FORMAT_BIN:          { param.fileOut->close(); break; }
  case FORMAT_TEXT:         { param.fileOut->close(); break; }
  case FORMAT_XML:          { param.fileOut->close(); break; }
  }
}

void readTextFromInputFile(BoWText*& text) {
  switch (param.inputFormat) {
  case FORMAT_BIN: {
    text->read(*(param.fileIn));
    break;
  }
  case FORMAT_XML: {
    // not treated in this function
    break;
  }
  default:
    return;
  }
}

void readDocumentFromInputFile(BoWDocument*& document) {
  switch (param.inputFormat) {
  case FORMAT_BIN: {
    document->read(*(param.fileIn));
    break;
  }
  case FORMAT_XML: {
    // not treated in this function
    break;
  }
  default:
    return;
  }
}

void writeToOutputFile(const BoWText* text) {
  ostream* output;
  if (param.outputOnCout) { output=&cout; }
  else { output=param.fileOut; }

  switch (param.outputFormat) {
  case FORMAT_BIN: {
    text->write(*(param.fileOut));
    break;
  }
  case FORMAT_TEXT: {
    *output << *text;
    break;
  }
  case FORMAT_XML: {
//    BoWXMLWriter::getInstance().setOutputStream(*output);
    if (param.documents) {
      BoWXMLWriter::getInstance().
        writeBoWDocument(*output, static_cast<const BoWDocument*>(text));
    }
    else {
      BoWXMLWriter::getInstance().writeBoWText(*output, text);
    }
    break;
  }
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
  
  if (argc<2) {    cerr << USAGE; exit(1); }
  readCommandLineArguments(argc,argv);
  if (param.help) { cerr << HELP; exit(1); }
  
  if (param.inputFormat == FORMAT_XML) {
    if (param.outputFormat == FORMAT_BIN) {
      ofstream fileOut(param.outputFile.c_str());
      if (! fileOut.good()) {
        cerr << "cannot open output file [" << param.outputFile << "]" << endl;
        exit(1);
      }
      BoWFileHeader header(BOWFILE_SDOCUMENT);
      header.write(fileOut);
      BoWXMLReader reader(param.inputFile,fileOut);
    }
    else if (param.outputFormat == FORMAT_TEXT) {
      cerr << "TEXT format not supported for XML input" << endl;
    }
  }
  else {
    openInputFile();
    openOutputFile();
    if (param.documents) {
      BoWDocument* document=new BoWDocument;
      while (! endOfInputFile()) {
        document->reinit();
        readDocumentFromInputFile(document);
        //       wcerr << "document read=" << document << endl;
        if (! document->empty() || ! document->getDocName().empty()) {
          writeToOutputFile(document);
        }
        else {
          cerr << "document is empty and has no name" << endl;
        }
      }
    }
    else {
      BoWText* text=new BoWText;
      readTextFromInputFile(text);
      writeToOutputFile(text);
    }
    closeFiles();
  }
 
  return EXIT_SUCCESS;
}
