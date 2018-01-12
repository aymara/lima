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
#include <QtCore/QTimer>
#include <QMetaType>

using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace std;

namespace Lima {

// initialize delegate static members
int LimaSimpleClientDelegate::argc = 1;
char* LimaSimpleClientDelegate::argv[2] = {(char*)("LimaSimpleClientDelegate"), NULL};
QCoreApplication* LimaSimpleClientDelegate::app=nullptr;
//QThread* LimaSimpleClientDelegate::thread=nullptr;
boost::thread* LimaSimpleClientDelegate::thread=nullptr;
LimaWorker* LimaSimpleClientDelegate::m_worker=nullptr;
LimaController* LimaSimpleClientDelegate::m_controller=nullptr;

// needed to pass string in signals/slots
int metatype_string_id=qRegisterMetaType<std::string>("std::string");

// utility function for debug
std::string getThreadId()
{
  boost::thread::id id=boost::this_thread::get_id();
  //std::thread::id id = std::this_thread::get_id();
  ostringstream tid;
  tid << id; 
  return tid.str();
}

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
LimaWorker::LimaWorker(QObject* parent):
QObject(parent),
m_language(),
m_pipeline(),
m_client(nullptr),
m_handler(nullptr),
m_handlers(),
m_firstInitialization(true)
{
}

LimaWorker::~LimaWorker()
{
  //cout << getThreadId() << " LimaWorker destructor" << endl;
  if (m_handler!=nullptr) {
    delete m_handler;
  }
}

void LimaWorker::quit()
{
  // clear the linguistic resources: must be done inside the worker thread because they use QObjects
  // (otherwise, warning: QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread)
  Lima::LinguisticProcessing::LinguisticResources::changeable().clearResources();
}

LimaController::LimaController(QObject* parent):
QObject(parent),
m_finishedInit(false),
m_finishedAnalyze(false)
{
}

LimaController::~LimaController()
{
}

void LimaController::stop()
{
  //cout << getThreadId() << " LimaController::stop()" << endl;
  Q_EMIT(closeWorker());
  Q_EMIT(closeApp());
}

LimaSimpleClientDelegate::LimaSimpleClientDelegate()
{
  // initialize the internal thread
  if (thread == NULL)
  {
    thread = new boost::thread(LimaSimpleClientDelegate::onStarted);
    //cout << "thread created: "<< thread << endl;
    while (! m_worker) {
      sleep(0.1);
    }
    if (m_worker) {
      //cout << "create connections" << endl;
    }
  }
}

LimaSimpleClientDelegate::~LimaSimpleClientDelegate()
{
  //cout << getThreadId() << " LimaSimpleClientDelegate destructor" << endl;
  // do not join lightly: app.exec() does not finish
  // first stop the app and delete the linguistic resources associated to the worker
  m_controller->stop();
  // then wait for the thread to finish (so that everything is properly stopped before deleting the worker and controller
  thread->join();
  if (m_worker) {
    delete m_worker;
  }
  if (m_controller) {
    delete m_controller;
  }
}

void LimaSimpleClientDelegate::onStarted()
{
  //cout << "LimaSimpleClientDelegate::onStarted begin" << endl;
   if (QCoreApplication::instance() == NULL)
   {
     app = new QCoreApplication(argc, argv);
     
     // Task parented to the application so that it
     // will be deleted by the application.
     m_worker=new LimaWorker();
     m_controller=new LimaController();
    
     // connectors to call the functions of the worker
     QObject::connect(m_controller, SIGNAL(doInitialize(std::string,std::string)), m_worker, SLOT(initialize(std::string,std::string)));
     QObject::connect(m_controller, SIGNAL(doAnalyze(std::string)), m_worker, SLOT(analyze(std::string)));
     QObject::connect(m_worker,  SIGNAL(finishedInit()), m_controller, SLOT(endInit()));
     QObject::connect(m_worker,  SIGNAL(finishedAnalyze()), m_controller, SLOT(endAnalyze()));
     QObject::connect(m_controller,  SIGNAL(closeApp()), app, SLOT(quit()));
     QObject::connect(m_controller,  SIGNAL(closeWorker()), m_worker, SLOT(quit()));
     
     app->exec();
  }
  //cout << "LimaSimpleClientDelegate::onStarted end" << endl;
}

void LimaController::initialize(const std::string& language,
                                  const std::string& pipeline)
{
  //cout << getThreadId() << " LimaController::initialize" << endl;
  // pass the command to the worker in the thread, through a signal
  m_finishedInit=false;
  Q_EMIT(doInitialize(language,pipeline));
}

void LimaSimpleClientDelegate::initialize(const std::string& language,
                                          const std::string& pipeline)
{
  //cout << "LimaSimpleClientDelegate::initialize" << endl;
  //cout << ",controller=" << m_controller << ",worker=" << m_worker << endl;
  m_controller->initialize(language,pipeline);
  // wait until initialization is finished
  while (! m_controller->hasFinishedInit()) {
    //cerr << "waiting for initialization to finish" << endl;
    boost::this_thread::sleep( boost::posix_time::milliseconds(1000) );
  }
}

void LimaController::analyze(const std::string& text)
{
  //cout << getThreadId() << " LimaController::analyze" << endl;
  // pass the command to the worker in the thread, through a signal
  m_finishedAnalyze=false;
  Q_EMIT(doAnalyze(text));
}

std::string LimaSimpleClientDelegate::analyze(const std::string& text)
{
  //cout << getThreadId() << " LimaSimpleClientDelegate::analyze" << endl;
  // pass the command to the worker in the thread, through a signal
  m_controller->analyze(text);
  while (! m_controller->hasFinishedAnalyze()) {
    boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
  }
  return m_worker->getResult();
}

void LimaWorker::initialize(const std::string& language,
                            const std::string& pipeline)
{
  //cout << getThreadId() << " LimaWorker::initialize " << m_firstInitialization << endl;
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
  else {
    cerr << "Factory already initialized" << endl;
  }

  m_client = std::dynamic_pointer_cast<AbstractLinguisticProcessingClient>(LinguisticProcessingClientFactory::single().createClient(clientId));
  
  Q_EMIT(finishedInit());
}

void LimaWorker::analyze(const std::string& text)
{
  //cout << getThreadId() << " LimaWorker::analyze start" << endl;
  std::map<std::string,std::string> metaData;
  metaData["Lang"]=m_language;
  string pseudofilename=text.substr(0,10);
  boost::regex_replace(pseudofilename,boost::regex("[.,-;:!?\",& ]"),"_",boost::format_all);
  metaData["FileName"]=pseudofilename;
  
  ostringstream oss;
  m_handler->setOut(&oss);
  m_client->analyze(text, metaData, m_pipeline, m_handlers);
  //cout << getThreadId() << " LimaWorker::analyze end" << endl;
  //cout << oss.str();
  //cout << getThreadId() << " LimaWorker::analyze emit setResult" << endl;
  m_result=oss.str();
  Q_EMIT(finishedAnalyze());
}

  
} // end namespace
