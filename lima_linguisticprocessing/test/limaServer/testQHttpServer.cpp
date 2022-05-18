// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <qdir.h>
#include <QByteArray>
#include <QCoreApplication>
#include <QNetworkInterface>
#include <QTimer>

#include "common/LimaCommon.h"
#include "common/QsLog/QsLog.h"
#include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <iostream>

#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;

#define DEFAULT_PORT 8080

// to manage supported options.
#include <boost/program_options.hpp>
namespace po = boost::program_options;


int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  QsLogging::initQsLog();
  std::cerr << "QsLog initialized" << std::endl;

  // Declare the supported options.
  boost::program_options::options_description desc("Usage");
  // po::options_description desc("Usage");

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
  std::cerr << "option parsed " << std::endl;

  // Parse configuration file of lima server
  // options in command line supercede options in configuration file
  // port
 // Set the directory containing the (LIMA) configuration files")
  std::string configDir;
  std::string limaServerConfigFile;
  configDir = (qgetenv("LIMA_CONF").constData()==0?"":qgetenv("LIMA_CONF").constData(),"$LIMA_CONF");
  if (configDir.empty())
  {
    configDir = "/usr/share/config/lima/";
  }
  // "Set the LIMA server configuration file to use")
  limaServerConfigFile = "lima-server.xml";


  std::string fileName(configDir);
  fileName.append("/").append(limaServerConfigFile);
  XMLConfigurationFileParser configLimaServer(fileName);
  std::cerr << "Lima config parsed " << std::endl;
  quint16 port = DEFAULT_PORT;

  // analyzer pipelines
  // analyzer languages
  std::deque<std::string> pipes;
  {
    std::ostringstream oss;
    std::ostream_iterator<std::string> out_it (oss,", ");
    std::copy ( pipes.begin(), pipes.end(), out_it );
    std::cout << "main: before, pipes = " << oss.str() << std::endl;
  }

  /*
  QTimer t;
  // Create instance of server
  LimaServer server( configDir, langs, pipes, port, &app, &t);

  if (varMap.count("service-life")) {
    // Stop server and app after service-life seconds
    //note that we need to use t.connect, as main is not a QObject
    t.connect (&t, SIGNAL(timeout()), &server, SLOT(quit()));
    int time_out = service_life*1000;
    t.start(time_out);
  }
  */

  //return app.exec();
  int ret = app.exec();
  std::cout << "main: return " << ret;
  return ret;
}
