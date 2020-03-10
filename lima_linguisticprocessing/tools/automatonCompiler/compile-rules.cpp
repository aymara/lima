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
/******************************************************************************
*
* File        : compile-rules.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Fri Oct 25 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
******************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libautomatonCompiler/compilerExceptions.h"
#include "libautomatonCompiler/recognizerCompiler.h"

#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "common/tools/LimaMainTaskRunner.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/MediaProcessors/MediaAnalysisDumper.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/time/timeUtilsController.h"
#include "common/tools/FileUtils.h"

#include "linguisticProcessing/core/Automaton/recognizer.h"
#include "linguisticProcessing/core/Automaton/automatonReaderWriter.h"
#include "linguisticProcessing/core/Automaton/automatonCommon.h" // for exceptions
#include "common/LimaCommon.h"
#include "common/MediaticData/mediaticData.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <string.h>

#include <QtCore/QCoreApplication>


using namespace Lima;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;

//****************************************************************************
// declarations
//****************************************************************************
// help mode & usage
Q_GLOBAL_STATIC_WITH_ARGS(std::string, USAGE,
                          ("usage : compile-rules [-h] -ooutputfile rulesfile\n"));

Q_GLOBAL_STATIC_WITH_ARGS(std::string, HELP,
                          ((std::string("A compiler for the rules of the Named Entities recognizer\n"
                         +*USAGE
                         +"\n"
+"-h : this help page\n"
+"--output=file        : name of the output file for the compiled rules\n"
// +"(or -ofile)\n"
+"\n"
+"--language=...       : specify the language of the recognizer\n"
+"--modex=...          : specify the name of the modex config file\n"
+"--pipeline=...       : specify the name of the pipeline for the modex\n"
+"--configDir=...      : specify the directory to find the config files (default is $LIMA_CONF)\n"
+"--resourcesDir=...   : specify the directory to find the resources (default is $LIMA_RESOURCES)\n"
+"--common-config-file=...   : =<configuration/file/name>  Optional. Default is lima-common.xml\n"
+"--lp-config-file=...       : =<configuration/file/name>  Optional. Default is lima-analysis.xml\n"
+"--encoding=...       : specify the encoding of the rules file\n"
+"--useDictionary      : uses a dictionary to reorganize rules\n"
+"--debug              : compiles in debug mode\n"
+"\n"
+"--listTriggers : list the triggers with the corresponding offest\n"
+"--bin (or -r)  : read a binary file containing compiled rules : if \n"
+"                 the --listTriggers is not set, print the rules on stdout\n"
+"\n"
+"rulesfile is the name of the file containing the rules in plain text\n"))));

//****************************************************************************
#define DEFAULT_COMMON_CONFIG "lima-common.xml"
#define DEFAULT_LP_CONFIG "lima-analysis.xml"
#define DEFAULT_ENCODING "utf8"

//****************************************************************************
// GLOBAL variable -> the command line arguments
struct Param
{
  std::string inputRulesFile; // name of the rules file
  std::string outputFile;     // name of the output file for the compiled rules
  std::string resourcesDir;   // directory for resources
  std::string configDir;      // directory for config files
  std::string commonConfigFile; // config file for linguisticData
  std::string lpConfigFile;     // config file for linguistic processing
  std::string modexConfigFile;  // config file for modex
  std::string pipeline;         // pipeline for modex (defined in config file)
  std::string language;       // language of the files
  std::string encoding;       // default encoding of rules files
  bool decompile;        // reads compiled rules
  bool listTriggers;     // list the triggers with their associated index
  bool useDictionary;    // use a dictionary to reorganize rules
  bool loadPossibleTypes;// force loading of possible types
  bool debug;            // compile in debug mode (store rule ids for debug purposes)
  bool help;             // help mode
}
param={"",
       "",
       "",
       "",
       DEFAULT_COMMON_CONFIG,
       DEFAULT_LP_CONFIG,
       "",
       "",
       "",
       DEFAULT_ENCODING,
       false,
       false,
       false,
       false,
       false,
       false};

void readCommandLineArguments(uint64_t argc, char *argv[])
{
//   bool languageSpecified(false);
  for(uint64_t i(1); i<argc; i++)
  {
    std::string s(argv[i]);
    if (s=="-h" || s=="--help")
    {
      param.help=true;
      std::cerr << *HELP;
      exit(1);
    }
    else if (s=="-r" || s=="--decompile" || s=="--bin")
      param.decompile = true;
    else if (s=="-l" || s=="--listTriggers")
      param.listTriggers = true;
    else if (s=="-d" || s=="--useDictionary")
      param.useDictionary = true;
    else if (s=="--loadPossibleTypes")
      param.loadPossibleTypes = true;
    else if (s.find("--encoding=",0)==0)
    {
      param.encoding=s.substr(11,s.length()-11);
    }
    else if (s.find("--resourcesDir=",0)==0)
    {
      param.resourcesDir = std::string(s,15);
    }
    else if (s.find("--configDir=",0)==0)
    {
      param.configDir = std::string(s,12);
    }
    else if (s.find("--common-config-file=")==0)
    {
      param.commonConfigFile = std::string(s,21);
    }
    else if (s.find("--lp-config-file=")==0)
    {
      param.lpConfigFile = std::string(s,17);
    }
    else if (s.find("--language=",0)==0)
    {
      param.language=s.substr(11,s.length()-11);
//       languageSpecified=1;
    }
    else if (s.find("-o",0)==0)
    {
      param.outputFile=s.substr(2,s.length()-2);
      if (param.outputFile == "")
      {
        i++;
        if (i >= argc)
        {
          std::cerr << "no output filename given" << std::endl;
          std::cerr << *USAGE << std::endl;
          exit(1);
        }
        else
        {
          param.outputFile = argv[i];
        }
      }
    }
    else if (s.find("--output=",0)==0)
    {
      param.outputFile = s.substr(9, s.length()-9);
    }
    else if (s.find("--modex=",0)==0)
    {
      param.modexConfigFile = std::string(s,8);
    }
    else if (s.find("--pipeline=",0)==0)
    {
      param.pipeline = std::string(s,11);
    }
    else if (s.find("--debug",0)==0)
    {
      param.debug = true;
    }
    else if (s[0]=='-')
    {
      std::cerr << "unrecognized option " <<  s << std::endl;
      std::cerr << *USAGE << std::endl;
      exit(1);
    }
    else
    { // file names
      param.inputRulesFile=s;
    }
  }

  //ensure all needed parameters are set
  if (param.language.empty())
  {
    std::cerr << "Error: missing --language=.. argument " << std::endl;
    exit(1);
  }

}

std::vector<std::string> getDynamicLibraryNames(
  XMLConfigurationFileParser& parser,
  const std::string& pipeline);

//****************************************************************************
//  M A I N
//****************************************************************************
#include "common/tools/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

int run(int aargc, char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  auto task = std::unique_ptr<Lima::LimaMainTaskRunner>(new Lima::LimaMainTaskRunner(argc, argv, run, &a));

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task.get(), &Lima::LimaMainTaskRunner::finished,
                   [](int returnCode){ QCoreApplication::exit(returnCode); } );

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task.get(), SLOT(run()));

  return a.exec();
}

int run(int argc, char** argv)
{
  readCommandLineArguments(argc,argv);

  QStringList configDirs = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                             QStringList());
  QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);
  if (!param.configDir.empty())
  {
    configPath = QString::fromUtf8(param.configDir.c_str());
    configDirs = configPath.split(LIMA_PATH_SEPARATOR);
  }

  QStringList resourcesDirs = buildResourcesDirectoriesList(QStringList({"lima"}),
                                                            QStringList());
  QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  if (!param.resourcesDir.empty())
  {
    resourcesPath = QString::fromUtf8(param.resourcesDir.c_str());
    resourcesDirs = resourcesPath.split(LIMA_PATH_SEPARATOR);
  }

  if (QsLogging::initQsLog(configPath) != 0)
  {
    std::cerr << "Call to QsLogging::initQsLog(\"" << configPath.toStdString()
              << "\") failed.";
    return EXIT_FAILURE;
  }

  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
//   Lima::AmosePluginsManiager::changeable().loadPlugins(configPath);

  if (!Lima::AmosePluginsManager::changeable().loadPlugins(configPath))
  {
    LOGINIT("Automaton::Compiler");
    LERROR << "compile-rules: Call to loadPlugins(\""
            << configPath << "\") failed.";
    std::cerr << "compile-rules: Call to loadPlugins(\""
              << configPath.toStdString() << "\") failed.";
    return EXIT_FAILURE;
  }

  std::deque<std::string> langs;
  langs.push_back(param.language);

  // initialize linguisticData
//   try
  {
    // initialize common
    LOGINIT("Automaton::Compiler");
    LDEBUG << "main: MediaticData::changeable().init( "
            << param.resourcesDir << ")...";

    MediaticData::changeable().init(
      resourcesPath.toUtf8().constData(),
      configPath.toUtf8().constData(),
      param.commonConfigFile,
      langs);
    LDEBUG << "main: MediaticData::changeable().init( "
            << param.resourcesDir << ") done!";

    /*
    * @TODO eviter l'initialisation des ressources dans compiles rules
    * On est oblige d'initialiser les ressources, juste pour recuperer un
    * Recognizer vide. Il faut pouvoir creer un Recognizer dans avoir a
    * initialiser les ressources
    */

    // initialize linguistic processing resources
    MediaId language = MediaticData::single().media(param.language);

    bool languageInitialized = false;
    QString lpConfigFile = findFileInPaths(configPath,
                                           param.lpConfigFile.c_str());
    if (!lpConfigFile.isEmpty())
    {
      if (QFileInfo::exists(lpConfigFile))
      {
        XMLConfigurationFileParser lpconfig(lpConfigFile.toUtf8().constData());
        const auto& langConfigFileName = lpconfig.getModuleGroupParamValue(
          "lima-coreclient",
          "mediaProcessingDefinitionFiles",
          param.language);
        QString langConfigFile = findFileInPaths(configPath,
                                                 langConfigFileName.c_str());
        if (!langConfigFile.isEmpty())
        {
          XMLConfigurationFileParser langParser(langConfigFile.toUtf8().constData());
          auto& module = langParser.getModuleConfiguration("Resources");
          LinguisticResources::changeable().initLanguage(
            language,
            module,
            false); // don't load mainkeys in stringpool, no use
          languageInitialized = true;
        }
      }
    }
    if(!languageInitialized)
    {
      LOGINIT("Automaton::Compiler");
      LERROR << "No language was configured configured with" << configDirs
              << "and" << param.lpConfigFile.c_str();
      std::cerr << "No language was configured configured with"
                << configDirs.join(":").toStdString()
                << "and" << param.lpConfigFile;
      return EXIT_FAILURE;
    }

    auto resReco = LinguisticResources::single().getResource(language,
                                                             "automatonCompiler");

    Recognizer& reco = *(static_cast< Recognizer* >(resReco));

    // look at the modex config file to find the dynamic libraries that must be loaded
    if (! param.modexConfigFile.empty())
    {
      LOGINIT("Automaton::Compiler");
      LDEBUG << "use modex file " << param.modexConfigFile;
      bool modexInitialized = false;
      Q_FOREACH(QString configDir, configDirs)
      {
        if (QFileInfo::exists(configDir + "/" + param.modexConfigFile.c_str()))
        {
          QString modexConfigFile = findFileInPaths(configPath,
                                                    param.modexConfigFile.c_str());
          if (!modexConfigFile.isEmpty())
          {
            XMLConfigurationFileParser modexconfig(modexConfigFile.toUtf8().constData());
            auto libraries = getDynamicLibraryNames(modexconfig,
                                                    param.pipeline);
            for (const auto& library: libraries)
            {
              LOGINIT("Automaton::Compiler");
              LDEBUG << "load library " << library;
              if (!Common::DynamicLibrariesManager::changeable().loadLibrary(library))
              {
                LERROR << "Call to loadLibrary(\"" << library << "\") failed.";
                return EXIT_FAILURE;
              }
            }
            modexInitialized = true;
          }
        }
      }
      if(!modexInitialized)
      {
        LOGINIT("Automaton::Compiler");
        LERROR << "No modex plugin was loaded with" << configDirs
                << "and" << param.modexConfigFile.c_str();
        return EXIT_FAILURE;
      }
    }
    //Recognizer reco;
    // if the rules file is in binary format and we want to print its content
    if (param.decompile)
    {
      try
      {
        //reco.readFromFile(param.inputRulesFile);
        AutomatonReader reader;
        reader.readRecognizer(param.inputRulesFile,reco);

        if (! param.listTriggers)
        {
          std::cout << reco;
        }
      }
      catch (std::exception& e)
      {
        std::cerr << "Error while reading rules file: "  << e.what() << std::endl;
        exit(1);
      }
    }
    else
    {
      // read the rules file in text format
      try
      {
        // auto ctrl2 = new TimeUtilsController("read file and build recognizer",
        //                                      true);
	// Lima::TimeUtilsController("read file and build recognizer", true);
        //std::cerr << "\rBuilding recognizer…";
        RecognizerCompiler::setRecognizerEncoding(param.encoding);
        RecognizerCompiler compiler(param.inputRulesFile);
        compiler.buildRecognizer(reco,language);
	// delete ctrl2;
      }
      catch (std::exception& e)
      {
        std::cerr << "recognizer construction failed:"<< e.what() << std::endl;
        exit(1);
      }

      // if we want to use a dictionary to reorganize rules
      if (param.useDictionary)
      {
	// Lima::TimeUtilsController("useDictionary", true);
        try
        {

          std::string dicostr = "mainDictionary";
          auto res = LinguisticResources::single().getResource(language,
                                                               dicostr);

          auto dico = static_cast< AnalysisDict::AbstractAnalysisDictionary* >(res);
          if (dico == nullptr)
          {
            throw std::runtime_error("dictionary not available for language "+
                                param.language);
          }
          // Reorganization not available
          // reco.reorganizeRules(*dico);
        }
        // when character is searched out of text buffer
        catch (std::exception& e)
        {
          std::cerr << "Error: " << e.what() << std::endl;
        }
      }

      // write recognizer to file
      try
      {
        if (! param.outputFile.empty())
        {
          //std::cerr << "\rWriting recognizer…";
          AutomatonWriter writer;
          LINFO << "writer.WritingRecognizer(language:" << language
                << "debug:" << param.debug << ")";
          writer.writeRecognizer(reco, param.outputFile, language, param.debug);
          //reco.writeToFile(param.outputFile);
        }
      }
      catch (Lima::LinguisticProcessing::Automaton::OpenFileException& e)
      {
        std::cerr << "OpenFileException: " << e.what() << std::endl;
        exit(1);
      }
    }

    if (param.listTriggers)
    {
      reco.listTriggers();
    }

  }
//   catch (InvalidConfiguration& e)
//   {
//     std::cerr << "Caught InvalidConfiguration: " << e.what() << std::endl;
//     throw e;
//   }
//   catch (NoSuchModule &)
//   {
//     std::cerr << e.what() << std::endl;
//   }
//   catch (NoSuchGroup& e)
//   {
//     std::cerr << e.what() << std::endl;
//   }
//   catch (NoSuchParam& )
//   {
//     std::cerr << e.what() << std::endl;
//   }
  //TIMELOGINIT;
  TimeUtils::logAllCumulatedTime("And at last");

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void addLibs(GroupConfigurationStructure& group,
            std::vector<std::string>& libNames)
{
  try
  {
    std::string libs = group.getAttribute("lib");
    std::string::size_type begin = 0;
    std::string::size_type i = libs.find(",",begin);
    while (i!=std::string::npos) {
      libNames.push_back(std::string(libs,begin,i-begin));
      begin=i+1;
      i=libs.find(",",begin);
    }
    libNames.push_back(std::string(libs,begin));
  }
  catch (NoSuchAttribute& ) {} // do nothing: optional
}

std::vector<std::string> getDynamicLibraryNames(
    XMLConfigurationFileParser& parser,
    const std::string& pipeline)
{
  std::vector<std::string> libNames;
  try
  {
    auto& module = parser.getModuleConfiguration("Processors");

    if (! pipeline.empty())
    {
      // search libs for given pipeline
      try
      {
        auto& group = module.getGroupNamed(pipeline);
        addLibs(group, libNames);
        // do it for all groups included in pipeline
        const auto& processUnits = group.getListsValueAtKey("processUnitSequence");
        for (const auto& processUnit: processUnits)
        {
          try
          {
            auto& pu = module.getGroupNamed(processUnit);
            addLibs(pu, libNames);
            // @todo: should be recursive
          }
          catch (NoSuchGroup &) {} // missing group for processUnit in pipeline: ignored
        }
        return libNames;
      }
      catch (NoSuchGroup &)
      {
        std::cerr << "Warning: config file for modex has no group '" << pipeline
                  << "' in 'Processors' : ignored" << std::endl;
      }
      catch (NoSuchList &) {} // no processUnitSequence list : ignored
    }

    // if no pipeline specified, go through all groups
    for (auto& mod: module)
    {
      // ModuleConfigurationStructure is a
      // map<string,GroupConfigurationStructure>
      addLibs(mod.second,libNames);
    }
  }
  catch (NoSuchModule &)
  {
    std::cerr << "Error: config file for modex has no module 'Processors'"
              << std::endl;
  }

  return libNames;
}
