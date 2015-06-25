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
/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST                                      *
 *                                                                         *
 ***************************************************************************/


#include "common/Data/readwritetools.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "common/MediaticData/mediaticData.h"


#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include <QtCore/QCoreApplication>

using namespace std;
namespace po = boost::program_options;

using namespace Lima;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::FlatTokenizer;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;


//****************************************************************************
void usage(int argc, char* argv[]);

//****************************************************************************
#include "common/tools/LimaMainTaskRunner.h"
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
  
  string chartFile;
  std::string resourcesPath;
  std::string configDir;
  std::string lpConfigFile;
  std::string commonConfigFile;
  std::string clientId;
  std::string cesartOutput;
  std::vector<std::string> languages;
  std::string pipeline;
  std::vector<std::string> vfiles;
  bool lower;

  po::options_description desc("Usage");
  desc.add_options()
      ("help,h", "Display this help message")
      ("language,l", po::value< std::vector<std::string> >(&languages),
       "supported languages trigrams")
      ("mm-core-client", po::value<std::string>(&clientId)->default_value("lima-coreclient"), 
       "Set the linguistic processing client to use")
      ("chart-file,c", po::value<std::string>(&chartFile)->default_value(""), 
      "Set the characters map file to use")
      ("resources-dir", po::value<std::string>(&resourcesPath)->default_value(getenv("LIMA_RESOURCES")), 
       "Set the directory where to look for LIMA linguistic resources")
      ("config-dir", po::value<std::string>(&configDir)->default_value(getenv("LIMA_CONF")), 
       "Set the directory where to look for LIMA configuration files")
      ("common-config-file", po::value<std::string>(&commonConfigFile)->default_value("lima-common.xml"), 
       "Set the LIMA common libraries configuration file to use")
      ("lp-config-file", po::value<std::string>(&lpConfigFile)->default_value("lima-analysis.xml"), 
       "Set the linguistic processing configuration file to use")
      ("pipeline", po::value< std::string >(&pipeline)->default_value("none"), 
       "Set the linguistic analysis pipeline to use")
      ("lower", po::value< bool >(&lower)->default_value(false),
       "Ask the program to also change to lowercase")
      ("input-file", po::value< std::vector<std::string> >(&vfiles), 
       "Set a text file to analyze")
      ;
  
  po::positional_options_description p;
  p.add("input-file", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
      options(desc).positional(p).run(), vm);
  po::notify(vm);
 
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  
  if (languages.size()<1)
  {
    std::cerr << "no language defined !" << std::endl;
    return -1;
  }
  std::deque<std::string> langs(languages.size());
  std::copy(languages.begin(), languages.end(), langs.begin());
        
  std::deque<std::string> pipelines;
  pipelines.push_back(pipeline);

  // initialize common
  MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);

  // initialize linguistic processing
  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configDir + "/" + lpConfigFile);
  LinguisticProcessingClientFactory::changeable().configureClientFactory(
      clientId,
      lpconfig,
      langs,
      pipelines);

  MediaId langid = MediaticData::single().getMediaId(languages[0]);

  if (chartFile == "")
  {
    if (languages[0] == "")
    {
      cerr << "no codefile nor language specified !" << endl;
      exit(1);
    }
    chartFile=resourcesPath+"/LinguisticProcessings/"+languages[0]+"/tokenizerAutomaton-"+languages[0]+".xml";
  }

  // initialization
  AbstractResource* res=LinguisticResources::single().
      getResource(langid,"flatcharchart");
  CharChart* charChart =static_cast<CharChart*>(res);

  for (vector<string>::const_iterator argItr=vfiles.begin();
       argItr!=vfiles.end();
       argItr++)
  {
    std::ifstream fin(argItr->c_str(), std::ifstream::binary);
    string line;
    if (fin.good())
    {
      getline(fin,line);
      while (fin.good() && !fin.eof() && line!="")
      {
        LimaString str=utf8stdstring2limastring(line);
        LimaString res;
        if (lower)
        {
          res = charChart->toLower(str);
        }
        else
        {
          res = charChart->unmark(str);
        }
        cout << limastring2utf8stdstring(res) << endl;
        getline(fin,line);
      }
    }
  }
  return 0;
}

void usage(int, char *argv[])
{
  cout << "usage: " << argv[0] << " [config] [options] file1 file2" << endl;
  cout << "where [config] can be either :" << endl;
  cout << "  --chart=<xml chart file>" << endl;
  cout << "  --language=<lang> : use $LIMA_RESOURCES/LinguisticProcessings/<lang>/tokenizerAutomaton-<lang>.xml" << endl;
  cout << "and [options] are : " << endl;
  cout << "--lower : just lower words" << endl;
  exit(0);
}
