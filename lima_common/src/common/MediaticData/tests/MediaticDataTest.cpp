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
#define BOOST_TEST_MODULE MediaticData
#include <boost/test/unit_test.hpp>

#include "common/MediaticData/mediaticData.h"

#include "common/time/traceUtils.h"
#include "common/QsLog/QsLog.h"
#include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/QsLog/QsDebugOutput.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"

using namespace Lima;

// conversion functions
BOOST_AUTO_TEST_CASE( MediaticData0 )
{
  QsLogging::initQsLog();
  Lima::AmosePluginsManager::single();
  
  std::string resourcesPath;
  std::string configDir;
  std::string commonConfigFile("lima-common.xml");
  std::deque<std::string> langs;
  langs.push_front("fre");
  
  resourcesPath = std::string (qgetenv("LIMA_RESOURCES").constData()==0?"":qgetenv("LIMA_RESOURCES").constData());
  if (resourcesPath.empty())
  {
    resourcesPath = "/usr/share/apps/lima/resources/";
  }
  std::cerr << "MediaticData0: resourcesPath=" << resourcesPath << std::endl;

  configDir = std::string (qgetenv("LIMA_CONF").constData()==0?"":qgetenv("LIMA_CONF").constData());
  if (configDir.empty())
  {
    configDir = "/usr/share/config/lima";
  }
  std::cerr << "MediaticData0: configDir=" << configDir << std::endl;
  
  // initialize common
  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);
  
  // use setter: Create LinguisticProcessing.IDIOM and LinguisticProcessing.SYNTACTIC_RELATION
  LimaString groupName1("LinguisticProcessing");
  Common::MediaticData::EntityGroupId group1 = Common::MediaticData::MediaticData::changeable().addEntityGroup(groupName1);
  LimaString entityName11("IDIOM");
  LimaString entityName12("SYNTACTIC_RELATION");
  Common::MediaticData::EntityType type11 = Common::MediaticData::MediaticData::changeable().addEntity(groupName1,entityName11);
  Common::MediaticData::EntityType type12 = Common::MediaticData::MediaticData::changeable().addEntity(groupName1,entityName12);
  
  // use setter: Create Location.CITYand Location.COUNTRY
  LimaString groupName2("Location");
  Common::MediaticData::EntityGroupId group2 = Common::MediaticData::MediaticData::changeable().addEntityGroup(groupName2);
  LimaString entityName21("CITY");
  LimaString entityName22("COUNTRY");
  Common::MediaticData::EntityType type21 = Common::MediaticData::MediaticData::changeable().addEntity(groupName2,entityName21);
  Common::MediaticData::EntityType type22 = Common::MediaticData::MediaticData::changeable().addEntity(groupName2,entityName22);
  
  // test getter: get groupId from name
  Lima::Common::MediaticData::EntityGroupId groupId2 = Common::MediaticData::MediaticData::single().getEntityGroupId(groupName2);
  std::cerr << "groupName2 = " << groupName1 << ", groupId2 = " << groupId2 << std::endl;
  BOOST_REQUIRE( groupId2 == group2 );
  // test getter: get groupName from groupId
  LimaString groupName22 = Common::MediaticData::MediaticData::single().getEntityGroupName(groupId2);
  BOOST_REQUIRE( groupName2 == groupName22);
  
  // test getter: get groupId from name
  Lima::Common::MediaticData::EntityGroupId groupId1 = Common::MediaticData::MediaticData::single().getEntityGroupId(groupName1);
  std::cerr << "groupName1 = " << groupName1 << ", groupId1 = " << groupId1 << std::endl;
  BOOST_REQUIRE( groupId1 == group1 );
  // test getter: get groupName from groupId
  BOOST_REQUIRE( groupName1 == Common::MediaticData::MediaticData::single().getEntityGroupName(groupId1));
  
  // test getter: get entity name from entity
  LimaString name11 = Common::MediaticData::MediaticData::single().getEntityName(type11);
  LimaString qualifiedEntityName11("LinguisticProcessing.IDIOM");
  std::cerr << "name11 = " << name11 << std::endl;
  BOOST_REQUIRE(name11==qualifiedEntityName11);
  
  // test getter: get entity type from name
  LimaString qualifiedEntityName21("Location.CITY");
  LimaString simpleEntityName21("CITY");
  Lima::Common::MediaticData::EntityType entityType211 = Common::MediaticData::MediaticData::single().getEntityType(qualifiedEntityName21);
  BOOST_REQUIRE(entityType211 == type21);
  Lima::Common::MediaticData::EntityType entityType212 = Common::MediaticData::MediaticData::single().getEntityType(groupId2, simpleEntityName21);
  BOOST_REQUIRE(entityType211 == type21);

 }
