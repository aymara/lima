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

 #include "common/LimaVersion.h"
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
#include <iomanip>


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
Q_GLOBAL_STATIC_WITH_ARGS(string, USAGE, ("usage : readBoWFile [options] fileIn [fileIn ...]\n"));

Q_GLOBAL_STATIC_WITH_ARGS(string, HELP, (std::string("read binary files containing bag-of-words representations of texts or documents (example program)\n")
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
    exit(EXIT_FAILURE);
  }
}

//****************************************************************************
// GLOBAL variable -> the command line arguments
struct Param
{
  QStringList files;     // input file
  FormatType outputFormat;    // format of the output file
  bool useIterator;           // use BoWTokenIterator to go through BoWText
  bool useIndexIterator;      // use IndexElementIterator to go through BoWText
  uint64_t maxCompoundSize; // maximum size of compounds (only if useIndexIterator)
  bool help;                  // help mode
}
param={ QStringList(),
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
      cerr << "unrecognized option " <<  s << endl;
      cerr << *USAGE << endl;
      exit(EXIT_FAILURE);
    }
    else
    {
      param.files << QString::fromStdString(std::string(s));
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
  BagOfWords::BoWBlocType bt;
  switch (param.outputFormat)
  {
    case TEXT:
    {
      TextWriterBoWDocumentHandler writer(cout);
      while (! fileIn.eof())
      {
        bt = reader.readBoWDocumentBlock(fileIn, *document, writer, param.useIterator, param.useIndexIterator);
      }
      break;
    }
    case XML:
    {
      BoWXMLWriter writer(cout);
      writer.writeBoWDocumentsHeader();
      while (! fileIn.eof())
      {
          bt = reader.readBoWDocumentBlock(fileIn, *document, writer, param.useIterator, param.useIndexIterator);
      }
      writer.writeBoWDocumentsFooter();
      break;
    }
    case STAT:
    {
      SBoWStatWriter writer;
      while (! fileIn.eof())
      {
          bt = reader.readBoWDocumentBlock(fileIn, *document, writer, param.useIterator, param.useIndexIterator);
      }
      cout << writer << endl;
      break;
    }
  }
  if (bt!=BagOfWords::BoWBlocType::END_BLOC)
  {
    std::string err_msg = "input file ended prematurely.";
    std::cerr << "Error: " << err_msg << std::endl;
    throw LimaException(err_msg);
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
#ifndef DEBUG_LP
  try
  {
#endif
  QCoreApplication a(argc, argv);
  QCoreApplication::setApplicationName("readBowFile");
  QCoreApplication::setApplicationVersion(LIMA_VERSION);

  // Task parented to the application so that it
  // will be deleted by the application.
  Lima::LimaMainTaskRunner* task = new Lima::LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, &LimaMainTaskRunner::finished, &a, &QCoreApplication::exit);

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();
#ifndef DEBUG_LP
  }
  catch (const Lima::LimaException& e)
  {
    std::cerr << "Catched LimaException: " << e.what() << std::endl;
    return UNKNOWN_ERROR;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Catched std::exception: " << e.what() << std::endl;
    return UNKNOWN_ERROR;
  }
  catch (...)
  {
    std::cerr << "Catched unknown exception" << std::endl;
    return UNKNOWN_ERROR;
  }
#endif
}


int run(int argc,char** argv)
{
  QStringList configDirs = buildConfigurationDirectoriesList(
      QStringList({"lima"}), QStringList());
  QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  QStringList resourcesDirs = buildResourcesDirectoriesList(
      QStringList({"lima"}), QStringList());
  QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  QsLogging::initQsLog(configPath);
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();

  if (!Lima::AmosePluginsManager::changeable().loadPlugins(configPath))
  {
    cerr << "Can't load plugins." << endl;
    exit(EXIT_FAILURE);
  }

  std::string commonConfigFile = "lima-common.xml";
  std::deque<std::string> langs;
  if (argc<1)
  {
    cerr << *USAGE;
    exit(EXIT_FAILURE);
  }
  readCommandLineArguments(argc,argv);
  if (param.help)
  {
    cerr << *HELP;
    exit(EXIT_FAILURE);
  }

  BOWLOGINIT;

  // read BoWFile and output documents

  // initialize common
  Lima::Common::MediaticData::MediaticData::changeable().init(
          resourcesPath.toUtf8().constData(),
          configPath.toUtf8().constData(),
          commonConfigFile,
          langs);


  uint64_t i=1;
  QStringList error_files;
  for (const auto& inputFile: param.files)
  {

    std::cout << "\rReading file "<< i << "/" << param.files.size()
          << " ("  << std::setiosflags(std::ios::fixed)
          << std::setprecision(2) << (i*100.0/param.files.size()) <<"%) '"
          << inputFile.toUtf8().constData() << "'" /*<< std::endl*/
          << std::flush;

    std::ifstream fileIn(inputFile.toUtf8().constData(), std::ifstream::binary);
    if (! fileIn)
    {
      std::cerr << "cannot open input file [" << inputFile.toUtf8().constData()
                << "]" << std::endl;
      i++;
      continue;
    }

    BoWBinaryReader reader;
    try
    { // try to read the bowFile, and catch the exception in release mode
      // Let the exception rise in debug mode,
      // so we can traceback the exception occuring location in the debugger
      reader.readHeader(fileIn);
    }
    catch (exception& e)
    {
      cerr << "Error: input file does not seem to be a valid BowBinary file. Skip" << std::endl;
      fileIn.close();
      i++;
      error_files.append(inputFile);
      continue;
    }

    switch (reader.getFileType())  {
    case BoWFileType::BOWFILE_TEXT: {

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
      case TEXT: {
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
      fileIn.close();
      break;
    }
    case BoWFileType::BOWFILE_SDOCUMENT:
    {
      LINFO << "ReadBoWFile: file contains a StructuredBoWDocument";
      BoWDocument* document = new BoWDocument();
      try
      {
        readSDocuments(fileIn, document, reader);
      }
      catch (const std::exception& e)
      {
        std::cerr << "Error: failed to read this bow file, Skip : " << e.what() << std::endl;
        fileIn.close();
        ++i;
        error_files.append(inputFile);
        continue;
      }
      fileIn.close();
      delete document;
      break;
    }
  /*
    case BoWFileType::BOWFILE_DOCUMENT: {
      cerr << "ReadBoWFile: file contains a BoWDocument" << endl;
      BoWDocument* document=new BoWDocument();
      try
      {
          BoWXMLWriter::getInstance().writeBoWDocumentsHeader(cout);
          readDocuments(fileIn,document);
          BoWXMLWriter::getInstance().writeBoWDocumentsFooter(cout);
      }
      catch (const std::exception& e)
      {
        std::cerr << "Error: failed to read this bow file, Skip : " << e.what() << std::endl;
        fileIn.close();
        ++i;
        error_files.append(inputFile);
        continue;
      }
      fileIn.close();
      delete document;
      break;
    }
    case BoWFileType::BOWFILE_DOCUMENTST: {
      cerr << "ReadBoWFile: file contains a BoWDocumentST" << endl;
      BoWDocument* document=new BoWDocumentST();
      try
      {
        BoWXMLWriter::getInstance().writeBoWDocumentsHeader(cout);
        readDocuments(fileIn,document);
        BoWXMLWriter::getInstance().writeBoWDocumentsFooter(cout);
      }
      }
      catch (const std::exception& e)
      {
        std::cerr << "Error: failed to read this bow file, Skip : " << e.what() << std::endl;
        fileIn.close();
        ++i;
        error_files.append(inputFile);
        continue;
      }
      fileIn.close();
      delete document;
      break;
    }
  */
      default: {
        std::cerr << "format of file " << reader.getFileTypeString() << " not managed"
             << std::endl;
        fileIn.close();
        i++;
        continue;
      }
    }// end of switch type

    i++;
  } // end of loop on inputFiles

  if (error_files.size()) {
    std::cerr << std::endl << "Errors on " << error_files.size() << " file(s): " << std::endl
        << error_files.join("\n").toStdString() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
