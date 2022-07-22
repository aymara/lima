// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE EnhancedAnalysisDictionaryTest
#include <boost/test/unit_test.hpp>

#include "common/FsaAccess/FsaAccessSpare16.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "linguisticProcessing/core/AnalysisDict/EnhancedAnalysisDictionary.h"
#include "linguisticProcessing/core/AnalysisDict/FsaAccessResource.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntry.h"
#include "linguisticProcessing/core/MorphologicAnalysis/MorphoSyntacticDataHandler.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractAccessResource.h"
#include "common/misc/fsaStringsPool.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::FsaAccess;

// create EnhancedAnalysisDictionary and check most of public operation
BOOST_AUTO_TEST_CASE( EnhancedAnalysisDictionaryTest1 )
{
  // Initialize MediaData (for fre)
  //value of LIMA_RESOURCE_DIR is set by find_package(LIMADATA) and LIMALDATAConfig.cmake;
  std::string resourcesPath("/home/gael/Projets/Lima/lima/../Dist/master/debug/share/apps/lima/resources");
  std::string configDir(getenv("LIMA_CONF")==0?"/home/gael/Projets/Lima/lima/../Dist/master/debug/usr/share/config/lima/":getenv("LIMA_CONF"));
  std::string commonConfigFile("lima-common.xml");
  std::deque<std::string>langs(1,"fre");
  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);

  const MediaticData::MediaData& mediaData = MediaticData::MediaticData::single().mediaData("fre");
  MediaId mediaId = mediaData.getMedia();

  // initialize minimal resources to test EnhancedDict = globalFsaAccess + mainDictionary
  std::string lpFreConfFile("lima-lp-res-Enhanced.xml");
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
  Common::XMLConfigurationFiles::XMLConfigurationFileParser langParser(lpFreConfFile);
  std::cout << "configure resources for language " << *(langs.begin());
  try
  {
    Common::XMLConfigurationFiles::ModuleConfigurationStructure& module=langParser.getModuleConfiguration("Resources");
    LinguisticProcessing::LinguisticResources::changeable().initLanguage(
      mediaId,
      module,
      true); // Initialize StringPool with keys of globalFsaAccess
  }
  catch (Common::XMLConfigurationFiles::NoSuchModule& )
  {
    std::cerr << "no module 'Resources' in configuration file " << lpFreConfFile;
    throw InvalidConfiguration("no module 'Resources' in configuration file ");
  }

  // get globalFsaAccess (convert keys to Id)
  const Lima::LinguisticProcessing::AbstractResource* globalFsaAccess=Lima::LinguisticProcessing::LinguisticResources::single().getResource(mediaId,"globalFsaAccess");
  // check if it an AbstractAccessResource
  const LinguisticProcessing::AnalysisDict::AbstractAccessResource* aar=static_cast<const LinguisticProcessing::AnalysisDict::AbstractAccessResource*>(globalFsaAccess);
  BOOST_REQUIRE(aar != 0);
  std::cout << "EnhancedAnalysisDictionaryTest1: test aar != 0 OK\n";
  // check if it isMainKeys
  BOOST_REQUIRE(aar->isMainKeys());
  // check if it a FsaAccessResource
  const Lima::LinguisticProcessing::AnalysisDict::FsaAccessResource* far=static_cast<const Lima::LinguisticProcessing::AnalysisDict::FsaAccessResource*>(globalFsaAccess);
  BOOST_REQUIRE(far != 0);
  std::cout << "EnhancedAnalysisDictionaryTest1: test far != 0 OK\n";
  // get AbstractAccessByString
  const Common::AbstractAccessByString* aas = aar->getAccessByString();
  // check if it a FsaAccess::FsaAccessSpare16*
  const FsaAccess::FsaAccessSpare16* fas=static_cast<const FsaAccess::FsaAccessSpare16*>(aas);
  BOOST_REQUIRE(fas != 0);
  uint64_t nbEntries = fas->getSize();
  std::cout << "nbEntries=" << nbEntries << std::endl;

  // get mainDictionary
  const Lima::LinguisticProcessing::AbstractResource* mainDictionary=Lima::LinguisticProcessing::LinguisticResources::single().getResource(mediaId,"mainDictionary");
  // check if it a AbstractAnalysisDictionary
  const LinguisticProcessing::AnalysisDict::AbstractAnalysisDictionary* aad=static_cast<const LinguisticProcessing::AnalysisDict::AbstractAnalysisDictionary*>(mainDictionary);
  BOOST_REQUIRE( aad != 0 );
  // check if it a EnhancedAnalysisDictionary
  const LinguisticProcessing::AnalysisDict::EnhancedAnalysisDictionary* ead=static_cast<const LinguisticProcessing::AnalysisDict::EnhancedAnalysisDictionary*>(mainDictionary);
  BOOST_REQUIRE( ead != 0 );

  // test uint64_t getSize() const;
  // check size is size of accessKeys
  BOOST_REQUIRE( ead->getSize() == nbEntries );

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
  const Lima::LimaString entryKey("porte");
  // get id for key entryKey
  StringsPoolIndex id = static_cast<StringsPoolIndex>(aas->getIndex(entryKey));
  // get entry for key entryKey
  LinguisticProcessing::AnalysisDict::DictionaryEntry entry1 = ead->getEntry(entryKey);
  // check that id of entry is id of entryKey
  BOOST_REQUIRE( id == entry1.getEntryId() );
  
  BOOST_REQUIRE( !entry1.isEmpty() );
  BOOST_REQUIRE( !entry1.isFinal() );
  BOOST_REQUIRE( entry1.hasLingInfos() );
    
  // test DictionaryEntry getEntry(const StringsPoolIndex wordId) const;
  LinguisticProcessing::AnalysisDict::DictionaryEntry entry2 = ead->getEntry(id);
  BOOST_REQUIRE( id == entry2.getEntryId() );
  // test DictionaryEntry getEntry(const StringsPoolIndex wordId, const Lima::LimaString& word) const;
  LinguisticProcessing::AnalysisDict::DictionaryEntry entry3 = ead->getEntry(id,entryKey);
  BOOST_REQUIRE( id == entry3.getEntryId() );

  // void parseLingInfos(AbstractDictionaryEntryHandler* handler) const;
  // bool hasConcatenated() const;
  // void parseConcatenated(AbstractDictionaryEntryHandler* handler) const;
  // bool hasAccentedForms() const;
  // void parseAccentedForms(AbstractDictionaryEntryHandler* handler) const;
  LinguisticProcessing::LinguisticAnalysisStructure::MorphoSyntacticData msd;
  LinguisticProcessing::MorphologicAnalysis::MorphoSyntacticDataHandler morphoSyntacticDataHandler(msd,Lima::LinguisticProcessing::LinguisticAnalysisStructure::SIMPLE_WORD);
  LinguisticProcessing::AnalysisDict::AbstractDictionaryEntryHandler* lingInfoHandler=&morphoSyntacticDataHandler;
  entry1.parseLingInfos(lingInfoHandler);

  // std::set<StringsPoolIndex> allInflectedForms() const;
  // std::set<StringsPoolIndex> allLemma() const;
  // std::set<StringsPoolIndex> allNormalizedForms() const;

  std::set<StringsPoolIndex> forms; 
  forms = msd.allInflectedForms();
  std::set<StringsPoolIndex>::const_iterator formsIt = forms.begin();
  std::cout << "allInflectedForms: ";
  for( ; formsIt != forms.end() ; formsIt++ ) {
    StringsPoolIndex formId = *formsIt;
    LimaString formLimaString = aas->getSpelling(formId);
    std::string formStdString = Lima::Common::Misc::limastring2utf8stdstring(formLimaString);
    std::cout << formStdString << ", ";
  }
  std::cout << std::endl;
  
  std::set<StringsPoolIndex> lemmas; 
  lemmas = msd.allLemma();
  std::set<StringsPoolIndex>::const_iterator lemmasIt = lemmas.begin();
  std::cout << "allLemma: ";
  for( ; lemmasIt != lemmas.end() ; lemmasIt++ ) {
    StringsPoolIndex lemmaId = *lemmasIt;
    LimaString lemmaLimaString = aas->getSpelling(lemmaId);
    std::string lemmaStdString = Lima::Common::Misc::limastring2utf8stdstring(lemmaLimaString);
    std::cout << lemmaStdString << ", ";
  }
  std::cout << std::endl;
  
  std::set<StringsPoolIndex> normalized; 
  normalized = msd.allNormalizedForms();
  std::cout << "allNormalizedForms: ";
  std::set<StringsPoolIndex>::const_iterator normalizedIt = normalized.begin();
  for( ; normalizedIt != normalized.end() ; normalizedIt++ ) {
    StringsPoolIndex normalizedId = *normalizedIt;
    LimaString normalizedLimaString = aas->getSpelling(normalizedId);
    std::string normalizedStdString = Lima::Common::Misc::limastring2utf8stdstring(normalizedLimaString);
    std::cout << normalizedStdString << ", ";
  }
  std::cout << std::endl;
  
}
