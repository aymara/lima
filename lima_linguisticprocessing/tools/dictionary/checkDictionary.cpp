// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <cstdlib>

#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/AnalysisDict/EnhancedAnalysisDictionary.h"
#include "BasicHandler.h"

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::AnalysisDict;


// options
typedef struct ParamStruct
{
  std::string dataFileName;
  int entryId;
}
Param;

void displayEntry(EnhancedAnalysisDictionary& data,StringsPoolIndex index);

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
  
  for (int i = 1 ; i < argc; i++)
  {
    std::string arg(argv[i]);
    if (arg == "--help")
    {
      std::cerr << "USAGE : " << argv[0] << " <dataFile> [entryId]" << std::endl;
      return 0;
    }
  }
  
  EnhancedAnalysisDictionary dico(QString::fromUtf8(argv[1]));
  cout << dico.getSize() << " entries in data" << endl;
  if (argc == 2) {
    for (uint64_t i=0;i<dico.getSize();i++)
    {
      displayEntry(dico,StringsPoolIndex(i));
    }
  } else {
    displayEntry(dico,StringsPoolIndex(atoi(argv[2])));
  }
  return 0;
}

void displayEntry(EnhancedAnalysisDictionary& dico,StringsPoolIndex index)
{
  cout << "display entry " << index << " : " << endl;
  DictionaryEntry entry(dico.getEntry(index));
  if (entry.isFinal()) {
    cout << " ** final entry ** " << endl;
  }
  if (entry.isEmpty()) {
    cout << "empty entry ! " << endl;
  } else {
    cout << "entry is not empty : " << endl;
    BasicHandler handler(&cout);
    if (entry.hasLingInfos()) {
      cout << "has ling infos :" << endl;
      entry.parseLingInfos(&handler);
    }
    if (entry.hasAccentedForms()) {
      cout << "has accented forms : " << endl;
      entry.parseAccentedForms(&handler);
    }
    if (entry.hasConcatenated()) {
      cout << "has concatenated forms : " << endl;
      entry.parseConcatenated(&handler);
    }
  }
}
