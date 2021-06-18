/*
 *    Copyright 2004-2014 CEA LIST
 *
 *    This file is part of LIMA.
 *
 *    LIMA is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    LIMA is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */
/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"

#include "linguisticProcessing/client/AbstractLinguisticProcessingClient.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include <linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h>

#include "EasySourceHandler.h"

#include <QtCore/QCoreApplication>
#include <QtXml/QXmlSimpleReader>

using namespace EasyProcessor;
using namespace Lima;
using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;

void usage(int argc, char* argv[]);

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();

  std::string resourcesPath;
  if (getenv("LIMA_RESOURCES") != 0)
    resourcesPath=std::string(getenv("LIMA_RESOURCES"));
  else
  {
    std::cerr << "Environment variable LIMA_RESOURCES must be defined." << std::endl;
    exit(1);
  }
  std::string configDir;
  if (getenv("LIMA_CONF") != 0)
    configDir=std::string(getenv("LIMA_CONF"));
  else
  {
    std::cerr << "Environment variable LIMA_CONF must be defined." << std::endl;
    exit(1);
  }
  std::string lpConfigFile="lima-analysis.xml";
  std::string commonConfigFile="lima-common.xml";
  std::string easyConfigFile="easyprocessor.conf.xml";
  std::string pipeline="easy";
  std::string dumper="fullXmlDumper";
  std::string clientId="lima-coreclient";
  std::string workingDir=".";
  bool removeTempFiles = true;
  std::deque<std::string> files;

  if (argc>1)
  {
    for (int i = 1 ; i < argc; i++)
    {
      std::string arg(argv[i]);
      int pos = -1;
      if ( arg[0] == '-' )
      {
        if (arg == "--help")
          usage(argc, argv);
        else if ( (pos = arg.find("--lp-config-file=")) != -1 )
          lpConfigFile = arg.substr(pos+14);
        else if ( (pos = arg.find("--common-config-file=")) != -1 )
          commonConfigFile = arg.substr(pos+20);
        else if ( (pos = arg.find("--config-dir=")) != -1 )
          configDir = arg.substr(pos+13);
        else if ( (pos = arg.find("--resources-dir=")) != -1 )
          resourcesPath = arg.substr(pos+16);
        else if ( (pos = arg.find("--pipeline=")) != -1 )
          pipeline = arg.substr(pos+11);
        else if ( (pos = arg.find("--dumper=")) != -1 )
          dumper = arg.substr(pos+9);
        else if ( (pos = arg.find("--client=")) != -1 )
          clientId=arg.substr(pos+9);
        else if ( (pos = arg.find("--working-dir=")) != -1 )
          workingDir=arg.substr(pos+14);
        else if ( (pos = arg.find("--easy-config-file=")) != -1 )
          easyConfigFile=arg.substr(pos+19);
        else if ( (pos = arg.find("--keep-temp-files")) != -1 )
          removeTempFiles = false;
        else if ( (pos = arg.find("--rm-temp-files")) != -1 )
          removeTempFiles = true;
        else usage(argc, argv);
      }
      else
      {
        files.push_back(arg);
      }
    }
  }

  setlocale(LC_ALL,"fr_FR.UTF-8");

  std::deque<std::string> langs;
  langs.push_back("fre");

  // initialize common
  MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);

  // initialize linguistic processing
  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(QString::fromUtf8(configDir.c_str()) + "/" + lpConfigFile.c_str());
  LinguisticProcessingClientFactory::changeable().configureClientFactory(
    clientId,
    lpconfig,
    MediaticData::single().getMedias());

  std::shared_ptr< AbstractProcessingClient > client = std::shared_ptr< AbstractProcessingClient >(LinguisticProcessingClientFactory::single().createClient(clientId));

  // read easy configfile
  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser easyconfig(QString::fromUtf8(configDir.c_str()) + "/" + easyConfigFile.c_str());
  std::map<std::string,std::string> groupeTypeMapping;
  std::map<std::string,std::string> relationTypeMapping;
  std::map<std::string,std::string> srcVxTag;
  std::map<std::string,std::string> tgtVxTag;
  try {
    groupeTypeMapping=easyconfig.getModuleGroupConfiguration("easyprocessor","groupes").getMapAtKey("typeMapping");
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchModule& e) {
    std::cerr << "No module named 'easyprocessor' in easy configuration file" << std::endl;
    exit(-1);
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchGroup& e) {
    std::cerr << "No group named 'groupes' in easy configuration file" << std::endl;
    exit(-1);
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchMap& e) {
    std::cerr << "No map named 'typeMapping' in easy configuration file" << std::endl;
    exit(-1);
  }
  try {
    relationTypeMapping=easyconfig.getModuleGroupConfiguration("easyprocessor","relations").getMapAtKey("typeMapping");
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchModule& e) {
    std::cerr << "No module named 'easyprocessor' in easy configuration file" << std::endl;
    exit(-1);
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchGroup& e) {
    std::cerr << "No group named 'relations' in easy configuration file" << std::endl;
    exit(-1);
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchMap& e) {
    std::cerr << "No map named 'typeMapping' in easy configuration file" << std::endl;
    exit(-1);
  }
  try {
    srcVxTag=easyconfig.getModuleGroupConfiguration("easyprocessor","relations").getMapAtKey("srcTag");
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchModule& e) {
    std::cerr << "No module named 'easyprocessor' in easy configuration file" << std::endl;
    exit(-1);
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchGroup& e) {
    std::cerr << "No group named 'relations' in easy configuration file" << std::endl;
    exit(-1);
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchMap& e) {
    std::cerr << "No map named 'srcTag' in easy configuration file" << std::endl;
    exit(-1);
  }
  try {
    tgtVxTag=easyconfig.getModuleGroupConfiguration("easyprocessor","relations").getMapAtKey("tgtTag");
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchModule& e) {
    std::cerr << "No module named 'easyprocessor' in easy configuration file" << std::endl;
    exit(-1);
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchGroup& e) {
    std::cerr << "No group named 'relations' in easy configuration file" << std::endl;
    exit(-1);
  } catch (Lima::Common::XMLConfigurationFiles::NoSuchMap& e) {
    std::cerr << "No map named 'tgtTag' in easy configuration file" << std::endl;
    exit(-1);
  }


  // create a parser for lima results
  QXmlSimpleReader parser;
  EasySourceHandler esh;
  parser.setContentHandler(&esh);
  parser.setErrorHandler(&esh);

  // process each easy source file
  for (std::deque<std::string>::const_iterator it=files.begin();
       it!=files.end();
       it++)
  {
    std::cout << "process tests in " << *it << std::endl;
    try
    {
      QFile file(it->c_str());
      if (!file.open(QIODevice::ReadOnly))
      {
        std::cerr << "Error opening " << *it << std::endl;
        return 1;
      }
      if (!parser.parse( QXmlInputSource(&file)))
      {
        std::cerr << "Error parsing " << *it << " : " << parser.errorHandler()->errorString().toUtf8().constData() << std::endl;
        return 1;
      }
    }
    catch (Lima::LimaException& e)
    {
      std::cerr << __FILE__ << ", line " << __LINE__ << ": caught LimaException : " << std::endl << e.what() << std::endl;
    }
    catch (std::logic_error& e)
    {
      std::cerr << __FILE__ << ", line " << __LINE__ << ": caught logic_error : " << std::endl << e.what() << std::endl;
    }
    catch (std::runtime_error& e)
    {
      std::cerr << __FILE__ << ", line " << __LINE__ << ": caught runtime_error : " << std::endl << e.what() << std::endl;
    }

    std::map<std::string, AbstractAnalysisHandler*> handlers;


    const std::vector<Enonce>& enonces=esh.getEnonces();
    std::ostringstream xmlfname;
    xmlfname << *it << ".easy.xml";
    std::ofstream xmlOutput(xmlfname.str().c_str());

    xmlOutput << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
    xmlOutput << "<!DOCTYPE DOCUMENT SYSTEM \"easy.dtd\">" << std::endl;
    xmlOutput << "<DOCUMENT id=\"lima\" fichier=\"" << *it << "\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << std::endl;

    // traiter les enonces un par un
//     int enonceIndex=1;
    for (std::vector<Enonce>::const_iterator enonceItr=enonces.begin();
         enonceItr!=enonces.end();
         enonceItr++)
    {

      //      std::cerr << "analyzing text : " << enonceItr->text << std::endl;

     // Write text on output file for debuging. File removed at the end of
     // processing by default
      std::ostringstream txtfname;
      txtfname << enonceItr->id << ".txt";
      std::ofstream txtfout(txtfname.str().c_str());
      txtfout << enonceItr->text;
      txtfout.close();

      SimpleStreamHandler cfdh;
      cfdh.setOut(&xmlOutput);
      handlers["simpleStreamHandler"] = &cfdh;

      std::map<std::string,std::string> metaData;
      metaData["Lang"]="fre";
      metaData["FileName"]=enonceItr->id + ".txt";
      metaData["DocumentName"]=enonceItr->id;
      try
      {
        client->analyze(enonceItr->text,metaData,pipeline,handlers);
      }
      catch (const LinguisticProcessingException& e)
      {
        std::cerr << "Catched LinguisticProcessingException: '" << e.what()
            << "' during handling of " << enonceItr->id << " in " << (*it)
            << std::endl;
        continue;
      }

      if (removeTempFiles)
      {
        std::ostringstream rmcommand;
        rmcommand << "rm -f " << txtfname.str() << "*";
        system(rmcommand.str().c_str());
      }
    }
    xmlOutput << "</DOCUMENT>" << std::endl;
  }
  return EXIT_SUCCESS;
}

void usage(int , char *argv[])
{
  std::cout << "usage: " << argv[0] << " [OPTIONS] [file1 [file2 [...]]] " << std::endl;
  std::cout << "\t--easy-config-file=</path/to/the/file> Optional. Default is $LIMA_CONF/easyprocessor.conf.xml" << std::endl;
  std::cout << "\t--working-dir=</path/to/the/working/dir> Optional. Default is ./" << std::endl;
  std::cout << "\t--resources-dir=</path/to/the/resources> Optional. Default is $LIMA_RESOURCES" << std::endl;
  std::cout << "\t--config-dir=</path/to/the/configuration/directory> Optional. Default is $LIMA_CONF" << std::endl;
  std::cout << "\t--lp-config-file=<configuration/file/name>\tOptional. Default is lima-analysis.xml" << std::endl;
  std::cout << "\t--common-config-file=<configuration/file/name>\tOptional. Default is lima-common.xml" << std::endl;
  std::cout << "\t--client=<clientId>\tOptional. Default is 'lp-coreclient'" << std::endl;
  std::cout << "\t--pipeline=<pipelineId>\tOptional. Default is 'main'" << std::endl;
  std::cout << "\t--dumper=<dumperId>\tOptional. Default is 'bowDumper'" << std::endl;
  std::cout << "\t----keep-temp-files and --rm-temp-files\tOptional. Default is to delete temp files" << std::endl;
  std::cout << "\twhere files are files to analyze." << std::endl;
  std::cout << std::endl;
  exit(0);
}
