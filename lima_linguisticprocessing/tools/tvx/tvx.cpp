/*
    Copyright 2006-2016 CEA LIST
    Author: Olivier Mesnard <olivier.mesnard@cea.fr>
*/

/**
 * This program is designed to test the LIMA document readers whose role is to dispatch
 * analysis tasks to the various media analysis clients and to collect and transmit metadata
 * such as positions for example.
 *
 * Based on the tgv framework, the tests are described in XML files. For each data file listed,
 * the program calls the analysis and then reads the generated XML to check that it conforms to
 * what is specified in the test.
 */

#include "linguisticProcessing/common/tgv/TestCasesHandler.h"
#include "tools/tvx/ReaderTestCase.h"


#include "linguisticProcessing/core/XmlProcessingCommon.h"
#include "common/LimaCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/tools/FileUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"

#include "linguisticProcessing/client/AbstractLinguisticProcessingClient.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"

#include "linguisticProcessing/client/xmlreader/XmlReaderClientFactory.h"
#include "linguisticProcessing/client/xmlreader/AbstractXmlReaderClient.h"

#include <boost/foreach.hpp>

#include <QtXml/QXmlSimpleReader>
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDir>

#include <iostream>

using namespace std;

using namespace Lima::Common::Misc;
using namespace Lima::Common::TGV;
using namespace Lima::ReaderValidation;
using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::XmlReader;

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
  QObject::connect(task, &Lima::LimaMainTaskRunner::finished, &a, &QCoreApplication::exit);

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();

}


int run(int ,char** )
{
  auto configDirs = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                      QStringList());
  QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  auto resourcesDirs = buildResourcesDirectoriesList(QStringList({"lima"}),
                                                     QStringList());
  QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  QString lpConfigFile("lima-lp-tvx.xml");
  QString commonConfigFile("lima-common.xml");
  QString pipeline("indexer"); // image analyzed through Juilet
  QString useHandler("multimedia");
  QString clientId("lp-structuredXmlreaderclient");
  QString workingDir=QDir::currentPath();

  std::deque<std::string> medias;
  std::deque<std::string> pipelines;

  QCommandLineParser parser;
  parser.setApplicationDescription("LIMA multimedia documents analyzer.");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("files", QCoreApplication::translate("main", "File(s) to analyze."), "[file...]");
  QCommandLineOption progressOption(
      QStringList() << "p" << "Progress",
      QCoreApplication::translate("main", "Show progress. "));
  parser.addOption(progressOption);
  QCommandLineOption lpConfigFileOption(
      QStringList() << "analysis-config-file" << "lp-config-file",
      QCoreApplication::translate("main", "The main media analysis configuration file"),
      "lpConfigFile", lpConfigFile);
  parser.addOption(lpConfigFileOption);
  QCommandLineOption commonConfigFileOption(
      QStringList() << "common-config-file",
      QCoreApplication::translate("main", "The main common configuration file"),
      "commonConfigFile", commonConfigFile);
  parser.addOption(commonConfigFileOption);
  QCommandLineOption configPathOption(
      QStringList() << "config-dir" << "config-path",
      QCoreApplication::translate("main", "The colon-separated paths to configuration files"),
      "configPath", qgetenv("LIMA_CONF"));
  parser.addOption(configPathOption);
  QCommandLineOption resourcesPathOption(
      QStringList() << "resources-dir" << "resources-path",
      QCoreApplication::translate("main", "The colon-separated paths to media resources"),
      "resourcesPath", qgetenv("LIMA_RESOURCES"));
  parser.addOption(resourcesPathOption);
  QCommandLineOption mediasOption(
      QStringList() << "l" << "language" << "m" << "media",
      QCoreApplication::translate("main",
          "A media (or language) to activate. Must be repeated for each of them. The\n"
          "first encountered value of this option will be the default one when a tag\n"
          "in an analyzed file is not associated to any media in the configuration."),
          "media");
  parser.addOption(mediasOption);
  QCommandLineOption pipelineOption(
      QStringList() << "p" << "pipeline",
      QCoreApplication::translate("main", "The analysis pipeline to use."),
      "pipeline", pipeline);
  parser.addOption(pipelineOption);
  QCommandLineOption clientOption(
      QStringList() << "c" << "client",
      QCoreApplication::translate("main", "The analysis client to use."),
      "client", clientId);
  parser.addOption(clientOption);
  QCommandLineOption workingDirOption(
      QStringList() << "working-dir",
      QCoreApplication::translate("main", "Path to the working directory. Defaults to the path from which the program is launched."),
      "workingDir", workingDir);
  parser.addOption(workingDirOption);

  parser.process(QCoreApplication::arguments());

  if (parser.isSet(lpConfigFileOption))
    lpConfigFile = parser.value(lpConfigFileOption);
  if (parser.isSet(commonConfigFileOption))
    commonConfigFile = parser.value(commonConfigFileOption);
  if (parser.isSet(configPathOption))
    configPath = parser.value(configPathOption);
  if (parser.isSet(resourcesPathOption))
    resourcesPath = parser.value(resourcesPathOption);
  if (parser.isSet(mediasOption))
  {
    BOOST_FOREACH(const auto& media, parser.values(mediasOption))
      medias.push_back(media.toUtf8().constData());
  }
  else
  {
    std::cerr << "ERROR: At least one media should be set." << std::endl << std::endl;
    parser.showHelp(1);
  }
  if (parser.isSet(pipelineOption))
    pipeline = parser.value(pipelineOption);
  if (parser.isSet(clientOption))
    clientId = parser.value(clientOption);
  if (parser.isSet(workingDirOption))
    workingDir = parser.value(workingDirOption);

  QStringList files = parser.positionalArguments();


  pipelines.push_back(pipeline.toUtf8().constData());
///////////////////////////////////////////////////////////////////////////
  QsLogging::initQsLog(configPath);
  XMLREADERCLIENTLOGINIT;
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  if (!Lima::AmosePluginsManager::changeable().loadPlugins(configPath))
  {
    std::cerr << "Can't load plugins. Aborting." << std::endl;
    return 1;
  }

  setlocale(LC_ALL,"fr_FR.UTF-8");

  // initialize common
  MediaticData::changeable().init(
      resourcesPath.toUtf8().constData(),
      configPath.toUtf8().constData(),
      commonConfigFile.toUtf8().constData(),
      medias);

  bool xmlReaderClientFactoryConfigured = false;
  Q_FOREACH(QString limaConfDir, configDirs)
  {
    if (QFileInfo(limaConfDir + "/" + lpConfigFile).exists())
    {
      LDEBUG << "Configure XML reader client factory" << clientId << "with" << (limaConfDir + "/" + lpConfigFile);
      // initialize the factory of the XMLreader
      Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig((limaConfDir + "/" + lpConfigFile).toUtf8().constData());
      XmlReaderClientFactory::changeable().configureClientFactory(
          clientId.toUtf8().constData(),
          lpconfig,
          medias[0],
          medias,
          pipelines);
      xmlReaderClientFactoryConfigured = true;
      break;
    }
  }
  if(!xmlReaderClientFactoryConfigured)
  {
    XMLREADERCLIENTLOGINIT;
    LERROR << "No XmlReaderClientFactory were configured with" << configPath << "and" << lpConfigFile;
    return EXIT_FAILURE;
  }
  // Create the client

  LDEBUG << "Creating client";
  std::shared_ptr< AbstractXmlReaderClient > client(XmlReaderClientFactory::single().createClient(clientId.toUtf8().constData()));
  map<string, string> metaData;
  if(!medias.empty())
  {
      metaData["Lang"] = "LanguageNotYetSet";
  }

  std::string swd = std::string(workingDir.toUtf8().constData());
  ReaderTestCaseProcessor
    readerTestCaseProcessor(swd, client);

  QXmlSimpleReader xmlparser;
  TestCasesHandler tch(readerTestCaseProcessor);

  xmlparser.setContentHandler(&tch);
  xmlparser.setErrorHandler(&tch);

  for (auto it=files.begin(); it!=files.end(); it++)
  {
    const QString& fileName = *it;
    qDebug() << "process tests in " << fileName;
    try
    {
      QFile file(fileName);
      if (!file.open(QIODevice::ReadOnly))
      {
        std::cerr << "Error opening " << it->toUtf8().constData() << std::endl;
        return 1;
      }
      if (!xmlparser.parse( QXmlInputSource(&file)))
      {
        std::cerr << "Error parsing " << fileName.toUtf8().constData() << " : " << xmlparser.errorHandler()->errorString().toUtf8().constData() << std::endl;
        return 1;
      }
    }
    catch (Lima::LimaException& e)
    {
      std::cerr << "caught LimaException : " << endl << e.what() << std::endl;
    }
    catch (logic_error& e)
    {
      std::cerr << "caught logic_error : " << endl << e.what() << std::endl;
    }

    TestCasesHandler::TestReport resTotal;
    cout << endl;
    cout << "=========================================================" << endl;
    cout << endl;
    cout << "  TestReport :   " << fileName.toUtf8().constData() << " " << endl;
    cout << endl;
    cout << "\ttype           \tsuccess\tcond.\tfailed\ttotal" << endl;
    cout << "---------------------------------------------------------" << endl;
    for (map<string,TestCasesHandler::TestReport>::const_iterator resItr=tch.m_reportByType.begin();
         resItr!=tch.m_reportByType.end();
         resItr++)
    {
      string label(resItr->first);
      label.resize(15,' ');
      cout << "\t" << label << "\t" << resItr->second.success << "\t" << resItr->second.conditional << "\t" << resItr->second.failed << "\t" << resItr->second.nbtests << endl;
      resTotal.success+=resItr->second.success;
      resTotal.conditional+=resItr->second.conditional;
      resTotal.failed+=resItr->second.failed;
      resTotal.nbtests+=resItr->second.nbtests;
    }
    cout << "---------------------------------------------------------" << endl;
    cout << "\ttotal          \t" << resTotal.success << "\t" << resTotal.conditional << "\t" << resTotal.failed << "\t" << resTotal.nbtests << endl;
    cout << "=========================================================" << endl;
    cout << endl;
    tch.m_reportByType.clear();
  }
  return exitCode(tch);
}
