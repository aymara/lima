/*
    Copyright 2015 CEA LIST

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
#include "PropertyCodeTest0.h"

#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/MediaticData/mediaticData.h"
// #include "common/QsLog/QsLog.h"
// #include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
// #include "common/QsLog/QsDebugOutput.h"
#include "common/tools/FileUtils.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"

using namespace Lima;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::Common::PropertyCode;
// using namespace Lima::LinguisticProcessing;

void PropertyCodeTest0::initTestCase()
{
//   QStringList configDirs = buildConfigurationDirectoriesList(QStringList()
//       << "lima",QStringList());
//   QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);
//
//   QStringList resourcesDirs = buildResourcesDirectoriesList(QStringList()
//       << "lima",QStringList());
//   QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);
//
//   std::string commonConfigFile=std::string("lima-common.xml");
//
//   std::deque<std::string> langs;
//   langs.push_back("eng.ud");
//
//   QsLogging::initQsLog(configPath);
//   // Necessary to initialize factories
//   Lima::AmosePluginsManager::single();
//   Lima::AmosePluginsManager::changeable().loadPlugins(configPath);
//
//   // initialize common
//   Common::MediaticData::MediaticData::changeable().init(
//       resourcesPath.toUtf8().constData(),
//       configPath.toUtf8().constData(),
//       commonConfigFile,
//       langs);
}

void PropertyCodeTest0::test_load()
{
  qDebug() << "PropertyCodeTest0::test_load";
  PropertyCodeManager pcm;
  QVERIFY_EXCEPTION_THROWN(
    pcm.readFromXmlFile("/this/file/does/not/exist"),
    Lima::LimaException
  );

  QString dataFile = QFINDTESTDATA("code-ud-eng.xml");

  pcm.readFromXmlFile(dataFile.toUtf8().constData());
  auto paMicro = pcm.getPropertyAccessor("MICRO");
  auto pmMicro = pcm.getPropertyManager("MICRO");
  LinguisticCode code = pmMicro.getPropertyValue("NOUN");

  QVERIFY( pmMicro.getPropertySymbolicValue(code) == "NOUN" );

  LinguisticCode encoded = pcm.encode({
    {"MACRO", "NOUN"},
    {"MICRO", "NOUN"},
  });
  QVERIFY( pmMicro.getPropertySymbolicValue(encoded) == "NOUN" );

}

QTEST_GUILESS_MAIN(PropertyCodeTest0)

