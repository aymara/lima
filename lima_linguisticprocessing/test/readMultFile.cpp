/************************************************************************
 *
 * @file       readBoWFile.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Thu Oct  9 2003
 * @version    $Id: readBoWFile.cpp 9085 2008-02-26 14:15:55Z de-chalendarg $
 * copyright   Copyright (C) 2003 by CEA LIST (LVIC)
 *
 ***********************************************************************/

#include "linguisticProcessing/client/AnalysisHandlers/MultimediaDocumentReaderWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/MultimediaDocumentHandler.h"
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
#include <QtCore/QDir>
#include <QtCore/QCommandLineParser>

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
using namespace Lima::Common::Misc;
using namespace Lima::Handler;
using namespace Lima::Common::BagOfWords;

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

struct Param
{
  QStringList files;           // input file
  FormatType outputFormat = XML;    // format of the output file
  bool useIterator = false;           // use BoWTokenIterator to go through BoWText
  bool useIndexIterator = false;      // use IndexElementIterator to go through BoWText
  unsigned int maxCompoundSize = 0; // maximum size of compounds (only if useIndexIterator)
  QString resourcesPath;
  QString configPath;
  QString commonConfigFile = "lima-common.xml";
  QString extension;
};

Param readCommandLineArguments()
{
  Param param;
  QStringList configDirs = buildConfigurationDirectoriesList(
      QStringList({"lima"}), QStringList());
  param.configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  QStringList resourcesDirs = buildResourcesDirectoriesList(
      QStringList({"lima"}), QStringList());
  param.resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);


  QCommandLineParser parser;
  parser.setApplicationDescription("LIMA analyzed multimedia documents reader.");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("files",
                               QCoreApplication::translate("main",
                                                           "File(s) to analyze."),
                               "[file...]");
  parser.addOption(
    QCommandLineOption(
      QStringList() << "use-iterator",
      QCoreApplication::translate("main",
                                  "Whether to use an iterator on compounds or not") )
                  );
  parser.addOption(
    QCommandLineOption(
      QStringList() << "use-index-iterator",
      QCoreApplication::translate("main",
                                  "Whether to use an iterator as in index on compounds or not"))
                  );
  parser.addOption(
    QCommandLineOption(
      QStringList() << "output-format",
      QCoreApplication::translate("main", "The chosen output format"),
      "outputFormat", "xml")
                  );
  parser.addOption(
    QCommandLineOption(
      QStringList() << "common-config-file",
      QCoreApplication::translate("main", "The main common configuration file"),
      "commonConfigFile", "lima-common.xml")
                  );
  parser.addOption(
    QCommandLineOption(
      QStringList() << "maxCompoundSize" << "max-compound-size",
      QCoreApplication::translate("main",
                                  "The maximum length of generated compounds"),
      "maxCompoundSize", "0")
                  );
  parser.addOption(
    QCommandLineOption(
      QStringList() << "config-dir" << "config-path",
      QCoreApplication::translate("main",
                                  "The colon-separated paths to configuration files"),
      "configPath", qgetenv("LIMA_CONF"))
                  );
  parser.addOption(
    QCommandLineOption(
      QStringList() << "resources-dir" << "resources-path",
      QCoreApplication::translate("main",
                                  "The colon-separated paths to media resources"),
      "resourcesPath", qgetenv("LIMA_RESOURCES"))
                  );
  parser.addOption(
    QCommandLineOption(
      QStringList() << "extension",
      QCoreApplication::translate("main",
                                  "The extension of the output file. If not "
                                  "set, result is written on standard output."),
      "extension", ""
    )
  );
  parser.process(QCoreApplication::arguments());

  if (parser.isSet("use-index-iterator") && parser.isSet("use-iterator"))
  {
    std::cerr << "Cannot set both use-iterator and use-index-iterator options"
              << std::endl;
    QCoreApplication::exit(1);
  }
  else if (parser.isSet("use-iterator"))
  {
    param.useIterator = true;
  }
  else if (parser.isSet("use-index-iterator"))
  {
    param.useIndexIterator = true;
  }
  if (parser.isSet("output-format"))
  {
    if  (parser.value("output-format")=="text") param.outputFormat = TEXT;
    else if  (parser.value("output-format")=="xml") param.outputFormat = XML;
    else if  (parser.value("output-format")=="stat") param.outputFormat = STAT;
  }
  if (parser.isSet("maxCompoundSize"))
    param.maxCompoundSize = parser.value("maxCompoundSize").toUInt();
  if (parser.isSet("common-config-file"))
    param.commonConfigFile = parser.value("common-config-file");
  if (parser.isSet("config-path"))
    param.configPath = parser.value("config-path");
  if (parser.isSet("resources-path"))
    param.resourcesPath = parser.value("resources-path");
  if (parser.isSet("extension"))
    param.extension = parser.value("extension");

  param.files = parser.positionalArguments();
  return param;
}
// //a local writer to get stats from the BoW
// class SBoWStatWriter  : public BoWXMLWriter
// {
// public:
//   SBoWStatWriter():BoWXMLWriter(std::cout), m_nbDocs(0) {}
//   ~SBoWStatWriter() {}
//
//   void openSBoWNode(std::ostream& /*os*/,
//                     const Misc::GenericDocumentProperties* /*properties*/,
//                     const std::string& /*elementName*/)
//   {}
//
//   void openSBoWIndexingNode(std::ostream& /*os*/,
//                             const Misc::GenericDocumentProperties* /*properties*/,
//                             const std::string& /*elementName*/)
//   {
//     m_nbDocs++;
//   }
//
//   void processSBoWText(std::ostream& /*os*/,
//                        const BoWText* /*boWText*/,
//                        bool /*useIterators*/)
//   {}
//
//   void processProperties(std::ostream& /*os*/,
//                          const Misc::GenericDocumentProperties* /*properties*/,
//                          bool /*useIterators*/)
//   {}
//
//   void closeSBoWNode(std::ostream& /*os*/)
//   {}
//
//   friend ostream& operator<<(ostream& os, const SBoWStatWriter& writer) {
//     return os << "NbDocs=" << writer.m_nbDocs;
//   }
//
// private:
//   SBoWStatWriter(const SBoWStatWriter&);
//   unsigned long m_nbDocs;
// };



// read part of structured document from a file, using the document buffer given as argument
void readSDocuments(std::istream& fileIn,
                    BoWDocument& document,
                    MultimediaBinaryReader& reader,
                    const QString& outputFile = "",
                    bool useIterator = false,
                    bool useIndexIterator = false
                   )
{
  std::ostream* output = &cout;
  if (!outputFile.isEmpty())
  {
    output = new std::ofstream(outputFile.toUtf8().constData(),
                               std::ofstream::binary);
    if (!output)
    {
      std::cerr << "cannot open output file [" << outputFile.toUtf8().constData()
                << "] for writing." << std::endl;
      QCoreApplication::exit(1);
    }
  }
  MultimediaXMLWriter writer(*output);
  writer.writeMultimediaDocumentsHeader();
  while (! fileIn.eof())
  {
    reader.readMultimediaDocumentBlock(fileIn,
                                       document,
                                       writer,
                                       useIterator,
                                       useIndexIterator);
  }
  writer.writeMultimediaDocumentsFooter();
  if (!outputFile.isEmpty())
  {
    static_cast<std::ofstream*>(output)->close();
    delete output;
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
  LIMA_UNUSED(argc)
  LIMA_UNUSED(argv)
  Param param = readCommandLineArguments();
  QsLogging::initQsLog(param.configPath);
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  Lima::AmosePluginsManager::changeable().loadPlugins(param.configPath);

  BOWLOGINIT;

  std::deque<std::string> langs;

  // initialize common
  Common::MediaticData::MediaticData::changeable().init(
          param.resourcesPath.toUtf8().constData(),
          param.configPath.toUtf8().constData(),
          param.commonConfigFile.toUtf8().constData(),
          langs);

  // read BoWFile and output documents

  uint64_t i=1;
  for (const auto& inputFile: param.files)
  {
    if (!param.extension.isEmpty())
    {
      std::cout << "\rReading file "<< i << "/" << param.files.size()
            << " ("  << std::setiosflags(std::ios::fixed)
            << std::setprecision(2) << (i*100.0/param.files.size()) <<"%) '"
            << inputFile.toUtf8().constData() << "'" /*<< std::endl*/
            << std::flush;
    }
    std::ifstream fileIn(inputFile.toUtf8().constData(), std::ifstream::binary);
    if (! fileIn)
    {
      std::cerr << "cannot open input file [" << inputFile.toUtf8().constData()
                << "]" << std::endl;
      QCoreApplication::exit(1);
    }
    MultimediaBinaryReader reader;
  //   try
  //   {
      reader.readHeader(fileIn);
  //   }
  //   catch (exception& e)
  //   {
  //     cerr << "Error: " << e.what() << endl;
  //     exit(1);
  //   }

    QString outputFile;
    if (!param.extension.isEmpty())
    {
      outputFile = inputFile + "." + param.extension;
    }
    LINFO << "ReadMultimediaFile: file contains a StructuredDocument";
    BoWDocument document;
  //   try
  //   {
      readSDocuments(fileIn,
                     document,
                     reader,
                     outputFile,
                     param.useIterator,
                     param.useIndexIterator);
  //   }
  //   catch (exception& e) { cerr << "Error: " << e.what() << endl; }
    fileIn.close();
    ++i;
  }

  return EXIT_SUCCESS;
}
