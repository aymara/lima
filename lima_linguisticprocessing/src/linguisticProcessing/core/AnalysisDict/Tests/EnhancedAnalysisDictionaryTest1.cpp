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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE EnhancedAnalysisDictionaryTest
#include <boost/test/unit_test.hpp>

#include "common/FsaAccess/FsaAccessSpare16.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "linguisticProcessing/core/AnalysisDict/EnhancedAnalysisDictionary.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractAccessResource.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::FsaAccess;

// create EnhancedAnalysisDictionary
BOOST_AUTO_TEST_CASE( EnhancedAnalysisDictionaryTest1 )
{
/*
 * <modulesConfig>
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
  std::string resourcesPath(getenv("LIMA_RESOURCES")==0?"/usr/share/apps/lima/resources/":getenv("LIMA_RESOURCES"));
  std::string configDir(getenv("LIMA_CONF")==0?"/usr/share/config/lima/":getenv("LIMA_CONF"));
  std::string commonConfigFile("lima-common.xml");
  std::deque<std::string>langs(1,"fre");
  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);

  const MediaticData::MediaData& mediaData = MediaticData::MediaticData::single().mediaData("fre");
  MediaId mediaId = mediaData.getMedia();

  // initialize resources
  std::string lpFreConfFile("lima-lp-res-Enhanced.xml");
  Common::XMLConfigurationFiles::XMLConfigurationFileParser langParser(lpFreConfFile);
  std::cout << "configure resources for language " << *(langs.begin());
  try
  {
    Common::XMLConfigurationFiles::ModuleConfigurationStructure& module=langParser.getModuleConfiguration("Resources");
    LinguisticProcessing::LinguisticResources::changeable().initLanguage(
      mediaId,
      module,
      true); // load main keys
  }
  catch (Common::XMLConfigurationFiles::NoSuchModule& )
  {
    std::cerr << "no module 'Resources' in configuration file " << lpFreConfFile;
    throw InvalidConfiguration("no module 'Resources' in configuration file ");
  }

  const Lima::LinguisticProcessing::AbstractResource* globalFsaAccess=Lima::LinguisticProcessing::LinguisticResources::single().getResource(mediaId,"globalFsaAccess");


/**
@author Benoit Mathieu
*/
  const LinguisticProcessing::AnalysisDict::AbstractAccessResource* aar=static_cast<const LinguisticProcessing::AnalysisDict::AbstractAccessResource*>(globalFsaAccess);
  assert(aar->isMainKeys());
  
  const Lima::LinguisticProcessing::AbstractResource* mainDictionary=Lima::LinguisticProcessing::LinguisticResources::single().getResource(mediaId,"mainDictionary");

  const LinguisticProcessing::AnalysisDict::EnhancedAnalysisDictionary* ead=static_cast<const LinguisticProcessing::AnalysisDict::EnhancedAnalysisDictionary*>(mainDictionary);
  uint64_t nbEntries = ead->getSize();
  std::cout << "nbEntries=" << nbEntries << std::endl;
  BOOST_REQUIRE( ead->getSize() == 660156 );
 

//   
//   std::cout << "register mainkeys" << std::endl;
//   MediaId langid=MediaticData::single().getMediaId(param.language);
//   FsaStringsPool& sp= Common::MediaticData::MediaticData::changeable().stringsPool(langid);
//   sp.registerMainKeys(fsaAccess);
//   
//   std::cout << "build EnhancedAnalysisDictionary... " << std::endl;
//   dictionary=new EnhancedAnalysisDictionary(
//     &sp,
//     fsaAccess,
//     param.dataFileName);
//   
//   std::cout << "EnhancedAnalysisDictionary end" << std::endl;
// 
//   std::pair<DictionarySubWordIterator, DictionarySubWordIterator> proclicticSubWord_it = m_procliticDictionary->getSubWordEntries(0, hyperWordString);
//   BOOST_REQUIRE( fsaAccess->getSize() == 660156 );
}
