#include "LangDetector.h"
#include "LangDetectorCore.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include <iostream>
#include <QtCore/QString>

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;

namespace Lima::LinguisticProcessing::LDetector
{

static SimpleFactory<MediaProcessUnit,LangDetector> langdetectorFactory(LANGDETECTOR_CLASSID); // clazy:exclude=non-pod-global-static

LangDetector::LangDetector():m_d(new LangDetectorCore())
{

}

LangDetector::~LangDetector()
{
  delete m_d;
}


void LangDetector::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration, Manager* manager)
{
    QString fileName=Common::Misc::findFileInPaths(Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),unitConfiguration.getParamsValueAtKey("langDetectorModel").c_str());
    m_d->loadModel(fileName.toStdString());
}

LimaStatusCode LangDetector::process(AnalysisContent &analysis) const {
    LANGDETECTORLOGINIT;
    LINFO << "start langdetector process";
    QString* originalText=dynamic_cast<QString*>(analysis.getData("Text").get());
    std::string langLabel = m_d->detectLang(originalText->toStdString());
    std::string language = Lima::LinguisticProcessing::LDetector::LangDetectorCore::labelToPrintable(langLabel);
    LINFO << "The detected language is: "<<language;
    return SUCCESS_ID;
}


}
