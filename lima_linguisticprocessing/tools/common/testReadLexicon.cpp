// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2019 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"

#include "common/Data/strwstrtools.h"
#include "common/misc/AbstractAccessByString.h"
#include "common/FsaAccess/FsaAccessSpare16.h"

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
using namespace Lima::Common::FsaAccess;

// options
typedef struct ParamStruct
{
  std::string keyFileName;
  std::string prefix;
  int offset;
}
Param;

void testAccessMethod(const Param& param)
{
  string resourcesPath = qEnvironmentVariableIsEmpty("LIMA_RESOURCES") ? "/usr/share/apps/lima/resources" : string(qgetenv("LIMA_RESOURCES").constData());
  string commonConfigFile = string("lima-common.xml");
  string configDir = qEnvironmentVariableIsEmpty("LIMA_CONF") ? "/usr/share/config/lima" : string(qgetenv("LIMA_CONF").constData());

  // Load lexicon
  Lima::Common::FsaAccess::FsaAccessSpare16* fsaAccess = new Lima::Common::FsaAccess::FsaAccessSpare16();
  fsaAccess->read(param.keyFileName);
  uint64_t size = fsaAccess->getSize();
  cerr <<  "FSA Access : " << size << " keys read from main keyfile" << endl;

  if( !param.prefix.empty() )
  {
    Lima::LimaString limaKey(Misc::utf8stdstring2limastring(param.prefix));
    Lima::LimaString extent = fsaAccess->getExtent(limaKey);
    std::cout << param.prefix << " => " << Misc::limastring2utf8stdstring(extent) << std::endl;
  }

  delete fsaAccess;
}

#include "common/tools/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

int run(int aargc, char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  LimaMainTaskRunner* task = new LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, &Lima::LimaMainTaskRunner::finished, [](int returnCode){ QCoreApplication::exit(returnCode); } );

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();
}


int run(int argc, char** argv)
{
  QsLogging::initQsLog();
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();

  setlocale(LC_ALL, "");

  // options reading
  Param param = {
                  std::string(),  // keyFileName
                  std::string(),  // prefix
                  -1,              // offset
                };

  for (int i = 1; i < argc; i++)
  {
    std::string arg(argv[i]);
    std::string::size_type pos = std::string::npos;
    if (arg == "--help")
    {
      std::cerr << "usage: " << argv[0]
      << " --help" << std::endl;
      std::cerr << "       " << argv[0]
      << " [--keyFileName=<string>]"
      << " [--prefix=<string>]"
      << " [--offset=<int>]"
      << std::endl;
      return 0;
    }
    else if ( (pos = arg.find("--keyFileName=")) != std::string::npos )
    {
      param.keyFileName = arg.substr(pos+14);
    }
    else if ( (pos = arg.find("--prefix=")) != std::string::npos )
    {
      param.prefix= arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--offset=")) != std::string::npos )
    {
      param.offset = atoi( (arg.substr(pos+9)).c_str() );
    }
  }

  cerr << "testReadLexicon: ";
  if(param.keyFileName.size())
  {
    cerr << "--keyFileName='" << param.keyFileName << "' ";
  }
  if(param.prefix.size())
  {
    cerr << "--prefix='" << param.prefix << "' ";
  }
  cerr << "--offset="<< param.offset << " ";
  cerr << endl;

  testAccessMethod( param );
  return EXIT_SUCCESS;
}
