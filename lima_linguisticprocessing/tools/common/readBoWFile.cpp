/*
    Copyright 2002-2019 CEA LIST

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
 * @file       readBoWFile.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Thu Oct  9 2003
 * @version    $Id: readBoWFile.cpp 9085 2008-02-26 14:15:55Z de-chalendarg $
 * copyright   Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "linguisticProcessing/common/BagOfWords/TextWriterBoWDocumentHandler.h"
#include "linguisticProcessing/common/BagOfWords/indexElementIterator.h"
#include "linguisticProcessing/common/BagOfWords/bowTokenIterator.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/indexElement.h"
#include "common/MediaticData/mediaticData.h"
#include "common/tools/FileUtils.h"

#include <QtCore/QCoreApplication>

#include <fstream>

namespace Lima {
  namespace Common {
    namespace BagOfWords {
      class BoWDocument;
    }
  }
}

using namespace std;
using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::Misc;

//****************************************************************************
// declarations
//****************************************************************************
// help mode & usage
Q_GLOBAL_STATIC_WITH_ARGS(string, USAGE, ("usage : readBoWFile [options] fileIn fileOut\n"));

Q_GLOBAL_STATIC_WITH_ARGS(string, HELP, (std::string("read a binary file containing a bag-of-words representations of texts or documents (example program)\n")
                         +*USAGE
                         +"\n"
+"--xml : use XML as output format\n"
+"--output-format=(text|xml) : format of the output\n"
+"--use-iterator use BoWToken iterator (include partial compounds)\n"
+"--use-index-iterator use IndexElement iterator  (structured ids)\n"
+"--help : this help page\n")
                        );

typedef enum {
  TEXT,
  XML,
  STAT
} FormatType;

FormatType readFormatType(const std::string& str)
{
  if (str == "text" ||
      str == "TEXT") { return TEXT; }
  else  if (str == "stat" ||
            str == "STAT" ||
            str == "nbdocs" ) { return STAT; }
  else if (str == "xml" ||
           str == "XML") { return XML; }
  else
  {
    cerr << "format type " << str << " is not defined";
    exit(1);
  }
}

//****************************************************************************
// GLOBAL variable -> the command line arguments
struct Param
{
  string inputFile;           // input file
  FormatType outputFormat;    // format of the output file
  bool useIterator;           // use BoWTokenIterator to go through BoWText
  bool useIndexIterator;      // use IndexElementIterator to go through BoWText
  uint64_t maxCompoundSize; // maximum size of compounds (only if useIndexIterator)
  bool help;                  // help mode
}
param={"",
       TEXT,
       false,
       false,
       0,
       false};

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  for(uint64_t i(1); i<argc; i++)
  {
    string s(argv[i]);
    if (s=="-h" || s=="--help")
      param.help=true;
    else if (s.find("--xml")==0)
      param.outputFormat=XML;
    else if (s.find("--nbdocs")==0)
      param.outputFormat=STAT;
    else if (s.find("--output-format=")==0)
      param.outputFormat=readFormatType(string(s,16));
    else if (s=="--use-iterator")
      param.useIterator=true;
    else if (s=="--use-index-iterator")
      param.useIndexIterator=true;
    else if (s.find("--maxCompoundSize=")==0) {
      param.maxCompoundSize=atoi(string(s,18).c_str());
    }
    else if (s[0]=='-')
    {
      cerr << "unrecognized option " <<  s
      << endl;
      cerr << *USAGE << endl;
      exit(1);
    }
    else
    {
      param.inputFile=s;
    }
  }
}

/*
// read documents in a file, using the document buffer given as argument
// (can be BoWDocument* or BoWDocumentST*)
void readDocuments(ifstream& fileIn, BoWDocument* document )
{
  while (! fileIn.eof())
  {
    document->reinit();
    BoWDocumentST* docST = dynamic_cast<BoWDocumentST*>(document);
    document->read(fileIn);
    if (! document->empty() || ! document->getStringValue(string("srcePrpty")).first.empty())
    {
      switch (param.outputFormat)
      {
          case TEXT:
          {
            if (param.useIterator)
            {
              BoWTokenIterator it(*document);
              while (! it.isAtEnd())
              {
                cout << it.getElement()->getOutputUTF8String() << endl;
                it++;
              }
            }
            else if (param.useIndexIterator)
            {
              IndexElementIterator it(*document);
              while (! it.isAtEnd())
              {
                cout << it.getElement() << endl;
                it++;
              }
            }
            else
            {
              docST = dynamic_cast<BoWDocumentST*>(document);
              if (docST != 0)
              {
                cout << *docST;
              }
              else
              {
                cout << *document;
              }
            }
            break;
          }
          case XML:
          {
            // TODO: use BoWXMLWriter as instances and no more as singleton
            BoWXMLWriter::getInstance().writeBoWDocument(cout,document,
                                                         param.useIterator,
                                                         param.useIndexIterator);
            break;
          }
      }
    }
    else if (fileIn.good())
    {
      cerr << "ReadBoWFile: document is empty and has no name" << endl;
    }
  }
}
*/

//a local writer to get stats from the BoW
class SBoWStatWriter  : public BoWXMLWriter
{
public:
  SBoWStatWriter():BoWXMLWriter(std::cout), m_nbDocs(0) {}
  ~SBoWStatWriter() {}

  void openSBoWIndexingNode(
                            const Misc::GenericDocumentProperties* /*properties*/,
                            const std::string& /*elementName*/) override
  {
    m_nbDocs++;
  }

  void processSBoWText(const BoWText* , bool ,
                         bool )  override
  {}

  void processProperties(const Misc::GenericDocumentProperties* /*properties*/, bool /*useIterators*/,
                         bool /*useIndexIterator*/) override
  {}

  void closeSBoWNode() override
  {}

  friend ostream& operator<<(ostream& os, const SBoWStatWriter& writer) {
    return os << "NbDocs=" << writer.m_nbDocs;
  }

private:
  SBoWStatWriter(const SBoWStatWriter&);
  uint64_t m_nbDocs;
};



// read part of structured document from a file, using the document buffer given as argument
void readSDocuments(ifstream& fileIn, BoWDocument* document, BoWBinaryReader& reader)
{
  switch (param.outputFormat)
  {
    case TEXT:
    {
      TextWriterBoWDocumentHandler writer(cout);
      while (! fileIn.eof())
      {
        reader.readBoWDocumentBlock(fileIn, *document, writer, param.useIterator, param.useIndexIterator);
      }
      break;
    }
    case XML:
    {
      BoWXMLWriter writer(cout);
      writer.writeBoWDocumentsHeader();
      while (! fileIn.eof())
      {
        reader.readBoWDocumentBlock(fileIn, *document, writer, param.useIterator, param.useIndexIterator);
      }
      writer.writeBoWDocumentsFooter();
    }
    case STAT:
    {
      SBoWStatWriter writer;
      while (! fileIn.eof())
      {
        reader.readBoWDocumentBlock(fileIn, *document, writer, param.useIterator, param.useIndexIterator);
      }
      cout << writer << endl;
      break;
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
  QStringList configDirs = buildConfigurationDirectoriesList(QStringList() << "amose" << "lima",QStringList());
  QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  QStringList resourcesDirs = buildResourcesDirectoriesList(QStringList() << "amose" << "lima",QStringList());
  QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  QsLogging::initQsLog(configPath);
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();

  if (!Lima::AmosePluginsManager::changeable().loadPlugins(configPath))
  {
    cerr << "Can't load plugins." << endl;
    exit(1);
  }

  std::string commonConfigFile = "lima-common.xml";
  std::deque<std::string> langs;
  if (argc<1)
  {
    cerr << *USAGE;
    exit(1);
  }
  readCommandLineArguments(argc,argv);
  if (param.help)
  {
    cerr << *HELP;
    exit(1);
  }

  BOWLOGINIT;

  // read BoWFile and output documents

  std::ifstream fileIn(param.inputFile.c_str(),std::ifstream::binary);
  if (! fileIn)
  {
    cerr << "cannot open input file [" << param.inputFile << "]" << endl;
    exit(1);
  }

  // initialize common
  Lima::Common::MediaticData::MediaticData::changeable().init(
          resourcesPath.toUtf8().constData(),
          configPath.toUtf8().constData(),
          commonConfigFile,
          langs);


  BoWBinaryReader reader;
  try
  {
    reader.readHeader(fileIn);
  }
  catch (exception& e)
  {
    cerr << "Error: " << e.what() << endl;
    exit(1);
  }

  switch (reader.getFileType())  {

  case BOWFILE_TEXT: {

    LINFO << "ReadBoWFile: file contains a BoWText";
    BoWText text;
    reader.readBoWText(fileIn,text);

    switch (param.outputFormat) {
    case XML: {
      BoWXMLWriter writer(cout);
      writer.writeBoWText(&text,
                          param.useIterator,
                          param.useIndexIterator);
      break;
    }
    case BOWFILE_NOTYPE: {
      if (param.useIterator) {
        BoWTokenIterator it(text);
        while (! it.isAtEnd()) {
          cout << it.getElement()->getOutputUTF8String() << endl;
          it++;
        }
      }
      else if (param.useIndexIterator) {
        LINFO << "ReadBoWFile: call IndexElementIterator with maxCompoundSize=" << param.maxCompoundSize;
        IndexElementIterator it(text,0,param.maxCompoundSize);
        while (! it.isAtEnd()) {
          cout << it.getElement() << endl;
          it++;
        }
      }
      else {
        cout << text << endl;
      }
      break;
    }
    default: cerr << "Error: output format not handled" << endl;
    }
    break;
  }
  case BOWFILE_SDOCUMENT:
  {
    LINFO << "ReadBoWFile: file contains a StructuredBoWDocument";
    BoWDocument* document=new BoWDocument();
    try
    {
      readSDocuments(fileIn, document, reader);
    }
    catch (exception& e) { cerr << "Error: " << e.what() << endl; }
    fileIn.close();
    delete document;
    break;
  }
/*
  case BOWFILE_DOCUMENT: {
    cerr << "ReadBoWFile: file contains a BoWDocument" << endl;
    BoWDocument* document=new BoWDocument();
    try
      {
        BoWXMLWriter::getInstance().writeBoWDocumentsHeader(cout);
        readDocuments(fileIn,document);
        BoWXMLWriter::getInstance().writeBoWDocumentsFooter(cout);
      }
    catch (exception& e) { cerr << "Error: " << e.what() << endl; }
    fileIn.close();
    delete document;
    break;
  }
  case BOWFILE_DOCUMENTST: {
    cerr << "ReadBoWFile: file contains a BoWDocumentST" << endl;
    BoWDocument* document=new BoWDocumentST();
    try {
      BoWXMLWriter::getInstance().writeBoWDocumentsHeader(cout);
      readDocuments(fileIn,document);
      BoWXMLWriter::getInstance().writeBoWDocumentsFooter(cout);
    }
    catch (exception& e) { cerr << "Error: " << e.what() << endl; }
    fileIn.close();
    delete document;
    break;
  }
*/
  default: {
    cerr << "format of file " << reader.getFileTypeString() << " not managed"
         << endl;
    exit(1);
  }
  }
  return EXIT_SUCCESS;
}
