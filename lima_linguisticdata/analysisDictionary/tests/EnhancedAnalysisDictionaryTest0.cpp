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

#include "EnhancedAnalysisDictionaryTest0.h"

#include "linguisticProcessing/core/AnalysisDict/EnhancedAnalysisDictionary.h"

#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/FsaAccess/FsaAccessSpare16.h"
#include "common/MediaticData/mediaticData.h"
#include "common/QsLog/QsLogCategories.h"
// #include "common/QsLog/QsLog.h"
// #include "common/QsLog/QsLogDest.h"
// #include "common/QsLog/QsDebugOutput.h"
#include "common/tools/FileUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::FsaAccess;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::Common::XMLConfigurationFiles;
// using namespace Lima::LinguisticProcessing;
// using namespace Lima::LinguisticProcessing::MorphologicAnalysis;
using namespace Lima::LinguisticProcessing::AnalysisDict;

void EnhancedAnalysisDictionaryTest0::initTestCase()
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

// conversion functions
void EnhancedAnalysisDictionaryTest0::test()
{
  QStringList configDirs = buildConfigurationDirectoriesList(QStringList() << "lima",QStringList());
  QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  // initialize minimal resources to test EnhancedDict = globalFsaAccess + mainDictionary
  QString lpConfFile = findFileInPaths(configPath, "lima-lp-fre.xml");

 /* content of file is:
  <modulesConfig>
  <module name="Resources">
    <group name="FsaStringsPool">
      <param key="mainKeys" value="globalFsaAccess"/>
    </group>
    <group name="mainDictionary" class="EnhancedAnalysisDictionary">
      <param key="accessKeys" value="globalFsaAccess"/>
      <param key="dictionaryValuesFile" value="LinguisticProcessings/fre/dicoDat-fre.dat"/>
    </group>
    <group name="globalFsaAccess" class="FsaAccess">
      <param key="keyFile" value="LinguisticProcessings/fre/dicoKey-fre.dat"/>
    </group>
   </module>
</modulesConfig>
*/
  XMLConfigurationFileParser langParser(lpConfFile.toUtf8().constData());
  std::cout << "configure resources for language fre" << std::endl;
  auto keyFileName = findFileInPaths(
    MediaticData::MediaticData::single().getResourcesPath().c_str(), 
    langParser.getModuleGroupParamValue("Resources",
                                        "globalFsaAccess",
                                        "keyFile").c_str());
  auto dataFileName = findFileInPaths(
    MediaticData::MediaticData::single().getResourcesPath().c_str(), 
    langParser.getModuleGroupParamValue("Resources",
                                        "mainDictionary",
                                        "dictionaryValuesFile").c_str());
  std::cout << "Load keys " << keyFileName.toUtf8().constData() << std::endl;
  FsaAccessSpare16* fsaAccess=new FsaAccessSpare16();
  fsaAccess->read(keyFileName.toUtf8().constData());
  
  std::cout << "Register mainkeys" << std::endl;
  MediaId langid = MediaticData::MediaticData::single().getMediaId("fre");
  FsaStringsPool& sp = MediaticData::MediaticData::changeable().stringsPool(langid);
  sp.registerMainKeys(fsaAccess);
  
  std::cout << "Build EnhancedAnalysisDictionary with data: " 
            << dataFileName.toUtf8().constData() << std::endl;
  EnhancedAnalysisDictionary dictionary(&sp,
                                        fsaAccess,
                                        dataFileName.toUtf8().constData());
  
  std::cout << "EnhancedAnalysisDictionary end" << std::endl;
  auto entry = dictionary.getEntry(QString::fromUtf8("le"));
//   auto id = entry.getEntryId();
  QVERIFY( true );
}

QTEST_MAIN(EnhancedAnalysisDictionaryTest0)

