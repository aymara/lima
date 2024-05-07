// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2020 by CEA LIST
 *
 *   Main LIMA executable
 *
 ***************************************************************************/

#include "common/LimaCommon.h"
#include "common/LimaVersion.h"
#include "common/tools/LimaMainTaskRunner.h"
#include "common/MediaticData/mediaticData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"
#include "common/QsLog/QsLog.h"
#include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/QsLog/QsDebugOutput.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/MediaProcessors/MediaProcessors.h"

#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/LTRTextHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/XmlBowDocumentHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/MetaDataHandler.h"
#include "linguisticProcessing/core/EventAnalysis/EventHandler.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <boost/program_options.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QRegularExpression>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QTextStream>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "cmd_line_helpers.h"

namespace po = boost::program_options;

using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima;

void listunits();
std::ostream* openHandlerOutputFile(AbstractTextualAnalysisHandler* handler,
                                    const std::string& fileName,
                                    const std::set< std::string >& dumpers,
                                    const std::string& dumperId);
void closeHandlerOutputFile(std::ostream* ofs);
int run(int aargc, char** aargv);

int main(int argc, char **argv)
{
#ifndef DEBUG_LP
  try
  {
#endif
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("analyzeText");
    QCoreApplication::setApplicationVersion(LIMA_VERSION);

    // Task parented to the application so that it
    // will be deleted by the application.
    auto task = new LimaMainTaskRunner(argc, argv, run, &a);

    // This will cause the application to exit when
    // the task signals finished.
    QObject::connect(task, &LimaMainTaskRunner::finished,
                     &a, &QCoreApplication::exit);

    // This will run the task from the application event loop.
    QTimer::singleShot(0, task, SLOT(run()));

    return a.exec();
#ifndef DEBUG_LP
  }
  catch( const InvalidConfiguration& e )
  {
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

void initMetaData (const QString& meta, std::map<std::string, std::string>& metaData)
{
  // parse 'meta' argument to add metadata
  if (!meta.isEmpty())
  {
    auto metas = meta.split(",");
    for (const auto& aMeta: metas)
    {
      auto kv = aMeta.split(":");
      if (kv.size() != 2)
      {
        std::cerr << "meta argument '"<< aMeta.toStdString() << "' is not of the form XXX:YYY: ignored" << std::endl;
      }
      else
      {
        metaData[kv[0].toStdString()] = kv[1].toStdString();
      }
    }
  }
}

int run(int argc, char** argv)
{
  LIMA_UNUSED(argc);
  LIMA_UNUSED(argv);
  auto configDirs = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                      QStringList());
  auto configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  auto resourcesDirs = buildResourcesDirectoriesList(QStringList({"lima"}),
                                                     QStringList());
  auto resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  QsLogging::initQsLog(configPath);
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  if (!Lima::AmosePluginsManager::changeable().loadPlugins(configPath))
  {
    throw InvalidConfiguration("loadLibrary method failed.");
  }

  std::string strResourcesPath;
  std::string lpConfigFile;
  std::string commonConfigFile;
  std::string clientId;
  std::vector<std::string> languages;
  std::vector<std::string> dumpersv;
  std::vector<std::string> outputsv;
  std::string pipeline;
  std::vector<std::string> files;
  std::vector<std::string> vinactiveUnits;
  std::string meta;
  std::string splitMode;
  std::string strConfigPath;


  po::options_description desc(R"(Usage: analyzeText [OPTION]... [FILE]...
Analyze one or more text files. The default behavior is to use the main
pipeline in English but this can be tweaked using the options and some
environment variables.

Configuration files are searched in directories in the following order:
 1. Paths explicitly given using options below;
 2. Dirs from the colon-separated LIMA_CONF environment variable;
 3. XDG_DATA_HOME or ~/.local/share/ if not defined;
 4. LIMA_DIST/config
 5. /usr/share/config/lima

If the environment variable LIMA_SHOW_CONFIG_PATH is defined and non-empty,
then the list built is written on stderr. Use it if you don't understand what
happens.

Options)");
  desc.add_options()
  ("help,h", "Display this help message")
  ("version,v", QString::fromUtf8("Shows LIMA version: %1.").arg(LIMA_VERSION).toUtf8().constData())
  ("language,l", po::value< std::vector<std::string> >(&languages),
   "supported languages trigrams")
  ("dumper,d",
   po::value< std::vector<std::string> >(&dumpersv),
   "a dumper to use, can be repeated. Valid values are [bow (BowTextWriter), "
   "bowh (BowTextHandler), fullxml (SimpleStreamHandler), text (SimpleStreamHandler), "
   "event (EventHandler), xmlbow (XmlBowDocumentHandler)]. To use any of them, "
   "the corresponding dumper must be available in the pipeline configuration. "
   "Default is bow but must be set if any other is set")
  ("output,o",
   po::value< std::vector<std::string> >(&outputsv),
   "where to write dumpers output. By default, each dumper writes its results on a file whose name is the input file with a predefined suffix  appended. This option allows to chose another suffix or to write on standard output. Its syntax  is the following: <dumper>:<destination> with <dumper> a  dumper name and destination, either the value 'stdout' or a suffix.")
  ("client,c",
   po::value<std::string>(&clientId)->default_value("lima-coreclient"),
   "Set the linguistic processing client to use")
  ("resources-dir", po::value<std::string>(&strResourcesPath),
   "Set the directory containing the LIMA linguistic resources")
  ("config-dir", po::value<std::string>(&strConfigPath),
   "Set the directory containing the (LIMA) configuration files")
  ("common-config-file",
   po::value<std::string>(&commonConfigFile)->default_value("lima-common.xml"),
   "Set the LIMA common libraries configuration file to use")
  ("lp-config-file",
   po::value<std::string>(&lpConfigFile)->default_value("lima-analysis.xml"),
   "Set the linguistic processing configuration file to use")
  ("pipeline,p",
   po::value< std::string >(&pipeline)->default_value(""),
   "Set the linguistic analysis pipeline to use")
  ("input-file",
   po::value< std::vector<std::string> >(&files),
   "Set a text file to analyze")
  ("inactive-units",
   po::value< std::vector<std::string> >(&vinactiveUnits),
   "Inactive some process units of the used pipeline")
  ("availableUnits",
   "Ask the program to list its known processing units")
  ("meta",
   po::value< std::string >(&meta),
   "Sets metadata values, in the format data1:value1,data2:value2,...")
  ("split-mode,s",
   po::value< std::string >(&splitMode)->default_value("none"),
   "Split input files depending on this value and analyze each part independently. Possible values are 'none' (default), 'lines' to split on each line break, and 'para' to split on paragraphs (empty lines). For values different of 'none', dumpers should probably be on append mode. If information is to be passed from one analysis to the next (e.g. with BratDumper to have global identifiers on the whole document), a MetaDataDumper must be present at the end of the pipeline.")
  ;

  po::positional_options_description p;
  p.add("input-file", -1);

  po::variables_map vm;
  try
  {
    po::store(po::command_line_parser(argc, argv).
    options(desc).positional(p).run(), vm);
    po::notify(vm);
  }
  catch (const boost::program_options::unknown_option& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    return SUCCESS_ID;
  }
  else if (vm.count("version"))
  {
    std::cout << LIMA_VERSION << std::endl;
    return SUCCESS_ID;
  }

  if (!strResourcesPath.empty())
  {
    resourcesPath = QString::fromUtf8(strResourcesPath.c_str());
    resourcesDirs = resourcesPath.split(LIMA_PATH_SEPARATOR);
  }
  if (!strConfigPath.empty())
  {
    configPath = QString::fromUtf8(strConfigPath.c_str());
    configDirs = configPath.split(LIMA_PATH_SEPARATOR);
  }


  bool udInLangs = false;
  bool oneUdLangInLangs = false;
  for (const auto& lang: languages)
  {
    if (lang == "ud")
    {
      udInLangs = true;
      break;
    }
    if (lang.rfind("ud-", 0) == 0)
    {
      oneUdLangInLangs = true;
    }
  }
  if (!udInLangs and oneUdLangInLangs)
  {
    languages.push_back("ud");
  }
  std::deque<std::string> langs(languages.size());
  std::copy(languages.begin(), languages.end(), langs.begin());
  std::set<std::string> dumpers;
  if (dumpersv.empty())
  {
    dumpers.insert("text");
  }
  else
  {
    for (uint64_t i = 0; i < dumpersv.size(); i++)
    {
      dumpers.insert(dumpersv[i]);
    }
  }

  if (vm.count("availableUnits"))
  {
    listunits();
    return EXIT_SUCCESS;
  }
  if (langs.size()<1)
  {
    std::cerr << "no language defined !" << std::endl;
    return EXIT_FAILURE;
  }

  QMap< QString, QString > outputs;
  for(const auto& soutput : outputsv)
  {
    QStringList output = QString::fromStdString(soutput).split(":");
    if (output.size()==2)
    {
      outputs[output[0]] = output[1];
    }
    else
    {
      // Option syntax  error
      std::cerr << "syntax error in output setting:" << soutput << std::endl;
    }
  }
  std::map<std::string,std::string> metaData;
  initMetaData(QString::fromStdString(meta), metaData);

  std::set<std::string> inactiveUnits;
  for (const auto & inactiveUnit : vinactiveUnits)
  {
    inactiveUnits.insert(inactiveUnit);
  }

  auto beginTime = TimeUtils::getCurrentTime();

  // initialize common
  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath.toUtf8().constData(),
    configPath.toUtf8().constData(),
    commonConfigFile,
    langs,
    parse_options_line(QString::fromStdString(meta), ',', ':'));

  langs = Common::MediaticData::MediaticData::changeable().getMedias();

  if (pipeline.size() == 0)
  {
    if (langs.size() == 1 && langs.front().substr(0, 2) == "ud")
    {
      pipeline = "deepud";
    }
    else
    {
      pipeline = "main";
    }
  }

  std::deque<std::string> pipelines({pipeline});

  auto clientFactoryConfigured = false;
  for(const auto& configDir: configDirs)
  {
    if (QFileInfo::exists(configDir + "/" + lpConfigFile.c_str()))
    {
      // initialize linguistic processing
      XMLConfigurationFileParser lpconfig(
        (configDir + "/" + lpConfigFile.c_str()));
      LinguisticProcessingClientFactory::changeable().configureClientFactory(
        clientId,
        lpconfig,
        langs,
        pipelines);
      clientFactoryConfigured = true;
      break;
    }
  }
  if(!clientFactoryConfigured)
  {
//     std::cerr << "No LinguisticProcessingClientFactory were configured with" << configDirs.join(LIMA_PATH_SEPARATOR).toStdString() << "and" << lpConfigFile << std::endl;
    return EXIT_FAILURE;
  }

  auto client = std::dynamic_pointer_cast<AbstractLinguisticProcessingClient>(
          LinguisticProcessingClientFactory::single().createClient(clientId));

  // Set the handlers
  std::map<std::string, AbstractAnalysisHandler*> handlers;
  BowTextWriter* bowTextWriter = nullptr;
  EventAnalysis::EventHandler* eventHandler = nullptr;
  BowTextHandler* bowTextHandler = nullptr;
  SimpleStreamHandler* simpleStreamHandler = nullptr;
  SimpleStreamHandler* fullXmlSimpleStreamHandler = nullptr;
  LTRTextHandler* ltrTextHandler = nullptr;

  if (dumpers.find("event") != dumpers.end())
  {
    eventHandler = new EventAnalysis::EventHandler();
    handlers.insert(std::make_pair("eventHandler",
                                   eventHandler));
  }
  if (dumpers.find("bow") != dumpers.end())
  {
    bowTextWriter = new BowTextWriter();
    handlers.insert(std::make_pair("bowTextWriter",
                                   bowTextWriter));
  }
  if (dumpers.find("bowh") != dumpers.end())
  {
    bowTextHandler = new BowTextHandler();
    handlers.insert(std::make_pair("bowTextHandler",
                                   bowTextHandler));
  }
  if (dumpers.find("text") != dumpers.end())
  {
    simpleStreamHandler = new SimpleStreamHandler();
    handlers.insert(std::make_pair("simpleStreamHandler",
                                   simpleStreamHandler));
  }
  if (dumpers.find("fullxml") != dumpers.end())
  {
    fullXmlSimpleStreamHandler = new SimpleStreamHandler();
    handlers.insert(std::make_pair("fullXmlSimpleStreamHandler",
                                   fullXmlSimpleStreamHandler));
  }
  if (dumpers.find("ltr") != dumpers.end())
  {
    ltrTextHandler= new LTRTextHandler();
    handlers.insert(std::make_pair("ltrTextHandler",
                                   ltrTextHandler));
  }


  metaData["Lang"] = langs[0];

  if(files.size() == 0)
  {
      std::cerr << "No file to analyze." << std::endl;
  }


  uint64_t nfile = 0;
  for (const auto&  file : files)
  {
    // display the progress of the analysis
    nfile++;
    std::cerr << "\rAnalyzing "<< nfile << "/" << files.size()
              << " ("  << std::setiosflags(std::ios::fixed)
              << std::setprecision(2) << (nfile*100.0/files.size()) <<"%) '"
              << file << "'" << std::flush;

    // set the output files (to 0 if not in list)
    // remember to call closeHandlerOutputFile for each call to openHandlerOutputFile
    QString bowOut = outputs.contains("bow")
        ? (outputs["bow"] == "stdout"
            ? "stdout"
            : QString::fromUtf8((file).c_str())+outputs["bow"])
        : QString::fromUtf8((file).c_str())+".bin";
    auto bowofs  = openHandlerOutputFile(bowTextWriter,
                                         std::string(bowOut.toStdString()),
                                         dumpers,
                                         "bow");
    QString textOut = outputs.contains("text")
        ? (outputs["text"] == "stdout"
            ? "stdout"
            : QString::fromUtf8((file).c_str())+outputs["text"])
        : "stdout";
    auto txtofs  = openHandlerOutputFile(simpleStreamHandler,
                                         std::string(textOut.toStdString()),
                                         dumpers,
                                         "text");
    QString fullxmlOut = outputs.contains("fullxml")
        ? (outputs["fullxml"] == "stdout"
            ? "stdout"
            : QString::fromStdString(file)+outputs["fullxml"])
        : "stdout";
    auto fullxmlofs  = openHandlerOutputFile(fullXmlSimpleStreamHandler,
                                             std::string(fullxmlOut.toStdString()),
                                             dumpers,
                                             "fullxml");

    // loading of the input file
    TimeUtils::updateCurrentTime();
    QFile qfile(file.c_str());
    if (!qfile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      std::cerr << "Cannot open file " << file << " ! " << std::endl;
      continue;
    }
    metaData["FileName"] = file;

    if (splitMode == "lines")
    {
      std::cerr << "%%%%%%%%%%%%% LINES %%%%%%%%%%%%%%%" << std::endl;
      int lineNum = 0, nbLines = 0;
      std::cerr << "Counting number of lines…";
      while (!qfile.atEnd())
      {
        qfile.readLine();
        nbLines++;
      }
      qfile.seek(0);

      QTextStream in(&qfile);
      std::cerr << "\rStarting analysis";
      while (!in.atEnd())
      {
        lineNum++;
        auto percent = QString::number((lineNum*1.0/nbLines*100),'f',2);
        auto contentText = in.readLine();
        if ( (lineNum % 100) == 0)
        {
          std::cerr << "\rAnalyzed "<< lineNum << "/" << nbLines
                    << " (" << percent.toUtf8().constData()
                    << "%) lines. At " << qfile.pos();
        }
#ifndef DEBUG_LP
        try {
#endif
        // analyze it
        client->analyze(contentText,
                        metaData,
                        pipeline,
                        handlers,
                        inactiveUnits);
#ifndef DEBUG_LP
        }
        catch (const LinguisticProcessingException& e) {
          std::cerr << "LinguisticProcessing error on line " << lineNum << "["
                   << contentText.toStdString() << "]:" << e.what()
                   << std::endl;
          //allows the process to continue on next line
        }
        catch (const LimaException& e) {
          std::cerr << "Error on line " << lineNum << "["
                     << contentText.toStdString() << "]:" << e.what()
                     << std::endl;
          //allows the process to continue on next line
        }
#endif
      }
      qfile.close();
    }
    else if (splitMode == "para")
    {
      std::cerr << "%%%%%%%%%%%%% PARA %%%%%%%%%%%%%%%" << std::endl;
      auto contentText = QString::fromUtf8(qfile.readAll().constData());
      qfile.close();
      if (contentText.isEmpty())
      {
        std::cerr << "file " << file << " has empty input ! " << std::endl;
        continue;
      }

      // for paragraph handling, add additional metadata handler to pass information
      // from one analysis to the next
      // use local metadata to modify so that the metadata is reinitialized at each document
      std::map<std::string,std::string> paraMetaData=metaData;
      MetaDataHandler* metaDataHandler=new MetaDataHandler();
      handlers["metaDataHandler"]=metaDataHandler;

      QRegularExpression sep("\n\n\n*");
      //QStringList paragraphs=contentText.split(sep,QString::SkipEmptyParts);
      //for (const auto& par: paragraphs) {
      //get positions to set offset
      int prevpos = 0;
      int numpar = 0;

      // cheap trick to analyze the last paragraph without calling the analysis after the while loop
      contentText+=QString("\n\n");

      qsizetype pos = 0;
      QRegularExpressionMatch match;
      while ((pos = contentText.indexOf(sep, pos, &match)) != -1)
      {
        numpar++;
        QString paragraph=contentText.mid(prevpos,pos-prevpos);
        //std::cerr << "prevpos=" << prevpos << ", pos=" << pos << ", paragraph=" << paragraph.toStdString() << std::endl;
        if (! paragraph.isEmpty()) {
          // analyze it with the proper offset
          paragraph.append("\n");
          paraMetaData["StartOffset"]=std::to_string(prevpos);
#ifndef DEBUG_LP
          try {
#endif
            // analyze it
            client->analyze(paragraph,
                            paraMetaData,
                            pipeline,
                            handlers,
                            inactiveUnits);
#ifndef DEBUG_LP
          }
          catch (const LinguisticProcessingException& e) {
            std::cerr << "LinguisticProcessing error on paragraph " << numpar << "["
                     << paragraph.toStdString() << "]:" << e.what()
                     << std::endl;
            //allows the process to continue on next paragraph
          }
          catch (const LimaException& e) {
            std::cerr << "Error on paragraph " << numpar << "["
                      << paragraph.toStdString() << "]:" << e.what()
                      << std::endl;
            //allows the process to continue on next paragraph
          }
#endif
        }
        pos += match.capturedLength();
        prevpos=pos;
        // update metadata
        const std::map<std::string,std::string>& newMetaData=metaDataHandler->getMetaData();
        if (newMetaData.size()!=0) {
          paraMetaData=newMetaData;
        }
      }
    }
    else // default == none
    {
      auto contentText = QString::fromUtf8(qfile.readAll().constData());
      qfile.close();
      if (contentText.isEmpty())
      {
        std::cerr << "file " << file << " has empty input ! " << std::endl;
        continue;
      }

      // The input text MUST be UTF-8 encoded !!!
      TimeUtils::logElapsedTime("ReadInputFile");
      TimeUtils::updateCurrentTime();

    //   // Testing the forging of pipelines. Moved to LimaAnalyzer on client side
    //   auto langid = Lima::Common::MediaticData::MediaticData::single().getMediaId("ud-eng");
    //   auto pipe = MediaProcessors::changeable().getPipelineForId(langid, pipeline);
    //   auto managers = Lima::MediaProcessors::single().managers();
    //   {
    //     QString jsonGroupString =
    //         "{ \"name\":\"cpptftokenizer\", "
    //         "  \"class\":\"CppUppsalaTensorFlowTokenizer\", "
    //         "  \"model_prefix\": \"tokenizer-eng\" }";
    //     auto jsonGroup = QJsonDocument::fromJson(jsonGroupString.toUtf8()).object();
    //
    //     GroupConfigurationStructure unitConfig(jsonGroup);
    //
    //
    //     // managers (Manager*) used in init of ProcessUnits are stored in
    //     // m_pipelineManagers of MediaProcessors
    //     auto tokenizer = MediaProcessUnit::Factory::getFactory(
    //       "CppUppsalaTensorFlowTokenizer")->create(
    //         unitConfig,
    //         managers[langid]);
    //     pipe->push_back(tokenizer);
    //   }
    //   {
    //     // <group name="tfmorphosyntax" class="TensorFlowMorphoSyntax">
    //     //   <param key="model_prefix" value="morphosyntax-eng"/>
    //     //   <param key="embeddings" value="fasttext-eng.bin"/>
    //     // </group>
    //     QString jsonGroupString =
    //         "{ \"name\":\"tfmorphosyntax\", "
    //         "  \"class\":\"TensorFlowMorphoSyntax\", "
    //         "  \"model_prefix\": \"morphosyntax-eng\", "
    //         "  \"embeddings\": \"fasttext-eng.bin\" "
    //         "}";
    //     auto jsonGroup = QJsonDocument::fromJson(jsonGroupString.toUtf8()).object();
    //     GroupConfigurationStructure unitConfig(jsonGroup);
    //
    //     // managers (Manager*) used in init of ProcessUnits are stored in
    //     // m_pipelineManagers of MediaProcessors
    //     auto pu = MediaProcessUnit::Factory::getFactory(
    //       "TensorFlowMorphoSyntax")->create(
    //         unitConfig,
    //         managers[langid]);
    //     pipe->push_back(pu);
    //   }
    //   {
    // // <group name="conllDumper" class="ConllDumper">
    // //   <param key="handler" value="simpleStreamHandler"/>
    // //   <param key="fakeDependencyGraph" value="false"/>
    // // </group>
    //     QString jsonGroupString =
    //         "{ \"name\":\"conllDumper\", "
    //         "  \"class\":\"ConllDumper\", "
    //         "  \"handler\": \"simpleStreamHandler\" "
    //         "  \"fakeDependencyGraph\": \"false\", "
    //         "}";
    //     auto jsonGroup = QJsonDocument::fromJson(jsonGroupString.toUtf8()).object();
    //
    //     GroupConfigurationStructure unitConfig(jsonGroup);
    //
    //     // managers (Manager*) used in init of ProcessUnits are stored in
    //     // m_pipelineManagers of MediaProcessors
    //     auto pu = MediaProcessUnit::Factory::getFactory(
    //       "ConllDumper")->create(
    //         unitConfig,
    //         managers[langid]);
    //     pipe->push_back(pu);
    //   }


#ifndef DEBUG_LP
      try {
#endif
        // analyze it
        auto analysis = client->analyze(contentText, metaData, pipeline, handlers, inactiveUnits);
        // std::cerr << "Running process on newly created process unit" << std::endl;
        // tokenizer->process(*analysis);
        // std::cerr << "Done" << std::endl;
#ifndef DEBUG_LP
      }
      catch (const LinguisticProcessingException& e) {
        std::cerr << "LinguisticProcessing error on document " << file << ":" << e.what()
                 << std::endl;
        //allows the process to continue on next file
      }
      catch (const LimaException& e) {
        std::cerr << "Error on document " << file << ":" << e.what()
                  << std::endl;
        //allows the process to continue to next file
      }
#endif
    }

    // Close and delete opened output files
    closeHandlerOutputFile(bowofs);
    closeHandlerOutputFile(txtofs);
    closeHandlerOutputFile(fullxmlofs);
  }
  std::cout << std::endl;
  // free handlers
  if (eventHandler != 0)
    delete eventHandler;
  if (bowTextWriter!= 0)
    delete bowTextWriter;
  if (simpleStreamHandler!= 0)
    delete simpleStreamHandler;
  if (fullXmlSimpleStreamHandler!= 0)
    delete fullXmlSimpleStreamHandler;
  if (bowTextHandler!= 0) {
    // not handled in output file: just print on output (this should just be used for testing)
    std::cout << bowTextHandler->getBowText();
    delete bowTextHandler;
  }
  if (ltrTextHandler!= 0) {
    // not handled in output file: just print on output (this should just be used for testing)
    std::cout << ltrTextHandler->getLTRText();
    delete ltrTextHandler;
  }
  TIMELOGINIT;
  LINFO << "Total: "
        << TimeUtils::diffTime(beginTime,TimeUtils::getCurrentTime())
        << " ms";
  TimeUtils::logAllCumulatedTime("Cumulated time.");

  return SUCCESS_ID;
}

std::ostream* openHandlerOutputFile(AbstractTextualAnalysisHandler* handler,
                                    const std::string& fileName,
                                    const std::set<std::string>&dumpers,
                                    const std::string& dumperId)
{
  std::ostream* ofs = nullptr;
  if (dumpers.find(dumperId)!=dumpers.end())
  {
    if (fileName=="stdout")
    {
      ofs = &std::cout;
    }
    else
    {
      ofs = new std::ofstream(fileName.c_str(),
                              std::ios_base::out
                                | std::ios_base::binary
                                | std::ios_base::trunc);
    }
    if (ofs->good())
    {
      handler->setOut(ofs);
    }
    else
    {
      std::cerr << "failed to open file " << fileName << std::endl;
      delete ofs; ofs = 0;
    }
  }
  return ofs;
}

void closeHandlerOutputFile(std::ostream* ofs)
{
  if (ofs != 0 && dynamic_cast<std::ofstream*>(ofs)!=0)
  {
    dynamic_cast<std::ofstream*>(ofs)->close();
    delete ofs;
    ofs = nullptr;
  }
}

void listunits()
{
    {
      std::cout << "Available resources factories : " << std::endl;
      auto ids = AbstractResource::Factory::getRegisteredFactories();
      for (const auto& id : ids)
      {
        std::cout << "- " << id << std::endl;
      }
      std::cout << std::endl;
    }
    {
      std::cout << "Available process units factories : " << std::endl;
      auto ids = MediaProcessUnit::Factory::getRegisteredFactories();
      for (const auto& id: ids)
      {
        std::cout << "- " << id << std::endl;
      }
      std::cout << std::endl;
    }
    std::cout << "Available client factories are : " << std::endl;
    {
      auto ids = LinguisticProcessingClientFactory::single().getRegisteredFactories();
      for (const auto& id : ids)
        {
          std::cout << "- " << id << std::endl;
        }
        std::cout << std::endl;
    }
}
