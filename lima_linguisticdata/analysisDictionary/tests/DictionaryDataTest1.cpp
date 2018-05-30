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
// clazy:skip


#include "DictionaryDataTest1.h"

#include "linguisticProcessing/core/AnalysisDict/DictionaryData.h"
// #include "linguisticProcessing/core/MorphologicAnalysis/MorphoSyntacticDataHandler.h"
// #include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
// #include "linguisticProcessing/core/LinguisticResources/AbstractAccessResource.h"
// #include "common/misc/fsaStringsPool.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/MediaticData/mediaticData.h"
// #include "common/QsLog/QsLog.h"
// #include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
// #include "common/QsLog/QsDebugOutput.h"
#include "common/tools/FileUtils.h"

using namespace Lima;
using namespace Lima::Common;
// using namespace Lima::Common::FsaAccess;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing;
// using namespace Lima::LinguisticProcessing::MorphologicAnalysis;
using namespace Lima::LinguisticProcessing::AnalysisDict;

void DictionaryDataTest1::initTestCase()
{
  QStringList configDirs = buildConfigurationDirectoriesList(QStringList() 
      << "lima",QStringList());
  QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  QStringList resourcesDirs = buildResourcesDirectoriesList(QStringList() 
      << "lima",QStringList());
  QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  std::string commonConfigFile=std::string("lima-common.xml");

  std::deque<std::string> langs({"fre"});

  QsLogging::initQsLog(configPath);
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  Lima::AmosePluginsManager::changeable().loadPlugins(configPath);

  // initialize common
  Common::MediaticData::MediaticData::changeable().init(
      resourcesPath.toUtf8().constData(),
      configPath.toUtf8().constData(),
      commonConfigFile,
      langs);
}

// create DictionaryData and check segment adress
void DictionaryDataTest1::test()
{
  QString dicoFile = findFileInPaths(
    Common::MediaticData::MediaticData::single().getResourcesPath().c_str(), 
    "LinguisticProcessings/fre/dicoDat-fre.dat");

  LinguisticProcessing::AnalysisDict::DictionaryData dicoData;
  dicoData.loadBinaryFile(dicoFile.toUtf8().constData());

  // uint64_t getSize() const;
//   uint64_t size = dicoData.getSize();
//   std::cerr << "size = " << dicoData.getSize() << std::endl;
//   QVERIFY(size == 660156);

  // test unsigned char* getEntryAddr(uint64_t index) const;
  uint64_t index(0);
  unsigned char*entryAddr = dicoData.getEntryAddr(index);
  std::cout << "dicoData.getEntryAddr[0]=" << hex << static_cast<void*>(entryAddr) << std::endl;
  std::cout << "dicoData..m_data=" << hex << static_cast<void*>(dicoData.m_data) << std::endl;
//  QVERIFY();
  // Que tester???
  // Il faudrait un petit dictionnaire bien contrôlé pour effectuer un jeu de test efficace
  
  
  // test inline unsigned char* getLingPropertiesAddr(uint64_t index) const;
  // unsigned char*
  // getLingPropertiesAddr(index);

  // test public uint64_t getSize() const;
  
  // test public static uint64_t readCodedInt(unsigned char* &p);
  
}

QTEST_MAIN(DictionaryDataTest1)

