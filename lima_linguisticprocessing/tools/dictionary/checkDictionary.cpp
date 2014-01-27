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

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  
  for (int i = 1 ; i < argc; i++)
  {
    std::string arg(argv[i]);
    if (arg == "--help")
    {
      std::cerr << "USAGE : " << argv[0] << " <dataFile> [entryId]" << std::endl;
      return 0;
    }
  }
  
  EnhancedAnalysisDictionary dico;
  dico.loadDataFile(argv[1]);
  cout << dico.getSize() << " entries in data" << endl;
  if (argc == 2) {
    for (uint64_t i=0;i<dico.getSize();i++)
    {
      displayEntry(dico,StringsPoolIndex(i));
    }
  } else {
    displayEntry(dico,StringsPoolIndex(atoi(argv[2])));
  }
  
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
