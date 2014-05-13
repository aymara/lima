
#include "common/LimaCommon.h"
// #include "common/linguisticData/linguisticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"

#include <QtCore/QCoreApplication>

#include <string>
#include <vector>
#include <iostream>

#include "common/MediaticData/mediaticData.h"

#include "linguisticProcessing/common/linguisticData/languageData.h"

#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"

using namespace Lima::Common::MediaticData;
using namespace std;

void usage(int argc, char* argv[]);

int main(int argc,char* argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  
  std::string resourcesPath=string(getenv("LIMA_RESOURCES"));
  std::string configDir=string(getenv("LIMA_CONF"));
  std::string configFile=string("lima-common.xml");

  std::deque<std::string> langs;

  if (argc>1)
  {
    for (int i = 1 ; i < argc; i++)
    {
      QString arg = QString::fromUtf8(argv[i]);
      int pos = -1;
      if ( arg[0] == '-' )
      {
        if (arg == "--help")
          usage(argc, argv);
        else if ( (arg.contains("--config-file=")) )
          configFile = arg.mid(pos+14).toUtf8().data();
        else if ( (arg.contains("--config-dir=")) )
          configDir = arg.mid(pos+13).toUtf8().data();
        else if ( (arg.contains("--resources-dir=")) )
          resourcesPath = arg.mid(pos+16).toUtf8().data();
        else usage(argc, argv);
      }
      else
      {
        langs.push_back(arg.toUtf8().data());
      }
    }
  }

  MediaticData::changeable().init(resourcesPath,configDir,configFile,langs);

  // const Lima::Common::MediaticData::LanguageData& languageData = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language))
  std::string m_language = langs[0];
  const Lima::Common::MediaticData::LanguageData& languageData = static_cast<const Lima::Common::MediaticData::LanguageData&>(Lima::Common::MediaticData::MediaticData::single().mediaData(m_language));

  Lima::Common::PropertyCode::PropertyCodeManager propertyManager = languageData.getPropertyCodeManager();
  
  const Lima::Common::PropertyCode::PropertyManager& macroManager = propertyManager.getPropertyManager("L_MACRO");
  
}

void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
  std::cout << "usage: " << argv[0] << " [OPTIONS] [lang1 [lang2 [...]]] " << std::endl;
  std::cout << "\t--resources-dir=</path/to/the/resources> Optional. Default is $LIMA_RESOURCES" << std::endl;
  std::cout << "\t--config-dir=</path/to/the/configuration/directory> Optional. Default is $LIMA_CONF" << std::endl;
  std::cout << "\t--config-file=<configuration/file/name>\tOptional. Default is lima-common.xml" << std::endl;
  std::cout << "\twhere langs are languages to load. If no languages, load all." << std::endl;
  exit(0);
}

