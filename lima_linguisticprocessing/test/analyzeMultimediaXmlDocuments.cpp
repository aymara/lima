/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST - LIC2M                              *
 ***************************************************************************/
#include "linguisticProcessing/core/XmlProcessingCommon.h"

#include "linguisticProcessing/client/xmlreader/AbstractXmlReaderClient.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"

#include "linguisticProcessing/client/xmlreader/XmlReaderClientFactory.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"
#include "common/tools/LimaMainTaskRunner.h"

#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowDocumentHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowDocumentWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/SBowDocumentWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/XmlSimpleHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/XmlBowDocumentHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/MultimediaDocumentHandler.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "common/MediaProcessors/MediaProcessors.h"
#include "common/LimaVersion.h"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <boost/foreach.hpp>

#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>
#include <QDir>

using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::XmlReader;
using namespace Lima;
using namespace Lima;
using namespace std;

void listunits();



int run(int aargc,char** aargv);

int main(int argc, char **argv)
{
#ifndef DEBUG_LP
  try
  {
#endif
  QCoreApplication a(argc, argv);
  QCoreApplication::setApplicationName("analyzeXml");
  QCoreApplication::setApplicationVersion(LIMA_VERSION);

  // Task parented to the application so that it
  // will be deleted by the application.
  LimaMainTaskRunner* task = new LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, &LimaMainTaskRunner::finished,
                    &a, &QCoreApplication::exit);

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();
#ifndef DEBUG_LP
  }
  catch( const InvalidConfiguration& e ) {
    std::cerr << "Catched InvalidConfiguration: " << e.what() << std::endl;
    return UNSUPPORTED_LANGUAGE;
  }
  catch (const LinguisticProcessingException& e)
  {
    std::cerr << "Catched LinguisticProcessingException: " << e.what() << std::endl;
    return INVALID_CONFIGURATION;
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


int run(int argc, char** argv)
{
  auto configDirs = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                      QStringList());
  auto configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  auto resourcesDirs = buildResourcesDirectoriesList(QStringList({"lima"}),
                                                     QStringList());
  auto resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  QString lpConfigFile("lima-analysis.xml");
  QString commonConfigFile("lima-common.xml");
  QString pipeline("xml");
  QString useHandler("multimedia");
  QString clientId("lp-structuredXmlreaderclient");

  std::deque<std::string> medias;
  std::deque<std::string> pipelines;

  QCommandLineParser parser;
  parser.setApplicationDescription("LIMA multimedia documents analyzer.");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("files",
                               QCoreApplication::translate("main",
                                                           "File(s) to analyze."),
                               "[file...]");
  QCommandLineOption availableUnitsOption(
      QStringList() << "available-units",
      QCoreApplication::translate("main",
                                  "Show all available resources, process units and dumpers."));
  parser.addOption(availableUnitsOption);
  QCommandLineOption progressOption(
      QStringList() << "P" << "Progress",
      QCoreApplication::translate("main", "Show progress."));
  parser.addOption(progressOption);
  QCommandLineOption lpConfigFileOption(
      QStringList() << "lp-config-file",
      QCoreApplication::translate("main",
                                  "The main media analysis configuration file"),
      "lpConfigFile", "lima-analysis.xml");
  parser.addOption(lpConfigFileOption);
  QCommandLineOption commonConfigFileOption(
      QStringList() << "common-config-file",
      QCoreApplication::translate("main", "The main common configuration file"),
      "commonConfigFile", "lima-common.xml");
  parser.addOption(commonConfigFileOption);
  QCommandLineOption configPathOption(
      QStringList() << "config-dir" << "config-path",
      QCoreApplication::translate("main",
                                  "The colon-separated paths to configuration files"),
      "configPath", qgetenv("LIMA_CONF"));
  parser.addOption(configPathOption);
  QCommandLineOption resourcesPathOption(
      QStringList() << "resources-dir" << "resources-path",
      QCoreApplication::translate("main",
                                  "The colon-separated paths to media resources"),
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
  QCommandLineOption handlerOption(
      QStringList() << "H" << "handler",
      QCoreApplication::translate("main", "A handler to activate."),
      "handler", useHandler);
  parser.addOption(handlerOption);

  parser.process(QCoreApplication::arguments());

  //std::cerr<< configPath.toStdString() << std::endl;
  QsLogging::initQsLog(configPath);
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  if (!Lima::AmosePluginsManager::changeable().loadPlugins(configPath))
  {
    throw InvalidConfiguration("loadLibrary method failed.");
  }

  XMLREADERCLIENTLOGINIT;
  setlocale(LC_ALL, "");


  if (parser.isSet(availableUnitsOption))
  {
    listunits();
    return EXIT_SUCCESS;
  }
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
    for(const auto& media: parser.values(mediasOption))
    {
      medias.push_back(media.toUtf8().constData());
    }
  }
  else
  {
    std::cerr << "ERROR: At least one media should be set."
              << std::endl << std::endl;
    parser.showHelp(1);
  }
  if (parser.isSet(pipelineOption))
    pipeline = parser.value(pipelineOption);
  if (parser.isSet(clientOption))
    clientId = parser.value(clientOption);
  if (parser.isSet(handlerOption))
    useHandler = parser.value(handlerOption);

  QStringList files = parser.positionalArguments();

  pipelines.push_back(pipeline.toUtf8().constData());

  uint64_t beginTime = TimeUtils::getCurrentTime();

  try
  {
    // initialize common
    MediaticData::changeable().init(
        resourcesPath.toUtf8().constData(),
        configPath.toUtf8().constData(),
        commonConfigFile.toUtf8().constData(),
        medias);

    bool xmlReaderClientFactoryConfigured = false;
    for(const QString& limaConfDir: configDirs)
    {
      if (QFileInfo(limaConfDir + "/" + lpConfigFile).exists())
      {
        LDEBUG << "Configure XML reader client factory" << clientId << "with"
                << (limaConfDir + "/" + lpConfigFile);
        // initialize the factory of the XMLreader
        XMLConfigurationFileParser lpconfig((limaConfDir + "/" + lpConfigFile).toUtf8().constData());
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

    uint64_t i=1;
    #pragma omp parallel for
    for(auto filesIt = files.cbegin(); filesIt != files.cend(); filesIt++)
    {
      LDEBUG << "Creating client";
      std::shared_ptr< AbstractXmlReaderClient > client(XmlReaderClientFactory::single().createClient(clientId.toUtf8().constData()));
      map<string, string> metaData;
      if(!medias.empty())
      {
          metaData["Lang"] = "LanguageNotYetSet";
      }

      const auto& fileName = QFileInfo(*filesIt).absoluteFilePath();
      try
      {
        if (parser.isSet(progressOption))
        {
          std::cout << "Analyzing "<< i << "/" << files.size()
                << " ("  << std::setiosflags(std::ios::fixed)
                << std::setprecision(2) << (i*100.0/files.size()) <<"%) '"
                << fileName.toUtf8().constData() << "'" << std::endl;
        }
        // loading of the input file
        TimeUtils::updateCurrentTime();
        std::ifstream file(fileName.toStdString(), std::ifstream::binary);
        std::string text_s;
        readStream(file, text_s);
        QString qtext_s = QString::fromUtf8(text_s.c_str());

        QRegExp rx("(&[^;]*;)");
        int shift = 0;
        int indexofent = 0;

        QMap< uint64_t,uint64_t > shiftFrom;
        while ((indexofent = rx.indexIn(qtext_s, indexofent)) != -1)
        {
          int indexInResolved = indexofent-shift;
          shift += rx.cap(1).size()-1;
          shiftFrom.insert(indexInResolved, shift);
          LDEBUG << "indexofent:" << indexofent << "; indexInResolved:"
                  << indexInResolved << "; shift:" << shift;
          indexofent += rx.matchedLength();
        }

        if(qtext_s.trimmed().isEmpty())
        {
            std::cerr << "file " << fileName.toUtf8().constData()
                      << " has empty input ! " << std::endl;
            continue;
        }
        text_s = qtext_s.toUtf8().constData();
        TimeUtils::logElapsedTime("ReadInputFile");

        // set handlers
        TimeUtils::updateCurrentTime();

        QString outputFile;

        if(useHandler == "bow")
        {
          // for bow documents, use xmlDocumentHandler
          outputFile = fileName + ".bin";
        }
        else if(useHandler == "txt")
        {
          // for text dumpers (textDumper, compactFullDumper, DepTripletDumper,
          // etc.), use simpleStreamHandler
          outputFile = fileName + ".out";
        }
        else if(useHandler == "xml")
        {
          // for xml dumpers (simpleXmlDumper, fullXmlDumper, etc.),
          // use xmlSimpleHandler
          outputFile = fileName + ".xml";
        }
        else if(useHandler == "multimedia")
        {
          // for xml dumpers (simpleXmlDumper, fullXmlDumper, etc.),
          // use xmlSimpleHandler
          outputFile = fileName + ".mult";
        }
        else
        {
          std::cerr << "Error: unknown handler type " << useHandler
                    << ": must be [bow|txt|xml]" << std::endl;
          continue;
        }

        AbstractAnalysisHandler *handler = nullptr;
        std::string handlerName = "";
        if(useHandler == "bow")
        {
          // for bow documents, use xmlDocumentHandler
          // (attendu par bowDumperXml)
          handlerName = "xmlDocumentHandler";
          handler = new XmlBowDocumentHandler;
        }
        else if (useHandler=="txt")
        {
          // for text dumpers (textDumper,compactFullDumper,DepTripletDumper,
          // etc.), use simpleStreamHandler
          handlerName="simpleStreamHandler";
          handler=new XmlSimpleHandler();
        }
        else if(useHandler == "xml")
        {
          // for xml dumpers (simpleXmlDumper, fullXmlDumper, etc.),
          //use xmlSimpleHandler
          handlerName = "xmlSimpleHandler";
          handler = new XmlSimpleHandler;
        }
        else if(useHandler == "multimedia")
        {
          // Attendu par bowDumperXml
          handlerName = "xmlDocumentHandler";
          handler = new Lima::Handler::MultimediaDocumentHandler(shiftFrom);
          LDEBUG << "run handler "<<handlerName<<"is a MultimediaDocumentHandler"
                  << dynamic_cast<Lima::Handler::MultimediaDocumentHandler*>(handler)->shiftFrom().size()
                  << handler;
        }
        else
        {
          std::cerr << "unknown handler" << useHandler << std::endl;
          throw InvalidConfiguration(std::string("unknown handler") + useHandler.toUtf8().constData());
        }
        std::ofstream fout(outputFile.toUtf8().constData(),
                           std::ofstream::binary);
        if(handler != 0)
        {
          if (dynamic_cast<AbstractXmlDocumentHandler*>(handler) != 0)
            dynamic_cast<AbstractXmlDocumentHandler*>(handler)->setOut(&fout);
          LDEBUG << "run call handler setAnalysisHandler " << handlerName
                  << handler;
          client->setAnalysisHandler(handlerName, handler);
        }

        TimeUtils::logElapsedTime("PrepareAnalysis");

        // analyze
        metaData["FileName"] = fileName.toUtf8().constData();
        client->analyze(text_s, metaData, pipeline.toUtf8().constData());
        client->releaseAnalysisHandler(handlerName);
      }
      catch(NoSuchMap &e)
      {
        cerr << "Analysis failed on file " << (fileName)
              << ": (NoSuchMap) " << e.what() << endl;
      }
      catch(XMLConfigurationFileException &e)
      {
        cerr << "Analysis failed on file " << (fileName)
              << ": (XMLConfigurationFileException) " << e.what() << endl;
      }
      ++i;
    }
  }
  catch(InvalidConfiguration &e)
  {
    cerr << "Invalid configuration" << e.what() << endl;
  }

  cout << "Total: " << TimeUtils::diffTime(beginTime, TimeUtils::getCurrentTime())
        << " ms" << endl;

  return EXIT_SUCCESS;
}

void listunits()
{
    {
        cout << "Available resources factories : " << endl;
        deque<string> ids = AbstractResource::Factory::getRegisteredFactories();
        for(deque<string>::const_iterator it = ids.begin();
                it != ids.end();
                it++) {
            cout << "- " << *it << endl;
        }
        cout << endl;
    }
    {
        cout << "Available process units factories : " << endl;
        deque<string> ids = MediaProcessUnit::Factory::getRegisteredFactories();
        for(deque<string>::const_iterator it = ids.begin();
                it != ids.end();
                it++) {
            cout << "- " << *it << endl;
        }
        cout << endl;
    }
    std::cout << "Available client factories are : " << std::endl;
    {
//     deque<string> ids=LinguisticProcessingClientFactory::single().getRegisteredFactories();
//     for (deque<string>::iterator it=ids.begin();
//          it!=ids.end();
//          it++)
//     {
//       cout << "- " << *it << endl;
//     }
//     cout << endl;
    }
}
