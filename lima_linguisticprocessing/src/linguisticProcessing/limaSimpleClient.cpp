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
  // -> do not remove existing delegate: contains the thread and the objects that communicate inside the thread
  // (QCoreApplication, LimaController and LimaWorker)
  //if (m_delegate!=nullptr) {
  //  delete m_delegate;
  //}
  if (m_delegate==nullptr) {
    m_delegate=new LimaSimpleClientDelegate();
  }
  // just relaunch initialize
  m_delegate->initialize(language,pipeline);
  
}

std::string LimaSimpleClient::analyze(const std::string& text)
{
  return m_delegate->analyze(text);
}

} // end namespace
