// Copyright (C) 2007 Gael de Chalendar <kleag@free.fr>
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "LimaDBusServer.h"
#include <qdir.h>
#include <QByteArray>
#include <QCoreApplication>
#include <QNetworkInterface>
#include <QTimer>

#include "common/LimaCommon.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/tools/FileUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
// #ifdef WIN32
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
// #endif
using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
using namespace Lima::Common::XMLConfigurationFiles;

#define DEFAULT_PORT 8080

// to manage supported options.
#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  QsLogging::initQsLog();
  // Necessary to initialize factories under Windows
  Lima::AmosePluginsManager::single();

  std::string configPath;
  std::string limaServerConfigFile;
  // list of langs to initialize analyzer
  std::vector<std::string> languages;
  // list of pipelines to initialize analyzer
  std::vector<std::string> pipelines;
  // list of inacive units in pipelines
  std::vector<std::string> vinactiveUnits;
  int optional_port;
  // time before service stop
  int service_life = 0;

  // Declare the supported options.
  po::options_description desc("Usage");
  desc.add_options()
  ("help,h", "Display this help message")
  ("language,l", po::value< std::vector<std::string> >(&languages), "supported languages trigrams")
  ("config-dir", po::value<std::string>(&configPath)->default_value(qgetenv("LIMA_CONF").constData()==0?"":qgetenv("LIMA_CONF").constData(),"$LIMA_CONF"),
                                                                                                                  "Set the directory containing the (LIMA) configuration files")
  ("common-config-file", po::value<std::string>(&limaServerConfigFile)->default_value("lima-dbusserver.xml"),
                                                                                  "Set the LIMA server configuration file to use")
  ("pipeline,p", po::value< std::vector<std::string> >(&pipelines), "Set the linguistic analysis supported pipelines")
  ("inactive-units", po::value< std::vector<std::string> >(&vinactiveUnits),
   "Inactive some process units of the used pipeline")
  ("port", po::value< int >(&optional_port),
   "set the listening port")
  ("service-life,t", po::value< int >(&service_life),
   "set the service life (nb seconds)");

  po::positional_options_description p;
  p.add("input-file", -1);

  // store value of options
  po::variables_map varMap;
  // parse args and set values in store
  po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), varMap);
  po::notify(varMap);

  //
  if (varMap.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  if (configPath.empty())
  {
    configPath = "/usr/share/config/lima/";
  }

  // Parse configuration file of lima server
  // options in command line supercede options in configuration file
  // port
  QString fileName = findFileInPaths(QString::fromStdString(configPath),
                                     QString::fromStdString(limaServerConfigFile));
  XMLConfigurationFileParser configLimaServer(fileName);

  // analyzer languages
  {
    std::ostringstream oss;
    std::ostream_iterator<std::string> out_it (oss,", ");
    std::copy ( languages.begin(), languages.end(), out_it );
    std::cout << "main: before, languages = " << oss.str() << std::endl;
  }
  std::deque<std::string> langs;
  if (varMap.count("language"))
  {
    if( !languages.empty() )
    {
      langs.resize(languages.size());
      std::copy(languages.begin(), languages.end(), langs.begin());
    }
  }
  else
  {
    try
    {
      langs = configLimaServer.getModuleGroupListValues("dbus-server", "analyzer", "languages") ;
    }
    catch (NoSuchModule& e1)
    {
      qDebug() << "dbus-server module not defined in config file. Ininialize with all available languages";
    }
    catch (NoSuchParam& e3)
    {
      qDebug() << "dbus-server/analyzer/languages parameter not defined in config file. Ininialize with all available languages";
    }
  }
  {
    std::ostringstream oss;
    std::ostream_iterator<std::string> out_it (oss,", ");
    std::copy ( langs.begin(), langs.end(), out_it );
    std::cout << "main: after, langs = " << oss.str() << std::endl;
  }

  // analyzer pipelines
  // analyzer languages
  std::deque<std::string> pipes;
  {
    std::ostringstream oss;
    std::ostream_iterator<std::string> out_it (oss,", ");
    std::copy ( pipes.begin(), pipes.end(), out_it );
    std::cout << "main: before, pipes = " << oss.str() << std::endl;
  }
  if (varMap.count("pipelines")) {
    if( !pipelines.empty() ) {
      pipes.resize(pipelines.size());
      std::copy(pipelines.begin(), pipelines.end(), pipes.begin());
    }
  }
  else {
    try
    {
      pipes = configLimaServer.getModuleGroupListValues("dbus-server", "analyzer", "pipelines") ;
    }
    catch (NoSuchModule& e1)
    {
	qDebug() << "dbus-server module not defined in config file. Ininialize with all available pipelines";
    }
    catch (NoSuchParam& e3)
    {
      qDebug() << "dbus-server/analyzer/languages parameter not defined in config file. Ininialize with all available pipelines";
    }
  }
  {
    std::ostringstream oss;
    std::ostream_iterator<std::string> out_it (oss,", ");
    std::copy ( pipes.begin(), pipes.end(), out_it );
    std::cout << "main: after, pipes = " << oss.str() << std::endl;
  }

  // Create instance of server
  LimaDBusServer server(QString::fromStdString(configPath),
                        langs,
                        pipes,
                        service_life*1000,
                        &app);

  //return app.exec();
  int ret = app.exec();
  std::cout << "main: return " << ret;
  return ret;
}
