/************************************************************************
 *
 * @file       convertBoWFormat.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Thu Oct  9 2003
 * @version    $Id: convertXmlToSBoW.cpp 9080 2008-02-25 18:33:51Z de-chalendarg $
 * copyright   Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "MultXmlReader.h"
#include "linguisticProcessing/core/XmlProcessingCommon.h"

#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/MediaticData/mediaticData.h"
#include "common/tools/LimaMainTaskRunner.h"

#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLWriter.h"
#include <fstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QTimer>


using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;
using namespace Lima::XmlReader;

//****************************************************************************
// declarations
//****************************************************************************
// help mode & usage
static const std::string USAGE0("(Use readBowFile --xml to convert sBoW bin file to XML)\n");
static const std::string USAGE("usage : convertXmlToSBoW [options] fileIn fileOut\n");
static const std::string HELP("convert structured-bag-of-words representations of texts from XML to bin (SBoW)\n"
+USAGE0
+USAGE
+"\n"
+"--help : this help page\n"
);

//****************************************************************************
// GLOBAL variable -> the command line arguments
struct Param
{
  std::string inputFile;           // input file
  std::string outputFile;          // output file
  bool help;                       // help mode
  std::deque<std::string> langs;
  std::ifstream*  fileIn;          // stored in global for convenience
  std::ofstream*  fileOut;         // (not a really pretty solution, I guess)
  MultXmlReader* reader;
  std::string lpConfigFile ;
  std::string commonConfigFile;
}
param =
{
    "",
    "",
    false,
    std::deque<std::string>(),
    0,
    0,
    0,
    "mm-analysis.xml",
    "mm-common.xml"
};

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  std::size_t pos;
  for(uint64_t i(1); i<argc; i++)
  {
    std::string arg(argv[i]);
    if (arg=="-h" || arg=="--help")
    {
      param.help=true;
    }
    else if ( (pos = arg.find("--language=")) != std::string::npos )
    {
      param.langs.push_back(arg.substr(pos + 11));
    }
    else if ( (pos = arg.find("--lp-config-file=")) != std::string::npos )
    {
      param.lpConfigFile = arg.substr(pos+17);
    }
    else if ( (pos = arg.find("--common-config-file=")) != std::string::npos )
    {
      param.commonConfigFile = arg.substr(pos+21);
    }
    else if (arg[0] == '-')
    {
      std::cerr << "unrecognized option " << arg << std::endl;
      std::cerr << USAGE << std::endl;
      exit(1);
    }
    else if (param.inputFile.empty())
    {
      param.inputFile=arg;
    }
    else
    {
      param.outputFile=arg;
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
  QStringList configDirs = buildConfigurationDirectoriesList(
      QStringList({"lima"}), QStringList());
  QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  QStringList resourcesDirs = buildResourcesDirectoriesList(
      QStringList({"lima"}), QStringList());
  QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  QsLogging::initQsLog(configPath);

  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  Lima::AmosePluginsManager::changeable().loadPlugins(configPath);

  setlocale(LC_ALL, "");

  std::deque<std::string> files;

  if (argc<2)
  {
    std::cerr << USAGE << std::endl;
    exit(1);
  }

  readCommandLineArguments(argc,argv);
  if (param.help)
  {
    std::cerr << HELP << std::endl;
    exit(1);
  }

  // initialize common
  Common::MediaticData::MediaticData::changeable().init(resourcesPath.toUtf8().constData(),
                                                        configPath.toUtf8().constData(),
                                                        param.commonConfigFile,
                                                        param.langs);

  std::deque<std::string> pipelines;

  XMLConfigurationFileParser lpconfig(findFileInPaths(configPath, param.lpConfigFile.c_str()).toUtf8().constData());
  LinguisticProcessingClientFactory::changeable().configureClientFactory(
            "lima-coreclient",
            lpconfig,
            param.langs,
            pipelines);

  std::ofstream fileOut(param.outputFile.c_str(), std::ofstream::binary);
  if (! fileOut.good())
  {
    std::cerr << "cannot open output file [" << param.outputFile << "]" << std::endl;
    exit(1);
  }

  // Reader constructor runs the parsing
  std::shared_ptr<const ShiftFrom> shiftFrom(new ShiftFrom(""));
  MultXmlReader reader(param.inputFile,fileOut, shiftFrom);
  return EXIT_SUCCESS;
}

