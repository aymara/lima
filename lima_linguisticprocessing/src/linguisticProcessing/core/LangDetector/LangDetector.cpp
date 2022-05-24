#include "LangDetector.h"
#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"
#include <iostream>
#include <sstream>
#include <QtCore/QString>
#include "fasttext/fasttext.h"

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;

namespace Lima::LinguisticProcessing::LDetector
{

static SimpleFactory<MediaProcessUnit,LangDetector> langdetectorFactory(LANGDETECTOR_CLASSID); // clazy:exclude=non-pod-global-static

class LangDetectorPrivate
{
public:
  LangDetectorPrivate();
  virtual ~LangDetectorPrivate();

  [[nodiscard]] string detectLang(const QString* sentence) const;
  /*Used to convert a language label "__label__lang" to a printable format "lang".*/
  [[nodiscard]] static string labelToPrintable(const string& label) ;

  fasttext::FastText ftext;

};

LangDetectorPrivate::LangDetectorPrivate()
= default;

LangDetectorPrivate::~LangDetectorPrivate() = default;

string LangDetectorPrivate::detectLang(const QString* sentence) const
{
    string lang;
    std::vector<std::pair<fasttext::real, std::string>> prediction;
    std::stringstream ioss(sentence->toStdString() + std::string("\n"));
    ftext.predictLine(ioss,prediction,1,0);
    lang = prediction.front().second;
    return lang;
}

string LangDetectorPrivate::labelToPrintable(const string& label)
{
    return label.substr(9,string::npos);
}

LangDetector::LangDetector():m_d(new LangDetectorPrivate())
{

}

LangDetector::~LangDetector()
{
  delete m_d;
}


void LangDetector::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration, Manager* manager)
{
    QString fileName=Common::Misc::findFileInPaths(Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),unitConfiguration.getParamsValueAtKey("langDetectorModel").c_str());
    m_d->ftext.loadModel(fileName.toStdString());
}

LimaStatusCode LangDetector::process(AnalysisContent &analysis) const {
    LANGDETECTORLOGINIT;
    LINFO << "start langdetector process";
    QString* originalText=dynamic_cast<QString*>(analysis.getData("Text"));
    string langLabel = m_d->detectLang(originalText);
    string language = m_d->labelToPrintable(langLabel);
    LINFO << "The detected language is: "<<language;
    return SUCCESS_ID;
}


}