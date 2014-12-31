#include "LimaDBusServer.h"
#include "AnalysisWrapper.h"

// Generated
#include "limadbusserveradaptor.h"

#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/misc/gregoriannowarn.hpp"
#include "common/misc/posix_timenowarn.hpp"
#include "common/time/traceUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/MediaProcessors/MediaAnalysisDumper.h"

// definition of logger
#include "linguisticProcessing/LinguisticProcessingCommon.h"
// factories
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/client/AbstractLinguisticProcessingClient.h"


#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QCoreApplication>
#include <QTimer>
#include <QDBusConnection>

// #include <stdlib.h>
// #include <boost/graph/buffer_concepts.hpp>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing;

class LimaDBusServerPrivate
{
  friend class LimaDBusServer;
  LimaDBusServerPrivate( const std::string& limaServerConfigFile,
              const std::deque<std::string>& langs,
              const std::deque<std::string>& pipelines,
              LimaDBusServer* p);
  virtual ~LimaDBusServerPrivate();

  const std::set<std::string> m_langs;
  QTimer m_timer;
  AnalysisWrapper* m_analyzer;
//   QSharedPointer< Lima::LinguisticProcessing::AbstractLinguisticProcessingClient > m_analyzer;
};


LimaDBusServerPrivate::LimaDBusServerPrivate( const std::string& configDir,
                                const std::deque<std::string>& langs,
                                const std::deque<std::string>& pipelines,
                                LimaDBusServer* p)
     : m_langs(langs.begin(),langs.end()), m_timer()
{
  CORECLIENTLOGINIT;
  LDEBUG << "::LimaDBusServer::LimaDBusServer()...";
  
  // initialize common
  std::string resourcesPath = qgetenv("LIMA_RESOURCES").constData();
  if( resourcesPath.empty() )
    resourcesPath = "/usr/share/apps/lima/resources/";
  std::string commonConfigFile("lima-common.xml");

  std::ostringstream oss;
  std::ostream_iterator<std::string> out_it (oss,", ");
  std::copy ( langs.begin(), langs.end(), out_it );
  LDEBUG << "LimaDBusServer::LimaDBusServer: init MediaticData("
           << "resourcesPath=" << resourcesPath
           << "configDir=" << configDir
           << "commonConfigFile=" << commonConfigFile
           << "langs=" << oss.str();
  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);
  
  // initialize linguistic processing
  std::string clientId("lima-coreclient");
  std::string lpConfigFile("lima-analysis.xml");
  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configDir + "/" + lpConfigFile);

  LDEBUG << "LimaDBusServer::LimaDBusServer: configureClientFactory...";
  LinguisticProcessingClientFactory::changeable().configureClientFactory(
    clientId,
    lpconfig,
    langs,
    pipelines);
  
  LDEBUG << "LimaDBusServer::LimaDBusServer: createClient...";
  m_analyzer=new AnalysisWrapper(static_cast<AbstractLinguisticProcessingClient*>(LinguisticProcessingClientFactory::single().createClient(clientId)),m_langs,p);

}

LimaDBusServerPrivate::~LimaDBusServerPrivate()
{
  delete Common::MediaticData::MediaticData::pchangeable();
  delete LinguisticProcessingClientFactory::pchangeable();
  // Do  not delete m_analyzer. It is a QObject with the public object as parent
  // delete m_analyzer;
}

LimaDBusServer::LimaDBusServer( const std::string& configDir,
                                const std::deque<std::string>& langs,
                                const std::deque<std::string>& pipelines,
                                int serviceLife,
                                QObject *parent)
     : QObject(parent), m_d(new LimaDBusServerPrivate(configDir,langs,pipelines,this))
{
  CORECLIENTLOGINIT;
  LDEBUG << "LimaDBusServer::LimaDBusServer()" << serviceLife;

  if (serviceLife > 0)
  {
    // Stop server and app after service-life seconds
    connect (&m_d->m_timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    m_d->m_timer.start(serviceLife);
  }

  new LimaDBusServerAdaptor(this);
  QDBusConnection dbus = QDBusConnection::sessionBus();
  dbus.registerObject("/LimaDBusServer", this);
  dbus.registerService("fr.cea.LimaDBusServer");
}

LimaDBusServer::~LimaDBusServer()
{
  CORECLIENTLOGINIT;
  LINFO << "LimaDBusServer::~LimaDBusServer";
  delete m_d;
}

QString LimaDBusServer::handleRequest(const QString& req, const QString& language, const QString& pipeline)
{
  CORECLIENTLOGINIT;
  LDEBUG << "LimaDBusServer::handleRequest" << req.left(100);
  return m_d->m_analyzer->analyze(req, language, pipeline);
}

