// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip

#include "EnhancedAnalysisDictionaryTest1.h"

#include "common/FsaAccess/FsaAccessSpare16.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/core/AnalysisDict/EnhancedAnalysisDictionary.h"
#include "linguisticProcessing/core/AnalysisDict/FsaAccessResource.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntry.h"
#include "linguisticProcessing/core/MorphologicAnalysis/MorphoSyntacticDataHandler.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractAccessResource.h"
#include "common/misc/fsaStringsPool.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/MediaticData/mediaticData.h"
// #include "common/QsLog/QsLog.h"
// #include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
// #include "common/QsLog/QsDebugOutput.h"
#include "common/tools/FileUtils.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::FsaAccess;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::MorphologicAnalysis;
using namespace Lima::LinguisticProcessing::AnalysisDict;

void EnhancedAnalysisDictionaryTest1::initTestCase()
{
  QStringList configDirs = buildConfigurationDirectoriesList(QStringList()
      << "lima",QStringList());
  QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  QStringList resourcesDirs = buildResourcesDirectoriesList(QStringList()
      << "lima",QStringList());
  QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  std::string commonConfigFile=std::string("lima-common.xml");

  std::deque<std::string> langs({"eng", "fre", "por"});

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

void EnhancedAnalysisDictionaryTest1::testEng()
{
  testLanguage("eng",
               "lima-lp-eng.xml",
               "door",
               QStringList() << "door",
               QStringList() << "door",
               QStringList() << "door",
               "NOUN");
}

void EnhancedAnalysisDictionaryTest1::testFre()
{
  testLanguage("fre",
              "lima-lp-fre.xml",
              "porte",
              QStringList() << "porte",
              QStringList() << "porte" << "porter",
              QStringList() << "porte" << "porter",
              "NC");

}

void EnhancedAnalysisDictionaryTest1::testPor()
{
  testLanguage("por",
               "lima-lp-por.xml",
               "porta",
               QStringList() << "porta",
               QStringList() << "porta" << "portar",
               QStringList() << "porta" << "portar",
               "N");
}

QTEST_MAIN(EnhancedAnalysisDictionaryTest1)

void EnhancedAnalysisDictionaryTest1::testLanguage(
    const QString& language,
    const QString& lpConfFileName,
    const QString& entryKey,
    const QStringList& refForms,
    const QStringList& refLemmas,
    const QStringList& refNormalized,
    const QString& micro
)
{
  const Lima::Common::MediaticData::MediaData& mediaData = Lima::Common::MediaticData::MediaticData::single().mediaData(language.toUtf8().constData());
  MediaId mediaId = mediaData.getMedia();

  QStringList configDirs = buildConfigurationDirectoriesList(QStringList() << "lima",QStringList());
  QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  QString lpConfFile = findFileInPaths(configPath, lpConfFileName);

  XMLConfigurationFileParser langParser(lpConfFile.toUtf8().constData());
  ModuleConfigurationStructure& module=langParser.getModuleConfiguration("Resources");
  LinguisticResources::changeable().initLanguage(
    mediaId,
    module,
    true); // Initialize StringPool with keys of globalFsaAccess

  // get globalFsaAccess (convert keys to Id)
  auto globalFsaAccess = LinguisticResources::single().getResource(mediaId,"globalFsaAccess");
  // check if it an AbstractAccessResource
  auto aar = dynamic_cast<const AbstractAccessResource*>(globalFsaAccess);
  QVERIFY(aar != nullptr);
  // check if it isMainKeys
  QVERIFY(aar->isMainKeys());
  // check if it a FsaAccessResource
  const FsaAccessResource* far=dynamic_cast<const FsaAccessResource*>(globalFsaAccess);
  QVERIFY(far != nullptr);
  // get AbstractAccessByString
  const Common::AbstractAccessByString* aas = aar->getAccessByString();
  QVERIFY(aas != nullptr);
  // check if it a FsaAccess::FsaAccessSpare16*
  const FsaAccess::FsaAccessSpare16* fas=dynamic_cast<const FsaAccess::FsaAccessSpare16*>(aas);
  QVERIFY(fas != nullptr);

  // get mainDictionary
  const AbstractResource* mainDictionary=LinguisticResources::single().getResource(mediaId,"mainDictionary");
  // check if it a AbstractAnalysisDictionary
  const AbstractAnalysisDictionary* aad=dynamic_cast<const AbstractAnalysisDictionary*>(mainDictionary);
  QVERIFY( aad != nullptr );
  // check if it a EnhancedAnalysisDictionary
  const EnhancedAnalysisDictionary* ead=dynamic_cast<const EnhancedAnalysisDictionary*>(mainDictionary);
  QVERIFY( ead != nullptr );

  // test uint64_t getSize() const;
  // check size is size of accessKeys
  QVERIFY( ead->getSize() == fas->getSize() );

  // test DictionaryEntry getEntry(const Lima::LimaString& word) const;
  // create DictionaryEntry from string or StringPool

/*
 * <entry k="porte">
  <i l="porte">
    <p v="Ncgfs-"/>
  </i>
  <i l="porter">
    <p v="Vpipi3-s"/>
    <p v="Vpipp3-s"/>
    <p v="Vpmpi2-s"/>
    <p v="Vpmpp2-s"/>
  </i>
</entry>
*/
  // get id for key entryKey
  StringsPoolIndex id = static_cast<StringsPoolIndex>(aas->getIndex(entryKey));
  // get entry for key entryKey
  DictionaryEntry entry1 = ead->getEntry(entryKey);
  // check that id of entry is id of entryKey
  QVERIFY( id == entry1.getEntryId() );

  QVERIFY( !entry1.isEmpty() );
  QVERIFY( !entry1.isFinal() );
  QVERIFY( entry1.hasLingInfos() );

  // test DictionaryEntry getEntry(const StringsPoolIndex wordId) const;
  DictionaryEntry entry2 = ead->getEntry(id);
  QVERIFY( id == entry2.getEntryId() );
  // test DictionaryEntry getEntry(const StringsPoolIndex wordId,
  //                                const Lima::LimaString& word) const;
  DictionaryEntry entry3 = ead->getEntry(id,entryKey);
  QVERIFY( id == entry3.getEntryId() );

  // void parseLingInfos(AbstractDictionaryEntryHandler* handler) const;
  // bool hasConcatenated() const;
  // void parseConcatenated(AbstractDictionaryEntryHandler* handler) const;
  // bool hasAccentedForms() const;
  // void parseAccentedForms(AbstractDictionaryEntryHandler* handler) const;
  LinguisticAnalysisStructure::MorphoSyntacticData msd;
  MorphoSyntacticDataHandler morphoSyntacticDataHandler(
    msd,
    LinguisticAnalysisStructure::SIMPLE_WORD);
  entry1.parseLingInfos(&morphoSyntacticDataHandler);

  // std::set<StringsPoolIndex> allInflectedForms() const;
  std::set<StringsPoolIndex> forms = msd.allInflectedForms();
  QStringList sforms;
  for(auto formsIt = forms.cbegin(); formsIt != forms.cend() ; formsIt++ )
  {
    StringsPoolIndex formId = *formsIt;
    LimaString formLimaString = aas->getSpelling(formId);
    sforms << formLimaString;
  }
  QVERIFY(sforms == refForms);

  // std::set<StringsPoolIndex> allLemma() const;
  std::set<StringsPoolIndex> lemmas = msd.allLemma();
  QStringList slemmas;
  for(auto lemmasIt = lemmas.cbegin() ;
      lemmasIt != lemmas.cend() ;
      lemmasIt++ )
  {
    StringsPoolIndex lemmaId = *lemmasIt;
    LimaString lemmaLimaString = aas->getSpelling(lemmaId);
    slemmas << lemmaLimaString;
  }
  QVERIFY(slemmas == refLemmas);

  // std::set<StringsPoolIndex> allNormalizedForms() const;
  std::set<StringsPoolIndex> normalized = msd.allNormalizedForms();
  QStringList snormalized;;
  for(auto normalizedIt = normalized.cbegin() ;
      normalizedIt != normalized.cend() ;
      normalizedIt++ )
  {
    StringsPoolIndex normalizedId = *normalizedIt;
    LimaString normalizedLimaString = aas->getSpelling(normalizedId);
    snormalized << normalizedLimaString;
  }
  QVERIFY(snormalized == refNormalized);

  // Test that one of the MorphoSyntacticData msd elements has its properties
  // LinguisticCode encoding @ref micro as its Micro
  auto& pcm = static_cast<const LanguageData&>(mediaData).getPropertyCodeManager();
  auto& pmMicro = pcm.getPropertyManager("MICRO");
  auto& pa = pmMicro.getPropertyAccessor();

  LinguisticCode nounCode = pmMicro.getPropertyValue(micro.toUtf8().constData());

  bool foundNoun = false;
  for (const auto& element: msd)
  {
    LinguisticCode elementMicro = pa.readValue(element.properties);
    if (elementMicro == nounCode)
    {
      foundNoun = true;
      break;
    }
  }
  QVERIFY( foundNoun );

}
