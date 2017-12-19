/************************************************************************
 *
 * @file       limaSimpleClient.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Dec 15 2017
 * copyright   Copyright (C) 2017 by CEA - LIST
 * 
 ***********************************************************************/

#include "limaSimpleClient.h"
#include "limaSimpleClientDelegate.h"

#include "common/LimaCommon.h"
#include "common/LimaVersion.h"
#include "common/tools/LimaMainTaskRunner.h"
#include "common/MediaticData/mediaticData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"
#include "common/QsLog/QsLog.h"
#include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/QsLog/QsDebugOutput.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"

#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <boost/algorithm/string/regex.hpp>

using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace std;

namespace Lima {

// initialize delegate static members
int LimaSimpleClientDelegate::argc = 1;
char* LimaSimpleClientDelegate::argv[2] = {(char*)("LimaSimpleClientDelegate"), NULL};
QCoreApplication* LimaSimpleClientDelegate::app=nullptr;
QThread* LimaSimpleClientDelegate::thread=nullptr;

//***********************************************************************
LimaSimpleClient::LimaSimpleClient():
m_delegate(nullptr)
{
}

LimaSimpleClient::~LimaSimpleClient()
{
  if (m_delegate!=nullptr) {
    delete m_delegate;
  }
}

void LimaSimpleClient::initialize(const std::string& language,
                                   const std::string& pipeline)
{
  // remove existing one if exist, to ensure proper initialization
  if (m_delegate!=nullptr) {
    delete m_delegate;
  }
  m_delegate=new LimaSimpleClientDelegate();
  m_delegate->initialize(language,pipeline);
}

std::string LimaSimpleClient::analyze(const std::string& text)
{
  return m_delegate->analyze(text);
}

//***********************************************************************
LimaSimpleClientDelegate::LimaSimpleClientDelegate():
m_language(),
m_pipeline(),
m_client(nullptr),
m_handler(nullptr),
m_handlers(),
m_firstInitialization(true)
{
  // initialize the internal thread
  if (thread == NULL)
  {
    thread = new QThread();
    connect(thread, SIGNAL(started()), this, SLOT(onStarted()), Qt::DirectConnection);
    thread->start();
  }
}

LimaSimpleClientDelegate::~LimaSimpleClientDelegate()
{
  if (m_handler!=nullptr) {
    delete m_handler;
  }
}

void LimaSimpleClientDelegate::onStarted()
{
   if (QCoreApplication::instance() == NULL)
   {
     app = new QCoreApplication(argc, argv);
     app->exec();
   }
}

void LimaSimpleClientDelegate::initialize(const std::string& language,
                                           const std::string& pipeline)
{
  m_language=language;
  m_pipeline=pipeline;
  string clientId("lima-coreclient");

  if (m_firstInitialization) {

    QStringList configDirs = buildConfigurationDirectoriesList(QStringList() << "lima",QStringList());
    QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);
    
    QStringList resourcesDirs = buildResourcesDirectoriesList(QStringList() << "lima",QStringList());
    QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

    // default values for lima configuration
    string commonConfigFile("lima-common.xml");
    string lpConfigFile("lima-analysis.xml");
    
    QsLogging::initQsLog(configPath);
    // Necessary to initialize factories
    Lima::AmosePluginsManager::single();
    Lima::AmosePluginsManager::changeable().loadPlugins(configPath);
    
    std::deque<std::string> langs(1,language);
    std::deque<std::string> pipelines(1,pipeline);
    
    // initialize common
    Common::MediaticData::MediaticData::changeable().init(
      resourcesPath.toUtf8().constData(),
      configPath.toUtf8().constData(),
      commonConfigFile,
    langs);
    
    bool clientFactoryConfigured = false;
    Q_FOREACH(QString configDir, configDirs)
    {
      if (QFileInfo::exists(configDir + "/" + lpConfigFile.c_str()))
      {
        // initialize linguistic processing
        Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig((configDir + "/" + lpConfigFile.c_str()).toStdString());
        LinguisticProcessingClientFactory::changeable().configureClientFactory(
          clientId,
          lpconfig,
          langs,
          pipelines);
        clientFactoryConfigured = true;
        break;
      }
    }
    if(!clientFactoryConfigured)
    {
      std::cerr << "No LinguisticProcessingClientFactory were configured with" << configDirs.join(LIMA_PATH_SEPARATOR).toStdString() << "and" << lpConfigFile << std::endl;
    }

    m_handler = new SimpleStreamHandler();
    m_handlers.insert(std::make_pair("simpleStreamHandler", m_handler));

    m_firstInitialization=false;
  }
  /*else {
    cerr << "Factory already initialized" << endl;
  }*/

  m_client = std::dynamic_pointer_cast<AbstractLinguisticProcessingClient>(LinguisticProcessingClientFactory::single().createClient(clientId));

}

std::string LimaSimpleClientDelegate::analyze(const std::string& text)
{
  std::map<std::string,std::string> metaData;
  metaData["Lang"]=m_language;
  string pseudofilename=text.substr(0,10);
  boost::regex_replace(pseudofilename,boost::regex("[.,-;:!?\",& ]"),"_",boost::format_all);
  metaData["FileName"]=pseudofilename;
  
  ostringstream oss;
  m_handler->setOut(&oss);
  m_client->analyze(text, metaData, m_pipeline, m_handlers);
  return oss.str();
}

  
} // end namespace
