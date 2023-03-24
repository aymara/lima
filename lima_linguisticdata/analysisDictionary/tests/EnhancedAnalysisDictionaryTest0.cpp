// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    Lima::Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
    langParser.getModuleGroupParamValue("Resources",
                                        "globalFsaAccess",
                                        "keyFile").c_str());
  auto dataFileName = findFileInPaths(
    Lima::Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
    langParser.getModuleGroupParamValue("Resources",
                                        "mainDictionary",
                                        "dictionaryValuesFile").c_str());
  std::cout << "Load keys " << keyFileName.toUtf8().constData() << std::endl;
  auto fsaAccess = std::make_shared<FsaAccessSpare16>();
  fsaAccess->read(keyFileName.toUtf8().constData());

  std::cout << "Register mainkeys" << std::endl;
  MediaId langid = Lima::Common::MediaticData::MediaticData::single().getMediaId("fre");
  FsaStringsPool& sp = Lima::Common::MediaticData::MediaticData::changeable().stringsPool(langid);
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

